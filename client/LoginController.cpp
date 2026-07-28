#include "LoginController.h"
LoginController::LoginController(NetworkManager *networkManager, QObject *parent):QObject(parent), networkManager(networkManager){
    connect(networkManager, &NetworkManager::responseReceived, this, &LoginController::onResponseReceived);
    connect(networkManager, &NetworkManager::loginSucceeded, this, &LoginController::onLoginSucceeded);
    connect(networkManager, &NetworkManager::loginFailed, this, &LoginController::onLoginFailed);
}
void LoginController::attemptLogin(const QString &username, const QString &password){
    if(username.trimmed().isEmpty() || password.isEmpty()){
        emit validationError("لطفاً نام کاربری و رمز عبور را وارد کنید");
        return;
    }
    if(!networkManager->isConnected()){
        emit loginFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->login(username.trimmed(), password);
}
void LoginController::attemptRegisterNormalUser(const QString &username, const QString &password, const QString &confirmPassword, const QString &securityAnswer) {
    if(username.trimmed().isEmpty() || password.isEmpty() || securityAnswer.trimmed().isEmpty()){
        emit validationError("تمامی فیلدها را پر کنید");
        return;
    }
    if(password != confirmPassword){
        emit validationError("رمز عبور و تکرار آن یکسان نیستند");
        return;
    }
    if(!networkManager->isConnected()){
        emit registrationFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->registerNormalUser(username.trimmed(), password, securityAnswer.trimmed());
}
void LoginController::attemptRegisterPublisher(const QString &username, const QString &password, const QString &confirmPassword, const QString &securityAnswer, const QString &firstName, const QString &lastName,
                                               const QString &email, const QString &publicationName, const QString &licenseNumber, const QString &shortDescription){
    if(username.trimmed().isEmpty() || password.isEmpty() || securityAnswer.trimmed().isEmpty() || firstName.trimmed().isEmpty() || lastName.trimmed().isEmpty() || email.trimmed().isEmpty() || publicationName.trimmed().isEmpty() || licenseNumber.trimmed().isEmpty()){
        emit validationError("تمامی فیلدهای اجباری را پر کنید");
        return;
    }
    if(password != confirmPassword){
        emit validationError("رمز عبور و تکرار آن یکسان نیستند");
        return;
    }
    if(!networkManager->isConnected()){
        emit registrationFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->registerPublisher(username.trimmed(), password, securityAnswer.trimmed(), firstName.trimmed(), lastName.trimmed(), email.trimmed(), publicationName.trimmed(), licenseNumber.trimmed(), shortDescription.trimmed());
}
void LoginController::attemptRecoverPassword(const QString &username, const QString &securityAnswer, const QString &newPassword, const QString &confirmNewPassword){
    if(username.trimmed().isEmpty() || securityAnswer.trimmed().isEmpty() || newPassword.isEmpty()){
        emit validationError("تمامی فیلدها را پر کنید");
        return;
    }
    if(newPassword != confirmNewPassword){
        emit validationError("رمز عبور جدید و تکرار آن یکسان نیستند");
        return;
    }
    if(!networkManager->isConnected()){
        emit recoveryFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->recoverPassword(username.trimmed(), securityAnswer.trimmed(), newPassword);
}
void LoginController::onLoginSucceeded(int userId, UserRole role){
    Q_UNUSED(userId)
    emit loginSucceeded(role);
}
void LoginController::onLoginFailed(const QString &message){
    emit loginFailed(message);
}
void LoginController::onResponseReceived(RequestType type, const Response &response){
    switch(type){
    case RequestType::Register:
        if(response.isSuccess())
            emit registrationSucceeded(response.getMessage());
        else
            emit registrationFailed(response.getMessage());
        break;
    case RequestType::RecoverPassword:
        if(response.isSuccess())
            emit recoverySucceeded(response.getMessage());
        else
            emit recoveryFailed(response.getMessage());
        break;
    default:
        break;
    }
}