#ifndef LIBRARYCONTROLLER_H
#define LIBRARYCONTROLLER_H
#include <QObject>
#include <QVariantList>
#include "NetworkManager.h"
class LibraryController : public QObject {
    Q_OBJECT

public:
    explicit LibraryController(NetworkManager *networkManager, QObject *parent = nullptr);
    void refreshShelves();
    void createShelf(const QString &shelfName);
    void renameShelf(int shelfId, const QString &newName);
    void deleteShelf(int shelfId);
    void addBookToShelf(int shelfId, int bookId);
    void removeBookFromShelf(int shelfId, int bookId);
    void refreshPurchasedBooks();

signals:
    void shelvesLoaded(const QVariantList &shelves);
    void shelvesLoadFailed(const QString &message);
    void shelfCreated(int shelfId, const QString &message);
    void shelfCreateFailed(const QString &message);
    void shelfRenamed(const QString &message);
    void shelfRenameFailed(const QString &message);
    void shelfDeleted(const QString &message);
    void shelfDeleteFailed(const QString &message);
    void bookAddedToShelf(const QString &message);
    void bookAddToShelfFailed(const QString &message);
    void bookRemovedFromShelf(const QString &message);
    void bookRemoveFromShelfFailed(const QString &message);
    void validationError(const QString &message);
    void purchasedBooksLoaded(const QVariantList &bookIds);
    void purchasedBooksLoadFailed(const QString &message);

private slots:
    void onResponseReceived(RequestType type, const Response &response);

private:
    NetworkManager *networkManager;
};

#endif // LIBRARYCONTROLLER_H
