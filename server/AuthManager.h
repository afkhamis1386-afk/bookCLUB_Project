#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H
#include "../common/Response.h"
#include "../common/Enums.h"
#include <QString>
#include <QVariantMap>
#include <QVector>
class UserRepository;
class PublisherRepository;
class AdminRepository;
class User;
class NormalUser;
class Publisher;
class Admin;
class AuthManager {
public:
    AuthManager();
    Response registerNormalUser(const QString &username, const QString &plainPassword, const QString &plainAnswer);
    Response registerPublisher(const QString &username, const QString &plainPassword, const QString &plainAnswer, const QString &firstName,
                               const QString &lastName, const QString &email,const QString &publicationName,
                               const QString &licenseNumber, const QString &shortDescription);
    Response login(const QString &username, const QString &plainPassword);
    Response changePassword(int userId, UserRole role, const QString &oldPassword, const QString &newPassword);
    Response recoverPassword(const QString &username, const QString &securityAnswer, const QString &newPassword);
    Response getAllGenres();
    Response getAllCategories();
    Response setFavoriteGenres(int userId, const QVector<int> &genreIds);
    Response getAccountInfo(int userId, UserRole role);
    Response updateAccount(int userId, UserRole role, const QVariantMap &accountData);
private:
    Response validateNormalUserRegistration(const QString &username, const QString &plainPassword, const QString &plainAnswer) const;
    Response validatePublisherRegistration(const QString &username, const QString &plainPassword, const QString &plainAnswer, const QString &firstName,
                                           const QString &lastName, const QString &email, const QString &publicationName, const QString &licenseNumber) const;
    Response validatePasswordChangeInput(const QString &oldPassword, const QString &newPassword) const;
    Response validateRecoverPasswordInput(const QString &username, const QString &securityAnswer, const QString &newPassword) const;
    bool isValidEmail(const QString &email) const;
    Response buildLoginSuccessResponse(int userId, const QString &username, UserRole role, const QString &publicationName = QString()) const;
};

#endif // AUTHMANAGER_H

