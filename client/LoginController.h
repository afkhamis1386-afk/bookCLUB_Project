#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H
#include <QObject>
#include "NetworkManager.h"
class LoginController : public QObject {
    Q_OBJECT
public:
    explicit LoginController(NetworkManager *networkManager, QObject *parent = nullptr);
    void attemptLogin(const QString &username, const QString &password);
    void attemptRegisterNormalUser(const QString &username, const QString &password, const QString &confirmPassword, const QString &securityAnswer, const QString &firstName, const QString &lastName);
    void attemptRegisterPublisher(const QString &username, const QString &password, const QString &confirmPassword, const QString &securityAnswer, const QString &firstName, const QString &lastName,
    const QString &email, const QString &publicationName, const QString &licenseNumber, const QString &shortDescription);
    void attemptRecoverPassword(const QString &username, const QString &securityAnswer, const QString &newPassword, const QString &confirmNewPassword);

signals:
    void loginSucceeded(UserRole role);
    void loginFailed(const QString &message);
    void registrationSucceeded(const QString &message);
    void registrationFailed(const QString &message);
    void recoverySucceeded(const QString &message);
    void recoveryFailed(const QString &message);
    void validationError(const QString &message);

private slots:
    void onResponseReceived(RequestType type, const Response &response);
    void onLoginSucceeded(int userId, UserRole role);
    void onLoginFailed(const QString &message);

private:
    NetworkManager *networkManager;
};

#endif // LOGINCONTROLLER_H
