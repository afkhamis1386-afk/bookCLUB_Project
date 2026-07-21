#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H
#include "../common/Response.h"
#include <QString>
#include <QDateTime>
class BookManager {
public:
    BookManager();
    Response addBook(int publisherUserId, const QString &bookName, const QString &description, double price, const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
    const QByteArray &coverImageData, const QString &coverImageExtension, const QByteArray &pdfData);
    Response getCoverImageData(int bookId);
    Response updateBook(int publisherUserId, int bookId, const QString &bookName, const QString &description, double price);
    Response applyDiscount(int publisherUserId, int bookId, double discountPercent, double discountAmount);
    Response applyTimedDiscount(int publisherUserId, int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate);
    Response deactivateBook(int publisherUserId, int bookId);
    Response reactivateBook(int publisherUserId, int bookId);
    Response getStorefrontBooks();
    Response getBooksByGenre(int genreId);
    Response getBooksByCategory(int categoryId);
    Response getNewestBooks(int limit);
    Response getBestSellers(int limit);
    Response getPopularBooks(int limit);
    Response getFreeBooks();
    Response searchBooks(const QString &query);
    Response getBookDetails(int bookId);
    Response getBookFileData(int userId, int bookId);
    Response getRecommendedBooks(int userId);
    Response getPurchasedBooks(int userId);
    Response saveReadingProgress(int userId, int bookId, int lastPage);
};

#endif // BOOKMANAGER_H
