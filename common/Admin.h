#ifndef ADMIN_H
#define ADMIN_H
#include "user.h"
#include <QString>
#include <QDataStream>
class Admin : public User {
private:
    QString firstName;
    QString lastName;
public:
    Admin();
    Admin(const QString &username, const QString &plainPassword, const QString &plainAnswer, const QString &firstName, const QString &lastName);
    Admin(int userId, const QString &encryptedUsername, const QString &passwordHash, const QString &answerHash, bool isBlocked, bool isDeleted, const QDateTime &registerDate, const QString &firstName, const QString &lastName);
    ~Admin() override;
    QString getRole() const override;
    QString getFirstName() const;
    QString getLastName() const;
    QString getFullName() const;
    bool setFirstName(const QString &name);
    bool setLastName(const QString &name);
    friend QDataStream &operator<<(QDataStream &out, const Admin &admin);
    friend QDataStream &operator>>(QDataStream &in, Admin &admin);
};
#endif // ADMIN_H
