#ifndef BOOKCARDWIDGET_H
#define BOOKCARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class BookCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BookCardWidget(int bookId, QWidget *parent = nullptr);
    int getBookId() const;
    void setCoverImage(const QByteArray &imageData);
    void setBookInfo(const QString &bookName, double price);
    void setGenreName(const QString &genreName);

signals:
    void clicked(int bookId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int bookId;
    QLabel *coverLabel;
    QLabel *nameLabel;
    QLabel *priceLabel;
    QLabel *genreLabel;
};

#endif // BOOKCARDWIDGET_H