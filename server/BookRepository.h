#ifndef BOOKREPOSITORY_H
#define BOOKREPOSITORY_H
#include "../common/Book.h"
#include <QString>
#include <QVector>
class BookRepository {
public:
    BookRepository();
    int insertBook(const Book &book);
    Book* loadBookById(int bookId);
    bool updateBook(const Book &book);
    bool updateDiscount(int bookId, double discountPercent, double discountAmount);
    bool setActiveStatus(int bookId, bool isActive);
    bool setDeletedStatus(int bookId, bool isDeleted);
    QVector<int> searchByName(const QString &nameQuery);
    QVector<int> searchByAuthorName(const QString &authorNameQuery);
    QVector<int> searchByPublisherName(const QString &publisherNameQuery);
    QVector<int> getAllActiveBookIds();
    QVector<int> getBooksByGenre(int genreId);
    QVector<int> getBooksByCategory(int categoryId);
    QVector<int> getNewestBooks(int limit);
    QVector<int> getFreeBooks();
    QVector<int> getBooksByPublisher(int publisherUserId);
    int getTotalBooksCountByPublisher(int publisherUserId);
};
#endif // BOOKREPOSITORY_H
