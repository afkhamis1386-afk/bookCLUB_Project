#ifndef SHELFREPOSITORY_H
#define SHELFREPOSITORY_H

#include "../common/Shelf.h"
#include <QString>
#include <QVector>

class ShelfRepository {
public:
    ShelfRepository();
    int insertShelf(const Shelf &shelf);
    Shelf* loadShelfById(int shelfId);
    QVector<int> getShelfIdsByUser(int userId);
    QVector<int> getBookIdsByShelf(int shelfId);
    bool updateShelfName(int shelfId, const QString &newName);
    bool deleteShelf(int shelfId);
    bool addBookToShelf(int shelfId, int bookId);
    bool shelfContainsBook(int shelfId, int bookId);
    bool removeBookFromShelf(int shelfId, int bookId);
    bool reorderShelves(int userId, const QVector<int> &shelfIds);
    bool reorderShelfBooks(int shelfId, const QVector<int> &bookIds);
    bool shelfNameExistsForUser(int userId, const QString &shelfName);
    bool shelfBelongsToUser(int shelfId, int userId);
};

#endif // SHELFREPOSITORY_H
