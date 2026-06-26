#include "user.h"
User::User(): username(""), passwordHash (""), securityQuestion(""), hashedSecurityAnswer(""), isBlocked(false){}
User::User( QString us ,QString pwh ,QString sq ,QString hsqa ,bool ib) :
    username(us), passwordHash(pwh), securityQuestion(sq), hashedSecurityAnswer(hsqa), isBlocked(ib){}
User::~User(){}
bool User::getIsBlocked() const { return isBlocked; }
QString User::getSecurityQuestion() const { return securityQuestion; }
QString User::getUsername() const { return username; }
void User::setUsername(QString newUsername) { username = newUsername; }
void User::setSecurityQuestion(QString question) { securityQuestion = question; }
void User::setIsBlocked(bool blocked) { isBlocked = blocked; }
bool User::login(QString inputPassword) {
    if (isBlocked)
        return false;
    return (this -> passwordHash == inputPassword);
}
void User::logout() {}
bool User::changePassword(QString oldPassword ,QString newPassword) {
    if ( this -> passwordHash == oldPassword) {
        if (!newPassword.isEmpty()) {
            this -> passwordHash = newPassword;
            return true;
        }
    }
    return false;
}
bool User::recoverPassword( QString answer, QString newPassword) {
    if ( this -> hashedSecurityAnswer == answer) {
        if (!newPassword.isEmpty()) {
            this -> passwordHash = newPassword;
            return true;
        }
    }
    return false;
}