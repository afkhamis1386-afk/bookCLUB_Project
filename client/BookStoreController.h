#ifndef BOOKSTORECONTROLLER_H
#define BOOKSTORECONTROLLER_H

#include <QObject>
#include <QVariantList>
#include "NetworkManager.h"

class BookStoreController : public QObject {
    Q_OBJECT
public:
    explicit BookStoreController(NetworkManager *networkManager, QObject *parent = nullptr);
    void loadAllBooks();
    void search(const QString &query);
    void loadBookDetails(int bookId);
    void loadBooksByGenre(int genreId);
    void loadBooksByCategory(int categoryId);
    void loadNewestBooks(int limit = 10);
    void loadFreeBooks();
    void loadRecommendedBooks();

signals:
    void booksLoaded(const QVariantList &bookIds);
    void booksLoadFailed(const QString &message);
    void searchResultsReceived(const QVariantList &bookIds);
    void searchFailed(const QString &message);
    void bookDetailsReceived(const QVariantMap &bookData);
    void bookDetailsFailed(const QString &message);
    void validationError(const QString &message);
    void recommendedBooksLoaded(const QVariantList &bookIds);
    void recommendedBooksLoadFailed(const QString &message);
private slots:
    void onResponseReceived(RequestType type, const Response &response);
private:
    NetworkManager *networkManager;
    RequestType lastBookListRequest;
};

#endif // BOOKSTORECONTROLLER_H



