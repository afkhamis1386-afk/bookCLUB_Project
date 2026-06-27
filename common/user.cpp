#include "user.h"
const QString User::encryptionKey = "MySecretKey1405!";
QString User::hashString(const QString& plainText) const {
    return QString(QCryptographicHash::hash(
        plainText.toUtf8(),
        QCryptographicHash::Sha256
               ).toHex());
}
QString User::encryptString(const QString& plainText) const {
    QByteArray input = plainText.toUtf8();
    QByteArray result;
    for (int i = 0; i < input.size(); ++i) {
        result.append(input[i] ^ encryptionKey[i % encryptionKey.size()].toLatin1());
    }
    return QString(result.toBase64());
}
QString User::decryptString(const QString& encrypted) const {
    QByteArray input = QByteArray::fromBase64(encrypted.toUtf8());
    QByteArray result;
    for (int i = 0; i < input.size(); ++i) {
        result.append(input[i] ^ encryptionKey[i % encryptionKey.size()].toLatin1());
    }
    return QString::fromUtf8(result);
}
User::User(): userId(-1), username(""), passwordHash (""), securityQuestion(""), hashedSecurityAnswer(""), isBlocked(false){}

User::User(QString us, QString plainPassword, QString sq, QString plainAnswer) :
    userId(-1), username(encryptString(us)), passwordHash(hashString(plainPassword)), hashedSecurityAnswer(hashString(plainAnswer)), securityQuestion(sq), isBlocked(false) {}

User::User(int userId, QString us, QString passwordHash, QString sq, QString answerHash, bool isBlocked) :
    userId(userId), username(us), passwordHash(passwordHash), securityQuestion(sq), hashedSecurityAnswer(answerHash),isBlocked(isBlocked) {}

User::~User(){}
int User::getUserId() const { return userId; }
bool User::getIsBlocked() const { return isBlocked; }
QString User::getSecurityQuestion() const { return securityQuestion; }
QString User::getUsername() const { return decryptString(username); }
void User::setUserId(int _id) { userId = _id; }
void User::setUsername(QString newUsername) { username = encryptString(newUsername); }
void User::setSecurityQuestion(QString question) { securityQuestion = question; }
void User::setIsBlocked(bool blocked) { isBlocked = blocked; }
bool User::verifyPassword(QString inputPassword) {
    if (isBlocked)
        return false;
    return (this -> passwordHash == hashString(inputPassword));
}
bool User::changePassword(QString oldPassword, QString newPassword) {
    if (passwordHash == hashString(oldPassword)) {
        if (!newPassword.isEmpty() && hashString(newPassword) != passwordHash) {
            passwordHash = hashString(newPassword);
            return true;
        }
    }
    return false;
}

bool User::recoverPassword(QString answer, QString newPassword) {
    if (hashedSecurityAnswer == hashString(answer)) {
        if (!newPassword.isEmpty() && hashString(newPassword) != passwordHash) {
            passwordHash = hashString(newPassword);
            return true;
        }
    }
    return false;
}
