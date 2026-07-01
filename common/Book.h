#ifndef BOOK_H
#define BOOK_H
#include <QString>
#include <QDateTime>
#include <QDataStream>
class Book {
private:
    int bookId;
    QString bookName;
    QString bookDescription;
    double bookPrice;
    double discountPercent;
    double discountAmount;
    QString coverImagePath;
    QString pdfFilePath;
    QDateTime registeredIn;
    bool isActive;
    bool isDeleted;
    int genreId;
    int categoryId;
    int authorId;
    int publisherUserId;
public:
    Book();
    Book(const QString &bN, const QString &bD, double bP, int gI, int cI, int auI, int pUI, const QString &cIP = QString(), const QString &pFP = QString());
    Book(int bI, const QString &bN, const QString &bD, double bP, double dP, double dA, const QString &cIP, const QString &pFP, const QDateTime &rIn, bool iA, bool iD, int gI, int cI, int auI, int pUI);
    ~Book();
    int getBookId() const;
    QString getBookName() const;
    QString getBookDescription() const;
    double getBookPrice() const;
    double getDiscountPercent() const;
    double getDiscountAmount() const;
    QString getCoverImagePath() const;
    QString getPdfFilePath() const;
    QDateTime getRegisteredIn() const;
    bool getIsActive() const;
    bool getIsDeleted() const;
    int getGenreId() const;
    int getCategoryId() const;
    int getAuthorId() const;
    int getPublisherUserId() const;
    void setBookId(int id);
    bool setBookName(const QString &name);
    bool setBookDescription(const QString &description);
    bool setBookPrice(double price);
    bool setDiscountPercent(double percent);
    bool setDiscountAmount(double amount);
    bool setCoverImagePath(const QString &path);
    bool setPdfFilePath(const QString &path);
    void setGenreId(int genreId);
    void setCategoryId(int categoryId);
    void setAuthorId(int authorId);
    void deactivate();
    void reactivate();
    void markAsDeleted();
    bool isAvailableForPurchase() const;
    double getFinalPrice() const;
    friend QDataStream &operator<<(QDataStream &out, const Book &book);
    friend QDataStream &operator>>(QDataStream &in, Book &book);
};
#endif // BOOK_H
