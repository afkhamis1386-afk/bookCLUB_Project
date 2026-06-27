#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QCryptographicHash>

class User : public QObject
{
    Q_OBJECT
protected :
    int userId;
    QString username;
    QString passwordHash;
    QString securityQuestion;
    QString hashedSecurityAnswer;
    bool isBlocked;
    static const QString encryptionKey;
    QString hashString(const QString& plainText) const;
    QString encryptString(const QString& plainText) const;
    QString decryptString(const QString& encrypted) const;
public:
    User();
    User(QString us, QString plainPassword, QString sq, QString plainAnswer);
    User(int userId, QString us, QString passwordHash, QString sq, QString answerHash, bool isBlocked);
    virtual ~User();
    int getUserId() const;
    virtual QString getRole() const = 0;
    bool getIsBlocked() const;
    QString getSecurityQuestion() const;
    QString getUsername() const;
    void setUserId(int _id);
    void setUsername(QString newUsername);
    void setSecurityQuestion(QString question);
    void setIsBlocked(bool blocked);
    virtual bool verifyPassword(QString inputPassword);
    virtual bool changePassword(QString oldPassword ,QString newPassword);
    virtual bool recoverPassword( QString answer, QString newPassword);
};

#endif // USER_H
