#include "written_grid_widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <algorithm>

WrittenGridWidget::WrittenGridWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(150, 150);
}

void WrittenGridWidget::showCalculation(const WrittenCalculation &calc)
{
    calculation = calc;

    for (QLineEdit *field : answerFields) {
        field->deleteLater();
    }
    answerFields.clear();

    int maxOperandLength = 0;
    for (const QString &op : calculation.operands) {
        maxOperandLength = std::max(maxOperandLength, static_cast<int>(op.length()));
    }
    totalDigitColumns = std::max(maxOperandLength, calculation.answerDigitCount);

    lineRowIndex = calculation.operands.size();
    totalDataRows = calculation.operands.size() + 2;   // + Trennlinie + Ergebniszeile

    for (int i = 0; i < calculation.answerDigitCount; ++i) {
        QLineEdit *field = new QLineEdit(this);
        field->setObjectName("writtenAnswerDigit");
        field->setMaxLength(1);
        field->setAlignment(Qt::AlignCenter);
        field->installEventFilter(this);
        answerFields.append(field);
        field->show();
    }

    for (int i = 0; i < answerFields.size(); ++i) {
        connect(answerFields[i], &QLineEdit::textChanged, this, [this, i](const QString &text) {
            if (text.length() == 1 && i + 1 < answerFields.size()) {
                answerFields[i + 1]->setFocus();
            }
        });
    }

    recomputeLayout();
    update();
}

void WrittenGridWidget::recomputeLayout()
{
    if (totalDigitColumns == 0) return;

    int totalColumns = totalDigitColumns + 1;   // +1 fuer die Operator-Spalte links

    // Ziffern-Verhaeltnis: Breite:Hoehe = 1:2 (wie echtes Rechenkaropapier: 5mm breit, 10mm hoch).
    // Wir suchen die groesste Zellenbreite, die SOWOHL in die verfuegbare Breite ALS AUCH
    // in die verfuegbare Hoehe passt - dadurch bleibt das Raster IMMER proportional korrekt,
    // egal wie gross das Fenster/der Bildschirm ist.
    double lineRowFraction = 0.35;   // Trennlinien-"Zeile" ist schmaler als eine normale Ziffernzeile
    double totalRowUnits = (totalDataRows - 1) + lineRowFraction;

    double cellWidthByWidth = static_cast<double>(width()) / totalColumns;
    double cellWidthByHeight = static_cast<double>(height()) / totalRowUnits / 2.0;

    cellWidth = std::min(cellWidthByWidth, cellWidthByHeight);
    cellHeight = cellWidth * 2.0;

    double gridWidth = cellWidth * totalColumns;
    double gridHeight = cellHeight * totalRowUnits;

    gridXOffset = (width() - gridWidth) / 2.0;
    gridYOffset = (height() - gridHeight) / 2.0;

    rowYOffsets.clear();
    double y = 0;
    for (int row = 0; row < totalDataRows; ++row) {
        rowYOffsets.append(y);
        y += (row == lineRowIndex) ? (cellHeight * lineRowFraction) : cellHeight;
    }

    layoutAnswerFields();
}

void WrittenGridWidget::layoutAnswerFields()
{
    if (answerFields.isEmpty()) return;

    int answerRow = totalDataRows - 1;
    int answerStartColumn = totalDigitColumns - calculation.answerDigitCount;

    for (int i = 0; i < answerFields.size(); ++i) {
        int col = 1 + answerStartColumn + i;
        double x = gridXOffset + col * cellWidth;
        double y = gridYOffset + rowYOffsets[answerRow];
        answerFields[i]->setGeometry(QRect(static_cast<int>(x), static_cast<int>(y),
                                           static_cast<int>(cellWidth), static_cast<int>(cellHeight)));
        answerFields[i]->setStyleSheet(QString("font-size: %1px;").arg(static_cast<int>(cellHeight * 0.5)));
    }
}

void WrittenGridWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    recomputeLayout();
}

void WrittenGridWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (totalDigitColumns == 0) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = this->font();
    font.setPointSizeF(cellHeight * 0.35);
    painter.setFont(font);
    painter.setPen(QPen(palette().text().color(), 1));

    int totalColumns = totalDigitColumns + 1;

    for (int row = 0; row < calculation.operands.size(); ++row) {
        QString padded = calculation.operands[row].rightJustified(totalDigitColumns, ' ');
        bool isLastOperand = (row == calculation.operands.size() - 1);

        if (isLastOperand) {
            QRectF opRect(gridXOffset, gridYOffset + rowYOffsets[row], cellWidth, cellHeight);
            painter.drawText(opRect, Qt::AlignCenter, calculation.operatorSymbol);
        }

        for (int col = 0; col < totalDigitColumns; ++col) {
            QChar ch = padded[col];
            QRectF cellR(gridXOffset + (col + 1) * cellWidth, gridYOffset + rowYOffsets[row], cellWidth, cellHeight);
            painter.drawRect(cellR);
            if (ch != ' ') {
                painter.drawText(cellR, Qt::AlignCenter, QString(ch));
            }
        }
    }

    double lineY = gridYOffset + rowYOffsets[lineRowIndex] + cellHeight * 0.15;
    painter.drawLine(QPointF(gridXOffset, lineY), QPointF(gridXOffset + totalColumns * cellWidth, lineY));

    // Rahmen fuer die Antwort-Zeile (die Kaestchen selbst sind echte QLineEdits, hier nur der Umriss)
    int answerRow = totalDataRows - 1;
    int answerStartColumn = totalDigitColumns - calculation.answerDigitCount;
    for (int i = 0; i < calculation.answerDigitCount; ++i) {
        QRectF cellR(gridXOffset + (1 + answerStartColumn + i) * cellWidth,
                     gridYOffset + rowYOffsets[answerRow], cellWidth, cellHeight);
        painter.drawRect(cellR);
    }
}

QString WrittenGridWidget::currentAnswerText() const
{
    QString result;
    for (QLineEdit *field : answerFields) {
        result += field->text().isEmpty() ? "0" : field->text();
    }
    return result;
}

void WrittenGridWidget::setInputEnabled(bool enabled)
{
    for (QLineEdit *field : answerFields) field->setEnabled(enabled);
}

void WrittenGridWidget::showAnswerColor(bool correct)
{
    QString color = correct ? "#2ecc71" : "#e74c3c";
    for (QLineEdit *field : answerFields) {
        field->setStyleSheet(field->styleSheet() + QString("border: 2px solid %1;").arg(color));
    }
}

void WrittenGridWidget::focusFirstDigit()
{
    if (!answerFields.isEmpty()) answerFields.first()->setFocus();
}

bool WrittenGridWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            emit answerSubmitted();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}