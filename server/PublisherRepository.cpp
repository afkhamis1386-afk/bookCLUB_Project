#include "DatabaseManager.h"
#include "../common/Enums.h"
#include "PublisherRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PublisherRepository::PublisherRepository() {}
int PublisherRepository::insertPublisher(const Publisher &publisher) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery insertUser(db);
    insertUser.prepare(
        "INSERT INTO Users (Username, PasswordHash, IsBlocked, IsDeleted, RegisterDate, RoleID) "
        "OUTPUT INSERTED.UserID "
        "VALUES (:username, :passwordHash, :isBlocked, :isDeleted, :registerDate, :roleId)"
        );
    insertUser.bindValue(":username", publisher.getEncryptedUsername());
    insertUser.bindValue(":passwordHash", publisher.getPasswordHash());
    insertUser.bindValue(":isBlocked", publisher.getIsBlocked());
    insertUser.bindValue(":isDeleted", publisher.getIsDeleted());
    insertUser.bindValue(":registerDate", publisher.getRegisterDate());
    insertUser.bindValue(":roleId", static_cast<int>(UserRole::Publisher) + 1);
        if (!insertUser.exec() || !insertUser.next()) {
        qWarning() << "خطا در ثبت کاربر پایه برای ناشر:" << insertUser.lastError().text();
        return -1;
    }
    int newUserId = insertUser.value(0).toInt();
    QSqlQuery insertPub(db);
    insertPub.prepare(
        "INSERT INTO Publishers "
        "(UserID, FirstName, LastName, Email, ShortDescription, PublicationName, "
        "PublisherLicenseNumber, SecurityAnswerHash) "
        "VALUES (:userId, :firstName, :lastName, :email, :shortDesc, :pubName, :licenseNum, :answerHash)"
        );
    insertPub.bindValue(":userId", newUserId);
    insertPub.bindValue(":firstName", publisher.getFirstName());
    insertPub.bindValue(":lastName", publisher.getLastName());
    insertPub.bindValue(":email", publisher.getEmail());
    insertPub.bindValue(":shortDesc", publisher.getShortDescription());
    insertPub.bindValue(":pubName", publisher.getPublicationName());
    insertPub.bindValue(":licenseNum", publisher.getPublisherLicenseNumber());
    insertPub.bindValue(":answerHash", publisher.getHashedSecurityAnswer());
    if (!insertPub.exec()) {
        qWarning() << "خطا در ثبت اطلاعات ناشر:" << insertPub.lastError().text();
        return -1;
    }
    return newUserId;
}
Publisher* PublisherRepository::loadPublisherById(int userId) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT u.UserID, u.Username, u.PasswordHash, p.SecurityAnswerHash, "
        "u.IsBlocked, u.IsDeleted, u.RegisterDate, "
        "p.FirstName, p.LastName, p.Email, p.PublicationName, "
        "p.PublisherLicenseNumber, p.ShortDescription "
        "FROM Users u JOIN Publishers p ON u.UserID = p.UserID "
        "WHERE u.UserID = :userId"
        );
    query.bindValue(":userId", userId);
    if (!query.exec() || !query.next()) {
        qWarning() << "ناشر یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    Publisher *publisher = new Publisher(
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toString(),
        query.value(3).toString(),
        query.value(4).toBool(),
        query.value(5).toBool(),
        query.value(6).toDateTime(),
        query.value(7).toString(),
        query.value(8).toString(),
        query.value(9).toString(),
        query.value(10).toString(),
        query.value(11).toString(),
        query.value(12).toString()
        );
    QSqlQuery bookQuery(db);
    bookQuery.prepare("SELECT BookID FROM Books WHERE PublisherUserID = :userId AND IsDeleted = 0");
    bookQuery.bindValue(":userId", userId);
    QVector<int> bookIds;
    if (bookQuery.exec()) {
        while (bookQuery.next())
            bookIds.append(bookQuery.value(0).toInt());
    }
    publisher->setPublishedBooks(bookIds);
    return publisher;
}
bool PublisherRepository::updateProfile(int userId, const QString &firstName, const QString &lastName,
                                        const QString &email, const QString &shortDescription,
                                        const QString &publicationName) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "UPDATE Publishers SET "
        "FirstName = :firstName, LastName = :lastName, Email = :email, "
        "ShortDescription = :shortDesc, PublicationName = :pubName "
        "WHERE UserID = :userId"
        );
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":email", email);
    query.bindValue(":shortDesc", shortDescription);
    query.bindValue(":pubName", publicationName);
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qWarning() << "خطا در به‌روزرسانی پروفایل ناشر:" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<int> PublisherRepository::getAllPublisherIds() {
    QVector<int> ids;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT u.UserID FROM Users u JOIN Publishers p ON u.UserID = p.UserID "
        "WHERE u.IsDeleted = 0"
        );
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool PublisherRepository::isEmailTaken(const QString &email) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Publishers WHERE Email = :email");
    query.bindValue(":email", email);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
bool PublisherRepository::isLicenseNumberTaken(const QString &licenseNumber) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Publishers WHERE PublisherLicenseNumber = :license");
    query.bindValue(":license", licenseNumber);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
double PublisherRepository::getTotalRevenue(int publisherUserId) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT ISNULL(SUM(oi.UnitPrice * (1 - oi.DiscountPercent / 100.0)), 0) "
        "FROM OrderItems oi "
        "JOIN Books b ON oi.BookID = b.BookID "
        "JOIN Orders o ON oi.OrderID = o.OrderID "
        "JOIN Statuses s ON o.StatusID = s.StatusID "
        "WHERE b.PublisherUserID = :publisherId "
        "AND s.StatusTitle IN ('Paid', 'Completed')"
        );
    query.bindValue(":publisherId", publisherUserId);
    if (query.exec() && query.next())
        return query.value(0).toDouble();
    return 0;
}