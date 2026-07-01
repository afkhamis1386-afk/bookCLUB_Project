#ifndef USER_H
#define USER_H
#include <QString>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDataStream>
const QString SECURITY_QUESTION = "نام کتاب مورد علاقه شما چیست؟";
class User {
protected:
    int userId;
    QString encryptedUsername;
    QString passwordHash;
    QString hashedSecurityAnswer;
    bool isBlocked;
    bool isDeleted;
    QDateTime registerDate;
    static const QString encryptionKey;
    QString hashString(const QString& plainText) const;
    QString encryptString(const QString& plainText) const;
    QString decryptString(const QString& encrypted) const;
public:
    User();
    User(const QString& plainUsername, const QString& plainPassword, const QString& plainAnswer);
    User(int userId, const QString& encryptedUsername, const QString& passwordHash,
         const QString& answerHash, bool blocked, bool deleted, const QDateTime& regDate);
    virtual ~User();
    virtual QString getRole() const = 0;
    static bool isValidUsername(const QString& username);
    static bool isStrongPassword(const QString& password);
    int getUserId() const;
    QString getUsername() const;
    QString getEncryptedUsername() const;
    bool getIsBlocked() const;
    bool getIsDeleted() const;
    QDateTime getRegisterDate() const;
    QString getPasswordHash() const;
    QString getHashedSecurityAnswer() const;
    void setUserId(int _id);
    bool setUsername(const QString& newUsername);
    void setIsBlocked(bool blocked);
    void setIsDeleted(bool deleted);
    void setRegisterDate(const QDateTime& date);
    virtual bool verifyPassword(const QString& inputPassword) const;
    virtual bool changePassword(const QString& oldPassword, const QString& newPassword);
    virtual bool recoverPassword(const QString& answer, const QString& newPassword);
    void serializeBase(QDataStream& out) const;
    void deserializeBase(QDataStream& in);
};
#endif // USER_H
