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
    bool updateActiveStatus(int userId, bool isActive);
    bool updatePasswordHash(int userId, const QString &newPasswordHash);
    bool isUsernameTakenByOther(const QString &encryptedUsername, int excludedUserId);
    bool updateNormalUserAccount(int userId, const QString &encryptedUsername,
                                 const QString &newPasswordHash, const QString &newSecurityAnswerHash);
    QVector<int> getAllUserIds();
    QVector<int> getAllNormalUserIds();
    QVector<int> getFavoriteGenreIds(int userId);
    QVector<int> getUserIdsByFavoriteGenre(int genreId);
    bool setFavoriteGenreIds(int userId, const QVector<int> &genreIds);
    QVector<int> getPurchasedBookIds(int userId);
    QMap<int, int> getReadingProgress(int userId);
    bool findRoleById(int userId, UserRole &outRole);
};

#endif // USERREPOSITORY_H