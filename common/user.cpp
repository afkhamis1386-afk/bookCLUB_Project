#include "user.h"
#include <QRegularExpression>
#include <stdexcept>
using namespace std;
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
bool User::isValidUsername(const QString& username) {
    QString trimmed = username.trimmed();
    if (trimmed.length() < 3 || trimmed.length() > 20)
        return false;
    static const QRegularExpression regex(R"(^[a-zA-Z0-9_\-]+$)");
    return regex.match(trimmed).hasMatch();
}
bool User::isStrongPassword(const QString& password) {
    if (password.length() < 8)
        return false;
    static const QRegularExpression upperRegex("[A-Z]");
    static const QRegularExpression lowerRegex("[a-z]");
    static const QRegularExpression digitRegex("[0-9]");
    return upperRegex.match(password).hasMatch() &&
           lowerRegex.match(password).hasMatch() &&
           digitRegex.match(password).hasMatch();
}
User::User()
    : userId(-1),
    encryptedUsername(""),
    passwordHash(""),
    hashedSecurityAnswer(""),
    isBlocked(false),
    isDeleted(false),
    registerDate(QDateTime::currentDateTime()) {}
User::User(const QString& plainUsername, const QString& plainPassword, const QString& plainAnswer)
    : userId(-1),
    isBlocked(false),
    isDeleted(false),
    registerDate(QDateTime::currentDateTime()){
    if (!isValidUsername(plainUsername))
        throw invalid_argument("!نام کاربری نامعتبر است. طول مجاز ۳ تا ۲۰ کاراکتر و فقط حروف انگلیسی، اعداد و خط تیره مجاز است");
    if (!isStrongPassword(plainPassword))
        throw invalid_argument("!رمز عبور ضعیف است. رمز باید حداقل ۸ کاراکتر و شامل حروف بزرگ، کوچک و عدد باشد");
    if (plainAnswer.trimmed().isEmpty())
        throw invalid_argument("!پاسخ سوال امنیتی نمی‌تواند خالی باشد");
    this->encryptedUsername = encryptString(plainUsername.trimmed());
    this->passwordHash = hashString(plainPassword);
    this->hashedSecurityAnswer = hashString(plainAnswer.trimmed());
}
User::User(int userId, const QString& encryptedUsername, const QString& passwordHash, const QString& answerHash, bool blocked, bool deleted, const QDateTime& regDate)
    : userId(userId),
    encryptedUsername(encryptedUsername),
    passwordHash(passwordHash),
    hashedSecurityAnswer(answerHash),
    isBlocked(blocked),
    isDeleted(deleted),
    registerDate(regDate) {}
User::~User(){}
int User::getUserId() const { return userId; }
QString User::getUsername() const { return decryptString(encryptedUsername); }
QString User::getEncryptedUsername() const { return encryptedUsername; }
bool User::getIsBlocked() const { return isBlocked; }
bool User::getIsDeleted() const { return isDeleted; }
QDateTime User::getRegisterDate() const { return registerDate; }
QString User::getPasswordHash() const { return passwordHash; }
QString User::getHashedSecurityAnswer() const { return hashedSecurityAnswer; }
void User::setUserId(int _id) { userId = _id; }
void User::setUsername(const QString& newUsername) {
    if (!isValidUsername(newUsername))
        throw invalid_argument("!نام کاربری جدید معتبر نیست");
    encryptedUsername = encryptString(newUsername.trimmed());
}
void User::setIsBlocked(bool blocked) { isBlocked = blocked; }
void User::setIsDeleted(bool deleted) { isDeleted = deleted; }
void User::setRegisterDate(const QDateTime& date) { registerDate = date; }
bool User::verifyPassword(const QString& inputPassword) const {
    return (this->passwordHash == hashString(inputPassword));
}
bool User::changePassword(const QString& oldPassword, const QString& newPassword) {
    if (passwordHash == hashString(oldPassword)) {
        if (!isStrongPassword(newPassword))
            throw invalid_argument("!رمز عبور جدید ضعیف است");
        if (hashString(newPassword) != passwordHash) {
            passwordHash = hashString(newPassword);
            return true;
        }
    }
    return false;
}

bool User::recoverPassword(const QString& answer, const QString& newPassword) {
    if (hashedSecurityAnswer == hashString(answer.trimmed())) {
        if (!isStrongPassword(newPassword))
            throw invalid_argument("رمز عبور جدید ضعیف است.");
        if (hashString(newPassword) != passwordHash) {
            passwordHash = hashString(newPassword);
            return true;
        }
    }
    return false;
}
void User::serializeBase(QDataStream& out) const {
    out << quint32(1);
    out << userId
        << encryptedUsername
        << passwordHash
        << hashedSecurityAnswer
        << isBlocked
        << isDeleted
        << registerDate;
}
void User::deserializeBase(QDataStream& in) {
    quint32 version;
    in >> version;
    if (version == 1) {
        in >> userId
            >> encryptedUsername
            >> passwordHash
            >> hashedSecurityAnswer
            >> isBlocked
            >> isDeleted
            >> registerDate;
    }
}
QDataStream& operator<<(QDataStream& out, const User& user) {
    user.serializeBase(out);
    return out;
}
QDataStream& operator>>(QDataStream& in, User& user) {
    user.deserializeBase(in);
    return in;
}