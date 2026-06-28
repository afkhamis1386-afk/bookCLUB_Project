#ifndef BOOK_H
#define BOOK_H
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QDataStream>
class Book{
private:
    int bookId;
    QString title;
    int authorId;
    int genreId;
    int publisherId;
    double basePrice;
    double discountPrc;
    QDateTime discountStartTime;
    QDateTime discountEndTime;
    QString description;
    QString coverImagePath;
    QString pdfFilePath;
    QVector<int> reviews;
    int salesCount;
public:
    Book();
    Book(int id, const QString& ti, int auId, int geId, int pubId, double pr, const QString& des, const QString& coverPath = "", const QString& pdfPath = "");
    ~Book();
    int get_BookId() const { return bookId; }
    const QString& get_Title() const { return title; }
    int get_AuthorId() const { return authorId; }
    int get_GenreId() const { return genreId; }
    int get_PublisherId() const { return publisherId; }
    double get_BasePrice() const { return basePrice; }
    double get_DiscountPrc() const { return discountPrc; }
    QDateTime get_DiscountStartTime() const { return discountStartTime; }
    QDateTime get_DiscountEndTime() const { return discountEndTime; }
    const QString& get_Description() const { return description; }
    const QString& get_CoverImagePath() const { return coverImagePath; }
    const QString& get_PdfFilePath() const { return pdfFilePath; }
    const QVector<int>& get_Reviews() const { return reviews; }
    int get_SalesCount() const { return salesCount; }
    void set_Title(const QString& title);
    void set_AuthorId(int auId);
    void set_GenreId(int geId);
    void set_PublisherId(int pubId);
    void set_DiscountPrc(double discountPrc);
    void set_DiscountStartTime(const QDateTime& startTime);
    void set_DiscountEndTime(const QDateTime& endTime);
    void set_Description(const QString& description);
    void set_CoverImagePath(const QString& coverPath);
    void set_PdfFilePath(const QString& pdfPath);
    void set_SalesCount(int salesCount);
    void addReview(int reviewId);
    double get_FinalPrice() const;
    void updateDetails(const QString& title, double price);
    friend QDataStream& operator<<(QDataStream& out, const Book& book);
    friend QDataStream& operator>>(QDataStream& in, Book& book);
};
#endif // BOOK_H
