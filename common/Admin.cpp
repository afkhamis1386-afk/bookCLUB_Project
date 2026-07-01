#include "Admin.h"
Admin::Admin():User(){}
Admin::Admin(const QString &username, const QString &plainPassword, const QString &plainAnswer, const QString &firstName, const QString &lastName)
    :User(username, plainPassword, plainAnswer), firstName(firstName), lastName(lastName){}
Admin::Admin(int userId, const QString &encryptedUsername, const QString &passwordHash, const QString &answerHash, bool isBlocked, bool isDeleted, const QDateTime &registerDate, const QString &firstName, const QString &lastName)
    :User(userId, encryptedUsername, passwordHash, answerHash, isBlocked, isDeleted, registerDate), firstName(firstName), lastName(lastName){}
Admin::~Admin(){}
QString Admin::getRole() const {
    return "Admin";
}
QString Admin::getFirstName() const { return firstName; }
QString Admin::getLastName() const { return lastName; }
QString Admin::getFullName() const {
    return firstName + " " + lastName;
}
bool Admin::setFirstName(const QString &name){
    if(name.trimmed().isEmpty() || name.length() > 30)
        return false;
    firstName = name;
    return true;
}
bool Admin::setLastName(const QString &name){
    if(name.trimmed().isEmpty() || name.length() > 30)
        return false;
    lastName = name;
    return true;
}
QDataStream &operator<<(QDataStream &out, const Admin &admin){
    admin.serializeBase(out);
    out << admin.firstName << admin.lastName;
    return out;
}
QDataStream &operator>>(QDataStream &in, Admin &admin){
    admin.deserializeBase(in);
    in >> admin.firstName >> admin.lastName;
    return in;
}