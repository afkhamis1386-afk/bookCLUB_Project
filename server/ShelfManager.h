#ifndef SHELFMANAGER_H
#define SHELFMANAGER_H

#include "../common/Response.h"
#include <QString>
#include <QVariantList>

class ShelfManager {
public:
    ShelfManager();
    Response createShelf(int userId, const QString &shelfName);
    Response renameShelf(int userId, int shelfId, const QString &newName);
    Response deleteShelf(int userId, int shelfId);
    Response addBookToShelf(int userId, int shelfId, int bookId);
    Response removeBookFromShelf(int userId, int shelfId, int bookId);
    Response moveBookBetweenShelves(int userId, int sourceShelfId, int destShelfId, int bookId);
    Response reorderShelves(int userId, const QVariantList &shelfIds);
    Response reorderShelfBooks(int userId, int shelfId, const QVariantList &bookIds);
    Response getUserShelves(int userId);
};

#endif // SHELFMANAGER_H
