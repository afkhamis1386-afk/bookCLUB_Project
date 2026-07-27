#ifndef SAVEDBOOKMANAGER_H
#define SAVEDBOOKMANAGER_H

#include "../common/Response.h"
#include <QVariantList>

class SavedBookManager {
public:
    SavedBookManager();
    Response saveBook(int userId, int bookId);
    Response unsaveBook(int userId, int bookId);
    Response getSavedBooks(int userId);
    Response addFavoriteBook(int userId, int bookId);
    Response removeFavoriteBook(int userId, int bookId);
    Response getFavoriteBooks(int userId);
    Response reorderFavoriteBooks(int userId, const QVariantList &bookIds);
};

#endif // SAVEDBOOKMANAGER_H
