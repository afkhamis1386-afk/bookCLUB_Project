#include "user.h"
#include <QRegularExpression>

const QString User::encryptionKey = "MySecretKey1405!";
QString User::hashString(const QString &plainText) {
    return QString(QCryptographicHash::hash( plainText.toUtf8(), QCryptographicHash::Sha256).toHex());
}
QString User::encryptString(const QString &plainText) {
    QByteArray input = plainText.toUtf8();
    QByteArray result;
    for (int i = 0; i < input.size(); ++i)
        result.append(input[i] ^ encryptionKey[i % encryptionKey.size()].toLatin1());
    return QString(result.toBase64());
}
QString User::decryptString(const QString &encrypted) {
    QByteArray input = QByteArray::fromBase64(encrypted.toUtf8());
    QByteArray result;
    for (int i = 0; i < input.size(); ++i)
        result.append(input[i] ^ encryptionKey[i % encryptionKey.size()].toLatin1());
    return QString::fromUtf8(result);
}
bool User::isValidUsername(const QString &username) {
    QString trimmed = username.trimmed();
    if (trimmed.length() < 3 || trimmed.length() > 15)
        return false;
    static const QRegularExpression regex(R"(^[a-zA-Z0-9_\-]+$)");
    return regex.match(trimmed).hasMatch();
}
bool User::isStrongPassword(const QString &password) {
    if (password.length() < 8)
        return false;
    static const QRegularExpression upperRegex("[A-Z]");
    static const QRegularExpression lowerRegex("[a-z]");
    static const QRegularExpression digitRegex("[0-9]");
    return upperRegex.match(password).hasMatch() && lowerRegex.match(password).hasMatch() && digitRegex.match(password).hasMatch();
}
User::User()
    : userId(-1), encryptedUsername(""), passwordHash(""), hashedSecurityAnswer(""),
    isBlocked(false), isDeleted(false),isActive(true), registerDate(QDateTime::currentDateTime()) {}
User::User(const QString &plainUsername, const QString &plainPassword, const QString &plainAnswer)
    : userId(-1), isBlocked(false), isDeleted(false), isActive(true),
    registerDate(QDateTime::currentDateTime()) {
    encryptedUsername = encryptString(plainUsername.trimmed());
    passwordHash = hashString(plainPassword);
    hashedSecurityAnswer = hashString(plainAnswer.trimmed());
}
User::User(int userId, const QString &encryptedUsername, const QString &passwordHash, const QString &answerHash, bool blocked, bool deleted, bool active, const QDateTime &regDate)
    : userId(userId), encryptedUsername(encryptedUsername), passwordHash(passwordHash),
    hashedSecurityAnswer(answerHash), isBlocked(blocked), isDeleted(deleted), isActive(active),
    registerDate(regDate) {}
User::~User() {}
int User::getUserId() const { return userId; }
QString User::getUsername() const { return decryptString(encryptedUsername); }
QString User::getEncryptedUsername() const { return encryptedUsername; }
QString User::getPasswordHash() const { return passwordHash; }
QString User::getHashedSecurityAnswer() const { return hashedSecurityAnswer; }
bool User::getIsBlocked() const { return isBlocked; }
bool User::getIsDeleted() const { return isDeleted; }
bool User::getIsActive() const {return isActive; }
QDateTime User::getRegisterDate() const { return registerDate; }
void User::setUserId(int _id) { userId = _id; }
bool User::setUsername(const QString &newUsername) {
    if (!isValidUsername(newUsername))
        return false;
    encryptedUsername = encryptString(newUsername.trimmed());
    return true;
}
void User::setIsBlocked(bool blocked) { isBlocked = blocked; }
void User::setIsDeleted(bool deleted) { isDeleted = deleted; }
void User::setIsActive (bool active) {isActive = active; }
void User::setRegisterDate(const QDateTime &date) { registerDate = date; }
bool User::verifyPassword(const QString &inputPassword) const {
    return passwordHash == hashString(inputPassword);
}
bool User::changePassword(const QString &oldPassword, const QString &newPassword) {
    if (passwordHash != hashString(oldPassword))
        return false;
    if (hashString(newPassword) == passwordHash)
        return false;
    passwordHash = hashString(newPassword);
    return true;
}
bool User::recoverPassword(const QString &answer, const QString &newPassword) {
    if (hashedSecurityAnswer != hashString(answer))
        return false;
    if (hashString(newPassword) == passwordHash)
        return false;
    passwordHash = hashString(newPassword);
    return true;
}
void User::serializeBase(QDataStream &out) const {
    out << userId << encryptedUsername << passwordHash << hashedSecurityAnswer
        << isBlocked << isDeleted << isActive << registerDate;
}
void User::deserializeBase(QDataStream &in) {
    in >> userId >> encryptedUsername >> passwordHash >> hashedSecurityAnswer
        >> isBlocked >> isDeleted >> isActive >> registerDate;
}