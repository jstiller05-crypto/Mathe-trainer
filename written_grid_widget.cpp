#include "written_grid_widget.h"
#include "feedback_colors.h"
#include <QPainter>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <algorithm>

static constexpr int PAGE_COLUMNS = 32;

WrittenGridWidget::WrittenGridWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(200, 200);
}

void WrittenGridWidget::setNumberFont(const QString &family)
{
    numberFontFamily = family;
    update();
}

void WrittenGridWidget::setInkColor(const QColor &color)
{
    // Analog zu setNumberFont(): MainWindow kennt (ueber QGuiApplication::styleHints()
    // ->colorScheme()) bereits das aktuelle Theme und reicht die passende Textfarbe
    // hier durch - vorher war Qt::white in paintEvent() fest verdrahtet, im hellen
    // Theme also praktisch unsichtbar auf hellem Hintergrund.
    inkColor = color;
    update();
}

void WrittenGridWidget::showCalculation(const WrittenCalculation &calc)
{
    worksheetTasks.clear();   // Aufgabenblatt-Ansicht beenden, falls aktiv
    geometryModel = GeometryModel();   // Geometrie-Ansicht beenden, falls aktiv (leeres Modell = Modus aus)
    calculation = calc;
    solutionText.clear();   // Rueckmeldung der VORHERIGEN Aufgabe darf hier nicht mehr stehen

    for (QLineEdit *field : answerFields) field->deleteLater();
    answerFields.clear();

    int maxOperandLength = 0;
    for (const QString &op : calculation.operands) {
        maxOperandLength = std::max(maxOperandLength, static_cast<int>(op.length()));
    }
    totalDigitColumns = std::max(maxOperandLength, calculation.answerDigitCount);

    // Kommazahlen und negative Ergebnisse passen nicht in einzelne Ziffern-Kaestchen
    // (jedes Kaestchen fasst genau EIN Zeichen) - dafuer gibt es dann EIN
    // zusammenhaengendes Eingabefeld ueber die volle Antwortbreite.
    int fieldCount = calculation.freeformAnswer ? 1 : calculation.answerDigitCount;

    for (int i = 0; i < fieldCount; ++i) {
        QLineEdit *field = new QLineEdit(this);
        field->setObjectName("writtenAnswerDigit");
        if (!calculation.freeformAnswer) field->setMaxLength(1);
        field->setAlignment(Qt::AlignCenter);
        field->setFrame(false);
        field->installEventFilter(this);
        answerFields.append(field);
        field->show();
    }

    if (!calculation.freeformAnswer) {
        // Schriftliches Rechnen (Stacked) rechnet von der Einer-Stelle aus - nach
        // Eingabe geht der Fokus deshalb nach LINKS (i-1). Beim Kopfrechnen
        // (SingleLine) schreibt man dagegen ganz normal von links nach rechts (i+1).
        bool rightToLeft = (calculation.mode == WrittenCalculation::DisplayMode::Stacked);
        for (int i = 0; i < answerFields.size(); ++i) {
            connect(answerFields[i], &QLineEdit::textChanged, this, [this, i, rightToLeft](const QString &text) {
                if (text.length() != 1) return;
                int nextIndex = rightToLeft ? (i - 1) : (i + 1);
                if (nextIndex >= 0 && nextIndex < answerFields.size()) {
                    answerFields[nextIndex]->setFocus();
                }
            });
        }
    }

    recomputeLayout();
    update();
}

void WrittenGridWidget::showWorksheet(const QVector<Task> &tasks)
{
    for (QLineEdit *field : answerFields) field->deleteLater();
    answerFields.clear();
    totalDigitColumns = 0;   // deaktiviert die normale Einzel-Aufgaben-Zeichnung
    geometryModel = GeometryModel();   // Geometrie-Ansicht beenden, falls aktiv

    worksheetTasks = tasks;
    update();
}

void WrittenGridWidget::showModel(const GeometryModel &newModel)
{
    worksheetTasks.clear();               // andere Anzeige-Modi beenden, wie bei showCalculation()/showWorksheet()
    totalDigitColumns = 0;
    calculation = WrittenCalculation();   // alte Rechnung darf hier nicht mehr nachwirken
    solutionText.clear();

    geometryModel = newModel;

    for (QLineEdit *field : answerFields) field->deleteLater();
    answerFields.clear();

    // Fuer jedes GESUCHTE Label (answerIndex >= 0) ein echtes Eingabefeld anlegen - die
    // Reihenfolge folgt geometryModel.labels, damit layoutGeometryAnswerFields() (gleiche
    // Iteration, gleiche Bedingung) die Felder wieder eindeutig zuordnen kann.
    // Validierung/Auswertung der Eingabe ist HIER bewusst noch nicht Teil des Widgets
    // (siehe written_grid_widget.h) - das Feld muss in diesem Schritt nur sichtbar an der
    // richtigen Stelle sitzen.
    for (const GeometryLabel &label : geometryModel.labels) {
        if (label.answerIndex < 0) continue;

        QLineEdit *field = new QLineEdit(this);
        field->setObjectName("geometryAnswerField");
        field->setAlignment(Qt::AlignCenter);
        field->setFrame(false);
        field->installEventFilter(this);   // Enter loest wie bei den Ziffern-Kaestchen answerSubmitted() aus
        answerFields.append(field);
        field->show();
    }

    qDebug() << "[WrittenGrid] Geometrie-Modell gesetzt - Punkte:" << geometryModel.points.size()
             << "| Kanten:" << geometryModel.edges.size() << "| Labels:" << geometryModel.labels.size()
             << "| Antwortfelder:" << answerFields.size();

    recomputeLayout();
    update();
}

void WrittenGridWidget::recomputeLayout()
{
    // Spalten-/Zeilenbedarf der aktuellen Aufgabe VORAB ermitteln (0, wenn keine
    // Einzelaufgabe aktiv ist, z.B. im Aufgabenblatt-Modus) - wird gleich gebraucht,
    // um zu entscheiden, ob die normale Kaestchengroesse ausreicht.
    int taskColumnsInSquares = 0;
    int taskRowsInSquares = 0;

    if (totalDigitColumns != 0) {
        if (calculation.mode == WrittenCalculation::DisplayMode::SingleLine) {
            // expression enthaelt bereits das abschliessende "=" - jedes Zeichen (auch
            // Leerzeichen, die als blanko Kaestchen mitgezaehlt werden) belegt eine Spalte.
            taskColumnsInSquares = calculation.expression.length() + calculation.answerDigitCount;
            taskRowsInSquares = 2;
        } else {
            taskColumnsInSquares = totalDigitColumns + 1;
            taskRowsInSquares = (calculation.operands.size() + 1) * 2;
        }
    }

    // Normalerweise entspricht eine Kaestchen-Seite 1/32 der Breite (wie kariertes
    // Papier). Braucht eine Aufgabe (z.B. eine lange Kette im "Schwere Aufgabe"-Modus)
    // MEHR als 32 Spalten, wuerde sie sonst mit negativem Offset nach links aus dem
    // sichtbaren Bereich rutschen - deshalb werden die Kaestchen dann verkleinert,
    // sodass die komplette Aufgabe exakt in die Breite passt statt geklemmt zu werden.
    squareSize = (taskColumnsInSquares > PAGE_COLUMNS)
                     ? static_cast<double>(width()) / taskColumnsInSquares
                     : static_cast<double>(width()) / PAGE_COLUMNS;
    pageRows = static_cast<int>(height() / squareSize);

    // Geometrie-Modus hat sein EIGENES Layout (freie Bounding-Box statt Karo-Raster-
    // Spalten/Zeilen) - deshalb hier abzweigen, bevor die Ziffern-Kaestchen-Logik unten
    // greift (die fuer totalDigitColumns==0 sowieso nichts zu tun haette).
    if (!geometryModel.points.isEmpty()) {
        recomputeGeometryLayout();
        return;
    }

    if (totalDigitColumns == 0) return;

    // std::max(0, ...) verhindert bei beiden Achsen einen negativen Offset (vorher
    // war nur die Zeilen-Achse abgesichert) - bei Verkleinerung oben ist der Bedarf
    // zwar rechnerisch gedeckt, das ist trotzdem ein guenstiger Sicherheitsnetz-Fall.
    int startColSquares = std::max(0, (PAGE_COLUMNS - taskColumnsInSquares) / 2);
    int startRowSquares = std::max(0, (pageRows - taskRowsInSquares) / 2);

    gridXOffset = startColSquares * squareSize;
    gridYOffset = startRowSquares * squareSize;

    layoutAnswerFields();
}

void WrittenGridWidget::layoutAnswerFields()
{
    if (answerFields.isEmpty()) return;

    double cellWidth = squareSize;
    double cellHeight = squareSize * 2.0;

    if (calculation.mode == WrittenCalculation::DisplayMode::SingleLine) {
        int answerStartCol = calculation.expression.length();   // expression endet bereits auf "=", direkt danach kommt die Antwort

        if (calculation.freeformAnswer) {
            placeFreeformField(answerStartCol, 0, cellWidth, cellHeight);
            return;
        }

        for (int i = 0; i < answerFields.size(); ++i) {
            double x = gridXOffset + (answerStartCol + i) * cellWidth;
            double y = gridYOffset;
            answerFields[i]->setGeometry(QRect(static_cast<int>(x), static_cast<int>(y),
                                               static_cast<int>(cellWidth), static_cast<int>(cellHeight)));
            answerFields[i]->setStyleSheet(QString("font-size: %1px; font-family: \"%2\"; background: transparent; border: none;")
                                               .arg(static_cast<int>(cellHeight * 0.65)).arg(numberFontFamily));
        }
        return;
    }

    int answerRowIndex = calculation.operands.size();

    if (calculation.freeformAnswer) {
        placeFreeformField(1, answerRowIndex, cellWidth, cellHeight);   // Spalte 1: gleich hinter der Operator-Spalte
        return;
    }

    int answerStartColumn = totalDigitColumns - calculation.answerDigitCount;

    for (int i = 0; i < answerFields.size(); ++i) {
        int col = 1 + answerStartColumn + i;
        double x = gridXOffset + col * cellWidth;
        double y = gridYOffset + answerRowIndex * cellHeight;
        answerFields[i]->setGeometry(QRect(static_cast<int>(x), static_cast<int>(y),
                                           static_cast<int>(cellWidth), static_cast<int>(cellHeight)));
        answerFields[i]->setStyleSheet(QString("font-size: %1px; font-family: \"%2\"; background: transparent; border: none;")
                                           .arg(static_cast<int>(cellHeight * 0.65)).arg(numberFontFamily));
    }
}

// Ein einzelnes, zusammenhaengendes Eingabefeld ueber "answerDigitCount" Kaestchen-
// breiten - fuer Antworten, die nicht in Ein-Zeichen-Kaestchen passen (Komma, Minus).
void WrittenGridWidget::placeFreeformField(int startCol, int rowIndex, double cellWidth, double cellHeight)
{
    double x = gridXOffset + startCol * cellWidth;
    double y = gridYOffset + rowIndex * cellHeight;
    double w = cellWidth * calculation.answerDigitCount;
    answerFields[0]->setGeometry(QRect(static_cast<int>(x), static_cast<int>(y),
                                       static_cast<int>(w), static_cast<int>(cellHeight)));
    answerFields[0]->setStyleSheet(QString("font-size: %1px; font-family: \"%2\"; background: transparent; border: none;")
                                       .arg(static_cast<int>(cellHeight * 0.55)).arg(numberFontFamily));
}

// Berechnet Skalierung + Verschiebung so, dass die Bounding-Box aller Punkte/Labels aus
// geometryModel SEITENVERHAELTNISTREU (kein Verzerren) in die verfuegbare Flaeche
// (Widget minus Rand) passt - uebernommen aus der urspruenglich eigenstaendigen
// GeometryModelWidget::recomputeLayout() (siehe showModel()-Kommentar in written_grid_widget.h).
void WrittenGridWidget::recomputeGeometryLayout()
{
    double minX = geometryModel.points.first().x();
    double maxX = minX;
    double minY = geometryModel.points.first().y();
    double maxY = minY;

    auto expand = [&](const QPointF &p) {
        minX = std::min(minX, p.x());
        maxX = std::max(maxX, p.x());
        minY = std::min(minY, p.y());
        maxY = std::max(maxY, p.y());
    };
    for (const QPointF &p : geometryModel.points) expand(p);
    // Auch Label-Positionen einbeziehen - bei rectangle_generator.cpp liegen die
    // Antwort-Labels (Flaeche/Umfang) z.B. bewusst UNTERHALB des Rechtecks, also
    // ausserhalb der reinen Punkte-Huelle.
    for (const GeometryLabel &label : geometryModel.labels) expand(label.position);

    // std::max(..., 0.001) sichert gegen Division durch 0 ab, falls ein (entartetes)
    // Modell nur einen einzigen Punkt haette.
    double modelWidth = std::max(maxX - minX, 0.001);
    double modelHeight = std::max(maxY - minY, 0.001);

    constexpr double Padding = 40.0;   // Pixel Rand rundherum, Linien/Labels sollen nicht am Widget-Rand kleben
    double availableWidth = std::max(static_cast<double>(width()) - 2 * Padding, 1.0);
    double availableHeight = std::max(static_cast<double>(height()) - 2 * Padding, 1.0);

    // Der KLEINERE der beiden moeglichen Skalierungsfaktoren gewinnt - sonst wuerde
    // z.B. ein schmales, hohes Modell in einem breiten Widget in die Breite verzerrt.
    geometryScale = std::min(availableWidth / modelWidth, availableHeight / modelHeight);

    // Zentrieren: ueberschuessiger Platz (auf der Achse, die NICHT den Skalierungsfaktor
    // bestimmt hat) wird je zur Haelfte links/rechts bzw. oben/unten verteilt.
    double scaledWidth = modelWidth * geometryScale;
    double scaledHeight = modelHeight * geometryScale;
    geometryOffsetX = Padding + (availableWidth - scaledWidth) / 2.0;
    geometryOffsetY = Padding + (availableHeight - scaledHeight) / 2.0;
    geometryMinX = minX;
    geometryMinY = minY;

    qDebug() << "[WrittenGrid] Geometrie-Layout neu berechnet - Skalierungsfaktor:" << geometryScale
             << "| Modellgroesse:" << modelWidth << "x" << modelHeight
             << "| verfuegbare Flaeche:" << availableWidth << "x" << availableHeight;

    layoutGeometryAnswerFields();
}

QPointF WrittenGridWidget::mapModelToWidget(const QPointF &modelPoint) const
{
    // Ursprung des Modells (geometryMinX/geometryMinY) auf (geometryOffsetX,
    // geometryOffsetY) verschieben, dann mit dem Faktor aus recomputeGeometryLayout() skalieren.
    return QPointF(geometryOffsetX + (modelPoint.x() - geometryMinX) * geometryScale,
                   geometryOffsetY + (modelPoint.y() - geometryMinY) * geometryScale);
}

void WrittenGridWidget::layoutGeometryAnswerFields()
{
    if (answerFields.isEmpty()) return;

    constexpr double FieldWidth = 70.0;
    constexpr double FieldHeight = 30.0;

    int fieldIndex = 0;
    for (const GeometryLabel &label : geometryModel.labels) {
        if (label.answerIndex < 0) continue;   // nur die GESUCHTEN Stellen bekommen ein Feld

        QPointF center = mapModelToWidget(label.position);
        QLineEdit *field = answerFields[fieldIndex];
        field->setGeometry(static_cast<int>(center.x() - FieldWidth / 2.0),
                            static_cast<int>(center.y() - FieldHeight / 2.0),
                            static_cast<int>(FieldWidth), static_cast<int>(FieldHeight));
        field->setStyleSheet(QString("font-family: \"%1\"; background: transparent; border: 1px solid %2;")
                                  .arg(numberFontFamily, inkColor.name()));
        fieldIndex++;
    }
}

// Zeichnet Kanten als Linien sowie feste Labels sowie die kurzen Beschriftungen VOR den
// (von layoutGeometryAnswerFields() bereits positionierten) Antwortfeldern - siehe
// geometry_model.h fuer die Bedeutung von GeometryLabel::text bei answerIndex >= 0.
void WrittenGridWidget::paintGeometryModel(QPainter &painter)
{
    painter.setPen(QPen(inkColor, 2));
    for (const GeometryEdge &edge : geometryModel.edges) {
        painter.drawLine(mapModelToWidget(geometryModel.points[edge.fromPointIndex]),
                          mapModelToWidget(geometryModel.points[edge.toPointIndex]));
    }

    QFont font = numberFontFamily.isEmpty() ? this->font() : QFont(numberFontFamily);
    font.setPointSizeF(13);
    painter.setFont(font);
    painter.setPen(QPen(inkColor, 1.5));

    constexpr double FieldWidth = 70.0;
    constexpr double CaptionWidth = 90.0;
    constexpr double CaptionGap = 6.0;   // kleiner Abstand zwischen Beschriftung und Eingabefeld

    int fixedLabelCount = 0;
    int captionCount = 0;
    for (const GeometryLabel &label : geometryModel.labels) {
        QPointF center = mapModelToWidget(label.position);

        if (label.answerIndex < 0) {
            // Fester Wert: zentrierter Text an der Label-Position (z.B. Kantenmitte) -
            // gleiche Grundidee wie die Kaestchen-Texte weiter unten in paintEvent().
            QRectF labelRect(center.x() - 40, center.y() - 15, 80, 30);
            painter.drawText(labelRect, Qt::AlignCenter, label.text);
            fixedLabelCount++;
            continue;
        }

        // Gesuchter Wert: das Eingabefeld selbst ist bereits als QLineEdit-Kindwidget an
        // dieser Stelle positioniert (layoutGeometryAnswerFields()) - hier nur noch die
        // optionale kurze Beschriftung ("Fläche =" o.ae.) LINKS davor zeichnen, falls
        // label.text nicht leer ist.
        if (!label.text.isEmpty()) {
            QRectF captionRect(center.x() - FieldWidth / 2.0 - CaptionGap - CaptionWidth,
                                center.y() - 15, CaptionWidth, 30);
            painter.drawText(captionRect, Qt::AlignVCenter | Qt::AlignRight, label.text);
            captionCount++;
        }
    }

    qDebug() << "[WrittenGrid] Geometrie gezeichnet:" << geometryModel.edges.size() << "Kanten,"
             << fixedLabelCount << "feste Labels," << captionCount << "Beschriftungen,"
             << answerFields.size() << "Antwortfelder";
}

void WrittenGridWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    recomputeLayout();
}

void WrittenGridWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- Hintergrund-Karo ueber die KOMPLETTE Flaeche ---
    // Aus der Ink-Farbe abgeleitet (statt fest QColor(150,150,150,90)) - so passt der
    // Kontrast zur Linie automatisch zum aktuellen Theme statt fuer beide nur "okay
    // genug" zu sein: helle Linien auf dunklem, dunkle Linien auf hellem Hintergrund.
    QColor gridLineColor = inkColor;
    gridLineColor.setAlpha(70);
    QPen gridPen(gridLineColor, 1);
    painter.setPen(gridPen);
    for (double x = 0; x <= width(); x += squareSize) {
        painter.drawLine(QPointF(x, 0), QPointF(x, height()));
    }
    for (double y = 0; y <= height(); y += squareSize) {
        painter.drawLine(QPointF(0, y), QPointF(width(), y));
    }

    // --- Aufgabenblatt: mehrere Aufgaben ueber die Seite verteilt ---
    if (!worksheetTasks.isEmpty()) {
        QFont font = numberFontFamily.isEmpty() ? this->font() : QFont(numberFontFamily);
        font.setPointSizeF(squareSize * 0.9);
        painter.setFont(font);
        painter.setPen(QPen(inkColor, 1.2));

        int columns = 3;
        double cellW = squareSize * 8;
        double cellH = squareSize * 4;

        for (int i = 0; i < worksheetTasks.size(); ++i) {
            int row = i / columns;
            int col = i % columns;
            QRectF cellR(col * cellW, row * cellH, cellW, cellH);

            // "expression" deckt mittlerweile ALLE Aufgabenarten ab (auch Ketten,
            // Potenz/Wurzel/Log) - nur Finanz-/Einheiten-Textaufgaben haben keine und
            // fallen auf den normalen promptText zurueck (siehe task.h-Kommentar).
            const Task &task = worksheetTasks[i];
            QString line = task.writtenCalculation.expression.isEmpty() ? task.promptText : task.writtenCalculation.expression;
            if (line.endsWith('=')) line += " ____";   // Platz zum Ausfuellen mit der Hand

            painter.drawText(cellR, Qt::AlignCenter, line);
        }
        return;
    }

    // --- Geometrie-Modell (siehe showModel()) ---
    if (!geometryModel.points.isEmpty()) {
        paintGeometryModel(painter);
        return;
    }

    // --- Einzelne Aufgabe (Stacked ODER SingleLine) ---
    if (totalDigitColumns == 0) return;

    double cellWidth = squareSize;
    double cellHeight = squareSize * 2.0;

    QFont font = numberFontFamily.isEmpty() ? this->font() : QFont(numberFontFamily);
    font.setPointSizeF(cellHeight * 0.5);
    painter.setFont(font);
    painter.setPen(QPen(inkColor, 1.5));

    // Zeile, in der die Rueckmeldung (Punkt 4) spaeter unterhalb der Aufgabe
    // landet - je nach Modus reicht die Aufgabe unterschiedlich weit nach unten.
    double solutionRow = 1.0;

    if (calculation.mode == WrittenCalculation::DisplayMode::SingleLine) {
        // Generischer Ausdruck (deckt einfache Aufgaben genauso ab wie verkettete
        // Ketten oder Potenz/Wurzel/Log, die kein festes Operanden-Schema haben) -
        // jedes Zeichen bekommt ein eigenes Kaestchen, Leerzeichen bleiben als
        // Luecke sichtbar (sorgt z.B. bei "20% von 60 =" fuer Lesbarkeit).
        int col = 0;
        for (QChar ch : calculation.expression) {
            if (ch != ' ') {
                QRectF cellR(gridXOffset + col * cellWidth, gridYOffset, cellWidth, cellHeight);
                painter.drawText(cellR, Qt::AlignCenter, QString(ch));
            }
            col++;
        }
    } else {
        for (int row = 0; row < calculation.operands.size(); ++row) {
            QString padded = calculation.operands[row].rightJustified(totalDigitColumns, ' ');
            bool isLastOperand = (row == calculation.operands.size() - 1);

            if (isLastOperand) {
                QRectF opRect(gridXOffset, gridYOffset + row * cellHeight, cellWidth, cellHeight);
                painter.drawText(opRect, Qt::AlignCenter, calculation.operatorSymbol);
            }

            for (int col = 0; col < totalDigitColumns; ++col) {
                QChar ch = padded[col];
                if (ch == ' ') continue;
                QRectF cellR(gridXOffset + (col + 1) * cellWidth, gridYOffset + row * cellHeight, cellWidth, cellHeight);
                painter.drawText(cellR, Qt::AlignCenter, QString(ch));
            }
        }

        int totalColumns = totalDigitColumns + 1;
        double lineY = gridYOffset + calculation.operands.size() * cellHeight;
        QPen linePen(inkColor, 2);
        painter.setPen(linePen);
        painter.drawLine(QPointF(gridXOffset, lineY), QPointF(gridXOffset + totalColumns * cellWidth, lineY));

        solutionRow = calculation.operands.size() + 1.0;
    }

    // --- Rueckmeldung (Punkt 4): "Richtig!" bzw. "Falsch - richtig waere ..." direkt
    // unterhalb der Aufgabe im Raster statt im (bei Raster-Aufgaben ausgeblendeten)
    // feedbackLabel - dadurch entsteht kein Ueberlagerungs-/Klick-Problem wie vorher.
    if (!solutionText.isEmpty()) {
        QRectF solutionRect(0, gridYOffset + solutionRow * cellHeight, width(), cellHeight);
        QFont solutionFont = font;
        solutionFont.setPointSizeF(cellHeight * 0.4);
        painter.setFont(solutionFont);
        painter.setPen(QPen(solutionCorrect ? inkColor : QColor(kWrongAnswerColor), 1.5));
        painter.drawText(solutionRect, Qt::AlignCenter, solutionText);
    }
}

QString WrittenGridWidget::currentAnswerText() const
{
    // Sobald EIN Kaestchen leer ist, gilt die ganze Antwort als ungueltig - vorher
    // wurde ein leeres Kaestchen stillschweigend durch "0" ersetzt, wodurch eine
    // komplett leer gelassene Aufgabe als "richtig" durchging, wenn das Ergebnis
    // zufaellig 0 war, und ein einzelnes ausgelassenes Kaestchen die Ziffern verschob.
    QString result;
    for (QLineEdit *field : answerFields) {
        if (field->text().isEmpty()) return QString();
        result += field->text();
    }
    return result;
}

// Liefert die Antwort(en) des aktuell aktiven Modus - im Geometrie-Modus EINEN String
// PRO GeometryLabel::answerIndex (nicht pro Erzeugungsreihenfolge der Felder, die
// zufaellig uebereinstimmen KANN, aber nicht muss), im Rechen-Modus wie bisher EINEN
// zusammengesetzten String fuer alle Ziffern-Kaestchen zusammen, hier nur in einen
// 1-elementigen Vektor verpackt.
QVector<QString> WrittenGridWidget::currentAnswerTexts() const
{
    if (!geometryModel.points.isEmpty()) {
        QVector<QString> texts;
        int fieldIndex = 0;

        for (const GeometryLabel &label : geometryModel.labels) {
            if (label.answerIndex < 0) continue;

            // texts bedarfsgerecht vergroessern statt vorab eine feste Groesse
            // anzunehmen - answerIndex muss nicht lueckenlos von 0 an in Label-
            // Reihenfolge auftauchen.
            if (texts.size() <= label.answerIndex) texts.resize(label.answerIndex + 1);

            QLineEdit *field = answerFields[fieldIndex];
            texts[label.answerIndex] = field->text();

            qDebug() << "[WrittenGrid] Antwortfeld" << fieldIndex << "(Erzeugungsreihenfolge)"
                     << "-> answerIndex" << label.answerIndex << "| Text:" << field->text();
            fieldIndex++;
        }
        return texts;
    }

    return { currentAnswerText() };
}

void WrittenGridWidget::setInputEnabled(bool enabled)
{
    for (QLineEdit *field : answerFields) field->setEnabled(enabled);
}

void WrittenGridWidget::showAnswerColor(bool correct)
{
    QString color = correct ? kCorrectAnswerColor : kWrongAnswerColor;
    for (QLineEdit *field : answerFields) {
        field->setStyleSheet(field->styleSheet() + QString("border: 2px solid %1;").arg(color));
    }
}

// Plural-Pendant zu showAnswerColor() - im Geometrie-Modus bekommt JEDES Antwortfeld
// seine EIGENE Farbe ueber correctness[label.answerIndex] (gleiche Index-Logik wie
// currentAnswerTexts() oben), im Rechen-Modus unveraendert EINE Farbe fuer alle Kaestchen.
void WrittenGridWidget::showAnswerColors(const QVector<bool> &correctness)
{
    if (!geometryModel.points.isEmpty()) {
        int fieldIndex = 0;

        for (const GeometryLabel &label : geometryModel.labels) {
            if (label.answerIndex < 0) continue;

            bool correct = (label.answerIndex < correctness.size()) && correctness[label.answerIndex];
            QString color = correct ? kCorrectAnswerColor : kWrongAnswerColor;
            QLineEdit *field = answerFields[fieldIndex];
            field->setStyleSheet(field->styleSheet() + QString("border: 2px solid %1;").arg(color));

            qDebug() << "[WrittenGrid] Farb-Feedback Antwortfeld" << fieldIndex << "-> answerIndex"
                     << label.answerIndex << "| korrekt:" << correct;
            fieldIndex++;
        }
        return;
    }

    showAnswerColor(!correctness.isEmpty() && correctness.first());
}

void WrittenGridWidget::showSolution(const QString &text, bool correct)
{
    solutionText = text;
    solutionCorrect = correct;
    update();
}

void WrittenGridWidget::insertSymbolAtFocus(const QString &symbol)
{
    // Uebernommen aus TaskView::insertSymbolAtFocus() (Punkt 4) - answerFields gab es
    // dort nur fuer die jetzt entfallene Nicht-Raster-Darstellung, das Sonderzeichen-
    // Menue muss deshalb hier ins Raster-eigene answerFields schreiben.
    QLineEdit *focused = qobject_cast<QLineEdit*>(QApplication::focusWidget());

    if (focused && answerFields.contains(focused)) {
        focused->insert(symbol);
    } else if (!answerFields.isEmpty()) {
        answerFields.first()->insert(symbol);   // Fallback, falls kein Feld fokussiert ist
    }
}

void WrittenGridWidget::focusFirstDigit()
{
    if (answerFields.isEmpty()) return;

    // Startfeld haengt wie die Schreibrichtung vom Darstellungsmodus ab: Stacked
    // beginnt bei der letzten (= Einer-)Stelle, SingleLine ganz normal beim ersten Feld.
    bool rightToLeft = (calculation.mode == WrittenCalculation::DisplayMode::Stacked);
    (rightToLeft ? answerFields.last() : answerFields.first())->setFocus();
}

bool WrittenGridWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent*>(event);
        auto *field = qobject_cast<QLineEdit*>(watched);

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            emit answerSubmitted();
            return true;
        }

        if (field && (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)) {
            int index = answerFields.indexOf(field);
            if (index != -1) {
                int targetIndex = (keyEvent->key() == Qt::Key_Left) ? index - 1 : index + 1;
                if (targetIndex >= 0 && targetIndex < answerFields.size()) {
                    answerFields[targetIndex]->setFocus();
                }
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}