#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H
#include "../common/Response.h"
#include <QString>
class BookManager {
public:
    BookManager();
    Response addBook(int publisherUserId, const QString &bookName, const QString &description, double price, const QString &genreTitle, const QString &categoryTitle, const QString &authorName, const QString &coverImagePath,
     const QString &pdfFilePath);
    Response updateBook(int publisherUserId, int bookId, const QString &bookName, const QString &description, double price);
    Response applyDiscount(int publisherUserId, int bookId, double discountPercent, double discountAmount);
    Response deactivateBook(int publisherUserId, int bookId);
    Response reactivateBook(int publisherUserId, int bookId);
    Response deleteBookByAdmin(int bookId);
    Response getStorefrontBooks();
    Response getBooksByGenre(int genreId);
    Response getBooksByCategory(int categoryId);
    Response getNewestBooks(int limit);
    Response getFreeBooks();
    Response searchBooks(const QString &query);
    Response getBookDetails(int bookId);
    Response getPublisherDashboard(int publisherUserId);
    Response getRecommendedBooks(int userId);
};

#endif // BOOKMANAGER_H
