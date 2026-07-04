#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H
#include "../common/normaluser.h"
#include "../common/Enums.h"
#include <QString>
#include <QVector>
#include <QMap>
class UserRepository {
public:
    UserRepository();
    int insertNormalUser(const NormalUser &user);
    NormalUser* loadNormalUserById(int userId);
    bool findUserRoleAndId(const QString &encryptedUsername, int &outUserId, UserRole &outRole);
    bool updateBlockedStatus(int userId, bool isBlocked);
    bool updateDeletedStatus(int userId, bool isDeleted);
    bool updatePasswordHash(int userId, const QString &newPasswordHash);
    QVector<int> getAllUserIds();
    QVector<int> getAllNormalUserIds();
    QVector<int> getFavoriteGenreIds(int userId);
    bool setFavoriteGenreIds(int userId, const QVector<int> &genreIds);
    QVector<int> getPurchasedBookIds(int userId);
    QVector<int> getSavedBookIds(int userId);
    bool addSavedBook(int userId, int bookId);
    bool removeSavedBook(int userId, int bookId);
    QMap<int, int> getReadingProgress(int userId);
    bool setLastReadPage(int userId, int bookId, int pageNumber);
    bool findRoleById(int userId, UserRole &outRole);
};

#endif // USERREPOSITORY_H
