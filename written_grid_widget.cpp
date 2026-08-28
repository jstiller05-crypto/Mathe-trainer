#include "written_grid_widget.h"
#include <QPainter>
#include <QKeyEvent>
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

void WrittenGridWidget::showCalculation(const WrittenCalculation &calc)
{
    calculation = calc;

    for (QLineEdit *field : answerFields) field->deleteLater();
    answerFields.clear();

    int maxOperandLength = 0;
    for (const QString &op : calculation.operands) {
        maxOperandLength = std::max(maxOperandLength, static_cast<int>(op.length()));
    }
    totalDigitColumns = std::max(maxOperandLength, calculation.answerDigitCount);

    for (int i = 0; i < calculation.answerDigitCount; ++i) {
        QLineEdit *field = new QLineEdit(this);
        field->setObjectName("writtenAnswerDigit");
        field->setMaxLength(1);
        field->setAlignment(Qt::AlignCenter);
        field->setFrame(false);
        field->installEventFilter(this);
        answerFields.append(field);
        field->show();
    }

    // WICHTIG: answerFields[0] ist die LINKESTE Ziffer (hoechster Stellenwert),
    // answerFields.last() ist die RECHTESTE (Einer-Stelle). Schriftliches Rechnen
    // beginnt bei den Einern -> nach Eingabe geht der Fokus nach LINKS (i-1), nicht nach rechts.
    for (int i = 0; i < answerFields.size(); ++i) {
        connect(answerFields[i], &QLineEdit::textChanged, this, [this, i](const QString &text) {
            if (text.length() == 1 && i - 1 >= 0) {
                answerFields[i - 1]->setFocus();
            }
        });
    }

    recomputeLayout();
    update();
}

void WrittenGridWidget::recomputeLayout()
{
    if (totalDigitColumns == 0) return;

    squareSize = static_cast<double>(width()) / PAGE_COLUMNS;
    pageRows = static_cast<int>(height() / squareSize);

    int taskColumnsInSquares = totalDigitColumns + 1;
    int taskRowsInSquares = (calculation.operands.size() + 1) * 2;

    int startColSquares = (PAGE_COLUMNS - taskColumnsInSquares) / 2;
    int startRowSquares = (pageRows - taskRowsInSquares) / 2;

    gridXOffset = startColSquares * squareSize;
    gridYOffset = std::max(0, startRowSquares) * squareSize;

    layoutAnswerFields();
}

void WrittenGridWidget::layoutAnswerFields()
{
    if (answerFields.isEmpty()) return;

    double cellWidth = squareSize;
    double cellHeight = squareSize * 2.0;

    int answerRowIndex = calculation.operands.size();
    int answerStartColumn = totalDigitColumns - calculation.answerDigitCount;

    for (int i = 0; i < answerFields.size(); ++i) {
        int col = 1 + answerStartColumn + i;
        double x = gridXOffset + col * cellWidth;
        double y = gridYOffset + answerRowIndex * cellHeight;

        answerFields[i]->setGeometry(QRect(static_cast<int>(x), static_cast<int>(y),
                                           static_cast<int>(cellWidth), static_cast<int>(cellHeight)));
        answerFields[i]->setStyleSheet(QString("font-size: %1px; font-family: \"%2\"; background: transparent; border: none;")
                                           .arg(static_cast<int>(cellHeight * 0.65))
                                           .arg(numberFontFamily));
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
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen gridPen(QColor(150, 150, 150, 90), 1);
    painter.setPen(gridPen);

    for (double x = 0; x <= width(); x += squareSize) {
        painter.drawLine(QPointF(x, 0), QPointF(x, height()));
    }
    for (double y = 0; y <= height(); y += squareSize) {
        painter.drawLine(QPointF(0, y), QPointF(width(), y));
    }

    if (totalDigitColumns == 0) return;

    double cellWidth = squareSize;
    double cellHeight = squareSize * 2.0;

    QFont font = numberFontFamily.isEmpty() ? this->font() : QFont(numberFontFamily);
    font.setPointSizeF(cellHeight * 0.5);
    painter.setFont(font);
    painter.setPen(QPen(Qt::white, 1.5));

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
    QPen linePen(Qt::white, 2);
    painter.setPen(linePen);
    painter.drawLine(QPointF(gridXOffset, lineY), QPointF(gridXOffset + totalColumns * cellWidth, lineY));
}

QString WrittenGridWidget::currentAnswerText() const
{
    QString result;
    for (QLineEdit *field : answerFields) result += field->text().isEmpty() ? "0" : field->text();
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
    // "Erste" Eingabe beim schriftlichen Rechnen ist die RECHTESTE Ziffer (Einer) -> .last()
    if (!answerFields.isEmpty()) answerFields.last()->setFocus();
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

        // Pfeiltasten: NEU - erlaubt manuelles Springen zwischen den Kaestchen
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