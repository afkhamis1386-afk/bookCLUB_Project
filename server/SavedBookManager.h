#ifndef SAVEDBOOKMANAGER_H
#define SAVEDBOOKMANAGER_H
#include "../common/Response.h"
class SavedBookManager {
public:
    SavedBookManager();
    Response saveBook(int userId, int bookId);
    Response unsaveBook(int userId, int bookId);
    Response getSavedBooks(int userId);
};

#endif // SAVEDBOOKMANAGER_H
