#ifndef USER_H
#define USER_H
#include <QString>
#include <QCryptographicHash>
#include <QDateTime>
const QString SECURITY_QUESTION = "نام کتاب مورد علاقه شما چیست؟";
class User
{
protected :
    int userId;
    QString username;
    QString passwordHash;
    QString hashedSecurityAnswer;
    bool isBlocked;
    QDateTime registerDate;
    static const QString encryptionKey;
    QString hashString(const QString& plainText) const;
    QString encryptString(const QString& plainText) const;
    QString decryptString(const QString& encrypted) const;
public:
    User();
    User(const QString& us, const QString& plainPassword, const QString& plainAnswer);
    User(int userId, const QString& encryptedUsername, const QString& passwordHash, const QString& answerHash, bool blocked, const QDateTime& regDate );
    virtual ~User();
    virtual QString getRole() const = 0;
    int getUserId() const;
    QString getUsername() const;
    bool getIsBlocked() const;
    QDateTime getRegisterDate() const;
    QString getPasswordHash() const;
    QString getEncryptedUsername() const;
    void setUserId(int _id);
    void setUsername(const QString& newUsername);
    void setIsBlocked(bool blocked);
    void setRegisterDate(const QDateTime& date);
    virtual bool verifyPassword(const QString& inputPassword) const;
    virtual bool changePassword(const QString& oldPassword, const QString& newPassword);
    virtual bool recoverPassword( const QString& answer, const QString& newPassword);

};

#endif // USER_H

