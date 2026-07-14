#include "bookcardwidget.h"
#include <QMouseEvent>
#include <QPixmap>

BookCardWidget::BookCardWidget(int bookId, QWidget *parent)
    : QWidget(parent), bookId(bookId)
{
    setFixedSize(160, 270);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "BookCardWidget { background-color: rgba(255,255,255,220); border-radius: 10px; }"
        "BookCardWidget:hover { background-color: rgba(255,255,255,255); }"
        );
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(144, 180);
    coverLabel->setScaledContents(true);
    coverLabel->setStyleSheet("background-color: #dcdcdc; border-radius: 6px;");
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setText("...");
    nameLabel = new QLabel(this);
    nameLabel->setWordWrap(true);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-weight: bold; font-size: 10pt; color: #2c3e50;");
    nameLabel->setMaximumHeight(40);
    priceLabel = new QLabel(this);
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setStyleSheet("color: #34495e; font-size: 9pt;");
    layout->addWidget(coverLabel);
    layout->addWidget(nameLabel);
    layout->addWidget(priceLabel);
}
int BookCardWidget::getBookId() const
{
    return bookId;
}
void BookCardWidget::setCoverImage(const QByteArray &imageData)
{
    QPixmap pixmap;
    if (pixmap.loadFromData(imageData)) {
        coverLabel->setPixmap(pixmap);
        coverLabel->setText("");
    }
}
void BookCardWidget::setBookInfo(const QString &bookName, double price)
{
    nameLabel->setText(bookName);
    if (price <= 0)
        priceLabel->setText("رایگان");
    else
        priceLabel->setText(QString::number(price, 'f', 0) + " تومان");
}
void BookCardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked(bookId);
    QWidget::mousePressEvent(event);
}