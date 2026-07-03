#include "AdminRepository.h"
#include "../common/Enums.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
AdminRepository::AdminRepository(){}
int AdminRepository::insertAdmin(const Admin &admin){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery insertUser(db);
    insertUser.prepare(
        "INSERT INTO Users (Username, PasswordHash, IsBlocked, IsDeleted, RegisterDate, RoleID) "
        "OUTPUT INSERTED.UserID "
        "VALUES (:username, :passwordHash, :isBlocked, :isDeleted, :registerDate, :roleId)");
    insertUser.bindValue(":username", admin.getEncryptedUsername());
    insertUser.bindValue(":passwordHash", admin.getPasswordHash());
    insertUser.bindValue(":isBlocked", admin.getIsBlocked());
    insertUser.bindValue(":isDeleted", admin.getIsDeleted());
    insertUser.bindValue(":registerDate", admin.getRegisterDate());
    insertUser.bindValue(":roleId", static_cast<int>(UserRole::Admin) + 1);
    if(!insertUser.exec() || !insertUser.next()){
        qWarning() << "خطا در ثبت کاربر پایه برای ادمین:" << insertUser.lastError().text();
        return -1;
    }
    int newUserId = insertUser.value(0).toInt();
    QSqlQuery insertAdm(db);
    insertAdm.prepare(
        "INSERT INTO ApplicationAdmins (UserID, FirstName, LastName) "
        "VALUES (:userId, :firstName, :lastName)");
    insertAdm.bindValue(":userId", newUserId);
    insertAdm.bindValue(":firstName", admin.getFirstName());
    insertAdm.bindValue(":lastName", admin.getLastName());
    if(!insertAdm.exec()){
        qWarning() << "خطا در ثبت اطلاعات ادمین:" << insertAdm.lastError().text();
        return -1;
    }
    return newUserId;
}
Admin* AdminRepository::loadAdminById(int userId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT u.UserID, u.Username, u.PasswordHash, "
        "u.IsBlocked, u.IsDeleted, u.RegisterDate, "
        "a.FirstName, a.LastName "
        "FROM Users u JOIN ApplicationAdmins a ON u.UserID = a.UserID "
        "WHERE u.UserID = :userId" );
    query.bindValue(":userId", userId);
    if(!query.exec() || !query.next()){
        qWarning() << "ادمین یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    Admin *admin = new Admin(
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toString(),
        QString(),
        query.value(3).toBool(),
        query.value(4).toBool(),
        query.value(5).toDateTime(),
        query.value(6).toString(),
        query.value(7).toString());
    return admin;
}
bool AdminRepository::updateFirstName(int userId, const QString &firstName){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE ApplicationAdmins SET FirstName = :firstName WHERE UserID = :userId");
    query.bindValue(":firstName", firstName);
    query.bindValue(":userId", userId);
    if(!query.exec()){
        qWarning() << "خطا در به روزرسانی نام ادمین:" << query.lastError().text();
        return false;
    }
    return true;
}
bool AdminRepository::updateLastName(int userId, const QString &lastName){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE ApplicationAdmins SET LastName = :lastName WHERE UserID = :userId");
    query.bindValue(":lastName", lastName);
    query.bindValue(":userId", userId);
    if(!query.exec()){
        qWarning() << "خطا در به روزرسانی نام خانوادگی ادمین:" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<int> AdminRepository::getAllAdminIds(){
    QVector<int> ids;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT u.UserID FROM Users u JOIN ApplicationAdmins a ON u.UserID = a.UserID "
        "WHERE u.IsDeleted = 0");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool AdminRepository::isAdminExists(int userId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM ApplicationAdmins WHERE UserID = :userId");
    query.bindValue(":userId", userId);
    if(query.exec() && query.next()){
        return query.value(0).toInt() > 0;
    }
    return false;
}
