#ifndef BOOK_H
#define BOOK_H
#include <QString>
#include <QVector>
#include <QDateTime>
class Book{
private:
    int bookId;
    QString title;
    QString author;
    QString genre;
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
    Book(int id, const QString& ti, const QString& au, const QString& ge, double pr, const QString& des, const QString& coverPath = "", const QString& pdfPath = "");
    ~Book();
    int get_BookId() const { return bookId; }
    QString get_Title() const { return title; }
    QString get_Author() const { return author; }
    QString get_Genre() const { return genre; }
    double get_BasePrice() const { return basePrice; }
    double get_DiscountPrc() const { return discountPrc; }
    QDateTime get_DiscountStartTime() const { return discountStartTime; }
    QDateTime get_DiscountEndTime() const { return discountEndTime; }
    QString get_Description() const { return description; }
    QString get_CoverImagePath() const { return coverImagePath; }
    QString get_PdfFilePath() const { return pdfFilePath; }
    QVector<int> get_Reviews() const { return reviews; }
    int get_SalesCount() const { return salesCount; }
    void set_Genre(const QString& genre);
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
};
#endif // BOOK_H
