#include "written_grid_widget.h"
#include "feedback_colors.h"
#include <QPainter>
#include <QKeyEvent>
#include <QApplication>
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

    worksheetTasks = tasks;
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