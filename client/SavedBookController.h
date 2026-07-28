#ifndef SAVEDBOOKCONTROLLER_H
#define SAVEDBOOKCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include "NetworkManager.h"

class SavedBookController : public QObject {
    Q_OBJECT
public:
    explicit SavedBookController(NetworkManager *networkManager, QObject *parent = nullptr);
    void saveBook(int bookId);
    void unsaveBook(int bookId);
    void refreshSavedBooks();
    void addFavoriteBook(int bookId);
    void removeFavoriteBook(int bookId);
    void refreshFavoriteBooks();
    void reorderFavoriteBooks(const QVariantList &bookIds);
signals:
    void bookSaved(const QString &message);
    void bookSaveFailed(const QString &message);
    void bookUnsaved(const QString &message);
    void bookUnsaveFailed(const QString &message);
    void savedBooksLoaded(const QVariantList &books);
    void savedBooksLoadFailed(const QString &message);
    void favoriteBookAdded(const QString &message);
    void favoriteBookAddFailed(const QString &message);
    void favoriteBookRemoved(const QString &message);
    void favoriteBookRemoveFailed(const QString &message);
    void favoriteBooksLoaded(const QVariantList &books);
    void favoriteBooksLoadFailed(const QString &message);
    void favoriteBooksReordered(const QString &message);
    void favoriteBooksReorderFailed(const QString &message);
    void validationError(const QString &message);
private slots:
    void onResponseReceived(RequestType type, const Response &response);

private:
    NetworkManager *networkManager;
};

#endif // SAVEDBOOKCONTROLLER_H
