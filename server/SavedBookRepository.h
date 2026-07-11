#ifndef SAVEDBOOKREPOSITORY_H
#define SAVEDBOOKREPOSITORY_H

#include <QVector>
class SavedBookRepository {
public:
    SavedBookRepository();
    bool isBookSaved(int userId, int bookId);
    bool saveBook(int userId, int bookId);
    bool unsaveBook(int userId, int bookId);
    QVector<int> getSavedBookIds(int userId);
    QVector<int> getUserIdsWhoSavedBook(int bookId);
};

#endif // SAVEDBOOKREPOSITORY_H
