#include "SavedBookRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SavedBookRepository::SavedBookRepository() {}
bool SavedBookRepository::isBookSaved(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM SavedBooks WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
bool SavedBookRepository::saveBook(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("INSERT INTO SavedBooks (UserID, BookID) VALUES (:userId, :bookId)");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در ذخیره کتاب:" << query.lastError().text();
        return false;
    }
    return true;
}
bool SavedBookRepository::unsaveBook(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM SavedBooks WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    return query.exec();
}
QVector<int> SavedBookRepository::getSavedBookIds(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT BookID FROM SavedBooks WHERE UserID = :userId");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> SavedBookRepository::getUserIdsWhoSavedBook(int bookId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT UserID FROM SavedBooks WHERE BookID = :bookId");
    query.bindValue(":bookId", bookId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
