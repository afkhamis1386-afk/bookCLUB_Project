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
User::User(): userId(-1), username(""), passwordHash (""), hashedSecurityAnswer(""), isBlocked(false), registerDate(QDateTime::currentDateTime()) {}

User::User(const QString& us, const QString& plainPassword, const QString& plainAnswer) :
    userId(-1), username(encryptString(us)), passwordHash(hashString(plainPassword)), hashedSecurityAnswer(hashString(plainAnswer)),
    isBlocked(false), registerDate(QDateTime::currentDateTime()){}

User::User(int userId, const QString& encryptedUsername, const QString& passwordHash, const QString& answerHash, bool blocked, const QDateTime& regDate) :
    userId(userId), username(encryptedUsername), passwordHash(passwordHash), hashedSecurityAnswer(answerHash),
      isBlocked(blocked), registerDate(regDate) {}
User::~User(){}
int User::getUserId() const { return userId; }
QString User::getUsername() const { return decryptString(username); }
bool User::getIsBlocked() const { return isBlocked; }
QDateTime User::getRegisterDate() const { return registerDate; }
QString User::getPasswordHash() const { return passwordHash; }
QString User::getEncryptedUsername() const { return username; }
void User::setUserId(int _id) { userId = _id; }
void User::setUsername(const QString& newUsername) { username = encryptString(newUsername);}
void User::setIsBlocked(bool blocked) { isBlocked = blocked; }
void User::setRegisterDate(const QDateTime& date) { registerDate = date; }
bool User::verifyPassword(const QString& inputPassword) const {
    return (this -> passwordHash == hashString(inputPassword));
}
bool User::changePassword(const QString& oldPassword, const QString& newPassword) {
    if (passwordHash == hashString(oldPassword)) {
        if (!newPassword.isEmpty() && hashString(newPassword) != passwordHash) {
            passwordHash = hashString(newPassword);
            return true;
        }
    }
    return false;
}
bool User::recoverPassword(const QString& answer, const QString& newPassword) {
    if (hashedSecurityAnswer == hashString(answer)) {
        if (!newPassword.isEmpty() && hashString(newPassword) != passwordHash) {
            passwordHash = hashString(newPassword);
            return true;
        }
    }
    return false;
};
