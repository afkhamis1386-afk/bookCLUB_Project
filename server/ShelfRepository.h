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
    bool updateShelfName(int shelfId, const QString &newName);
    bool deleteShelf(int shelfId);
    bool addBookToShelf(int shelfId, int bookId);
    bool removeBookFromShelf(int shelfId, int bookId);
    bool shelfNameExistsForUser(int userId, const QString &shelfName);
    bool shelfBelongsToUser(int shelfId, int userId);
};

#endif // SHELFREPOSITORY_H
