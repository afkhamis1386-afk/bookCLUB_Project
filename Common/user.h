#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>

class User : public QObject
{
    Q_OBJECT
protected :
    QString username;
    QString passwordHash;
    QString securityQuestion;
    QString hashedSecurityAnswer;
    bool isBlocked;
public:
    User();
    User( QString us, QString pwh, QString sq, QString hsqa, bool ib);
    virtual ~User();
    virtual QString getRole() const = 0;
    bool getIsBlocked() const;
    QString getSecurityQuestion() const;
    QString getUsername() const;
    void setUsername(QString newUsername);
    void setSecurityQuestion(QString question);
    void setIsBlocked(bool blocked);
    virtual bool login(QString inputPassword);
    virtual void logout();
    virtual bool changePassword(QString oldPassword ,QString newPassword);
    virtual bool recoverPassword( QString answer, QString newPassword);
};

#endif // USER_H
