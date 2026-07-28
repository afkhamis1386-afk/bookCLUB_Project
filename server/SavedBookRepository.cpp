#include "SavedBookRepository.h"
#include "DatabaseManager.h"
#include <QSqlDatabase>
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
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش حذف کتاب:" << db.lastError().text();
        return false;
    }
    QSqlQuery favoriteQuery(db);
    favoriteQuery.prepare("DELETE FROM FavouriteBooks WHERE UserID = :userId AND BookID = :bookId");
    favoriteQuery.bindValue(":userId", userId);
    favoriteQuery.bindValue(":bookId", bookId);
    if (!favoriteQuery.exec()) {
        qWarning() << "خطا در حذف کتاب از لیست علاقه مندی:" << favoriteQuery.lastError().text();
        db.rollback();
        return false;
    }
    QSqlQuery query(db);
    query.prepare("DELETE FROM SavedBooks WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در حذف کتاب ذخیره شده:" << query.lastError().text();
        db.rollback();
        return false;
    }
    if (!db.commit()) {
        qWarning() << "خطا در نهایی سازی حذف کتاب:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return query.numRowsAffected() > 0;
}
QVector<int> SavedBookRepository::getSavedBookIds(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM SavedBooks WHERE UserID = :userId "
        "ORDER BY SavedBookID");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    } else {
        qWarning() << "خطا در بازیابی کتاب های ذخیره شده:" << query.lastError().text();
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
bool SavedBookRepository::isFavoriteBook(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT COUNT(*) FROM FavouriteBooks "
        "WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
bool SavedBookRepository::addFavoriteBook(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.isOpen()) {
        qWarning() << "اتصال دیتابیس برای افزودن علاقه مندی باز نیست:" << db.lastError().text();
        return false;
    }
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش افزودن علاقه مندی:" << db.lastError().text();
        return false;
    }
    QSqlQuery orderQuery(db);
    orderQuery.prepare(
        "SELECT COALESCE(MAX(DisplayOrder), -1) + 1 "
        "FROM FavouriteBooks WITH (UPDLOCK, HOLDLOCK) WHERE UserID = :userId");
    orderQuery.bindValue(":userId", userId);
    if (!orderQuery.exec() || !orderQuery.next()) {
        qWarning() << "خطا در محاسبه ترتیب علاقه مندی جدید:" << orderQuery.lastError().text();
        db.rollback();
        return false;
    }
    QSqlQuery insertQuery(db);
    insertQuery.prepare(
        "INSERT INTO FavouriteBooks (UserID, BookID, DisplayOrder) "
        "VALUES (:userId, :bookId, :displayOrder)");
    insertQuery.bindValue(":userId", userId);
    insertQuery.bindValue(":bookId", bookId);
    insertQuery.bindValue(":displayOrder", orderQuery.value(0).toInt());
    if (!insertQuery.exec()) {
        qWarning() << "خطا در افزودن کتاب به لیست علاقه مندی ها:"
                   << insertQuery.lastError().text();
        db.rollback();
        return false;
    }
    if (!db.commit()) {
        qWarning() << "خطا در نهایی سازی افزودن علاقه مندی:" << db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}
bool SavedBookRepository::removeFavoriteBook(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "DELETE FROM FavouriteBooks WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در حذف کتاب از لیست علاقه مندی ها:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}
QVector<int> SavedBookRepository::getFavoriteBookIds(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM FavouriteBooks WHERE UserID = :userId "
        "ORDER BY DisplayOrder, FavouriteBookID");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    } else {
        qWarning() << "خطا در بازیابی لیست علاقه مندی ها:" << query.lastError().text();
    }
    return ids;
}
bool SavedBookRepository::reorderFavoriteBooks(int userId, const QVector<int> &bookIds) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش مرتب سازی علاقه مندی ها:" << db.lastError().text();
        return false;
    }
    QSqlQuery query(db);
    query.prepare(
        "UPDATE FavouriteBooks SET DisplayOrder = :displayOrder "
        "WHERE UserID = :userId AND BookID = :bookId");
    for (int i = 0; i < bookIds.size(); ++i) {
        query.bindValue(":displayOrder", i);
        query.bindValue(":userId", userId);
        query.bindValue(":bookId", bookIds.at(i));
        if (!query.exec()) {
            qWarning() << "خطا در ذخیره ترتیب علاقه مندی ها:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }
    if (!db.commit()) {
        qWarning() << "خطا در نهایی سازی ترتیب علاقه مندی ها:" << db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}