#include "ReadingProgressRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ReadingProgressRepository::ReadingProgressRepository() {}
bool ReadingProgressRepository::upsertProgress(int userId, int bookId, int lastPage) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM ReadingProgress WHERE UserID = :userId AND BookID = :bookId");
    checkQuery.bindValue(":userId", userId);
    checkQuery.bindValue(":bookId", bookId);
    if (!checkQuery.exec() || !checkQuery.next()) {
        qWarning() << "خطا در بررسی پیشرفت مطالعه:" << checkQuery.lastError().text();
        return false;
    }
    bool exists = checkQuery.value(0).toInt() > 0;
    QSqlQuery query(db);
    if (exists) {
        query.prepare("UPDATE ReadingProgress SET LastPage = :lastPage WHERE UserID = :userId AND BookID = :bookId");
    } else {
        query.prepare("INSERT INTO ReadingProgress (UserID, BookID, LastPage) VALUES (:userId, :bookId, :lastPage)");
    }
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    query.bindValue(":lastPage", lastPage);
    if (!query.exec()) {
        qWarning() << "خطا در ذخیره پیشرفت مطالعه:" << query.lastError().text();
        return false;
    }
    return true;
}
int ReadingProgressRepository::getLastPage(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT LastPage FROM ReadingProgress WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 1;
}