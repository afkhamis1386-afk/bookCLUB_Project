#include "DatabaseManager.h"
#include "ShelfRepository.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ShelfRepository::ShelfRepository() {}

int ShelfRepository::insertShelf(const Shelf &shelf) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Shelves (UserID, ShelfName, DisplayOrder) OUTPUT INSERTED.ShelfID "
        "SELECT :userIdValue, :shelfName, COALESCE(MAX(DisplayOrder), -1) + 1 "
        "FROM Shelves WHERE UserID = :userIdOrder");
    query.bindValue(":userIdValue", shelf.getUserId());
    query.bindValue(":shelfName", shelf.getShelfName());
    query.bindValue(":userIdOrder", shelf.getUserId());
    if (!query.exec() || !query.next()) {
        qWarning() << "خطا در ساخت قفسه جدید:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}

Shelf* ShelfRepository::loadShelfById(int shelfId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT ShelfID, UserID, ShelfName FROM Shelves WHERE ShelfID = :shelfId");
    query.bindValue(":shelfId", shelfId);
    if (!query.exec() || !query.next()) {
        qWarning() << "قفسه یافت نشد:" << query.lastError().text();
        return nullptr;
    }

    Shelf *shelf = new Shelf(
        query.value(0).toInt(),
        query.value(1).toInt(),
        query.value(2).toString());
    shelf->setBookIds(getBookIdsByShelf(shelfId));
    return shelf;
}

QVector<int> ShelfRepository::getShelfIdsByUser(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT ShelfID FROM Shelves WHERE UserID = :userId "
        "ORDER BY DisplayOrder, ShelfID");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    } else {
        qWarning() << "خطا در بازیابی ترتیب قفسه ها:" << query.lastError().text();
    }
    return ids;
}

QVector<int> ShelfRepository::getBookIdsByShelf(int shelfId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM ShelfBooks WHERE ShelfID = :shelfId "
        "ORDER BY DisplayOrder, BookID");
    query.bindValue(":shelfId", shelfId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    } else {
        qWarning() << "خطا در بازیابی ترتیب کتاب های قفسه:" << query.lastError().text();
    }
    return ids;
}

bool ShelfRepository::updateShelfName(int shelfId, const QString &newName) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Shelves SET ShelfName = :name WHERE ShelfID = :shelfId");
    query.bindValue(":name", newName);
    query.bindValue(":shelfId", shelfId);
    if (!query.exec()) {
        qWarning() << "خطا در ویرایش نام قفسه:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ShelfRepository::deleteShelf(int shelfId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش حذف قفسه:" << db.lastError().text();
        return false;
    }

    QSqlQuery deleteItems(db);
    deleteItems.prepare("DELETE FROM ShelfBooks WHERE ShelfID = :shelfId");
    deleteItems.bindValue(":shelfId", shelfId);
    if (!deleteItems.exec()) {
        qWarning() << "خطا در حذف کتاب های قفسه:" << deleteItems.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery deleteShelfQuery(db);
    deleteShelfQuery.prepare("DELETE FROM Shelves WHERE ShelfID = :shelfId");
    deleteShelfQuery.bindValue(":shelfId", shelfId);
    if (!deleteShelfQuery.exec()) {
        qWarning() << "خطا در حذف قفسه:" << deleteShelfQuery.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qWarning() << "خطا در نهایی سازی تراکنش حذف قفسه:" << db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

bool ShelfRepository::addBookToShelf(int shelfId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO ShelfBooks (ShelfID, BookID, DisplayOrder) "
        "SELECT :shelfIdValue, :bookId, COALESCE(MAX(DisplayOrder), -1) + 1 "
        "FROM ShelfBooks WHERE ShelfID = :shelfIdOrder");
    query.bindValue(":shelfIdValue", shelfId);
    query.bindValue(":bookId", bookId);
    query.bindValue(":shelfIdOrder", shelfId);
    if (!query.exec()) {
        qWarning() << "خطا در افزودن کتاب به قفسه:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ShelfRepository::removeBookFromShelf(int shelfId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM ShelfBooks WHERE ShelfID = :shelfId AND BookID = :bookId");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در حذف کتاب از قفسه:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool ShelfRepository::reorderShelves(int userId, const QVector<int> &shelfIds) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش مرتب سازی قفسه ها:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "UPDATE Shelves SET DisplayOrder = :displayOrder "
        "WHERE ShelfID = :shelfId AND UserID = :userId");
    for (int i = 0; i < shelfIds.size(); ++i) {
        query.bindValue(":displayOrder", i);
        query.bindValue(":shelfId", shelfIds.at(i));
        query.bindValue(":userId", userId);
        if (!query.exec()) {
            qWarning() << "خطا در ذخیره ترتیب قفسه ها:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        qWarning() << "خطا در نهایی سازی ترتیب قفسه ها:" << db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

bool ShelfRepository::reorderShelfBooks(int shelfId, const QVector<int> &bookIds) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if(!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش مرتب سازی کتاب های قفسه:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "UPDATE ShelfBooks SET DisplayOrder = :displayOrder "
        "WHERE ShelfID = :shelfId AND BookID = :bookId");
    for(int i = 0; i < bookIds.size(); ++i) {
        query.bindValue(":displayOrder", i);
        query.bindValue(":shelfId", shelfId);
        query.bindValue(":bookId", bookIds.at(i));
        if (!query.exec()) {
            qWarning() << "خطا در ذخیره ترتیب کتاب های قفسه:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }
    if (!db.commit()) {
        qWarning() << "خطا در نهایی سازی ترتیب کتاب های قفسه:" << db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

bool ShelfRepository::shelfNameExistsForUser(int userId, const QString &shelfName) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Shelves WHERE UserID = :userId AND ShelfName = :name");
    query.bindValue(":userId", userId);
    query.bindValue(":name", shelfName);
    if(query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}

bool ShelfRepository::shelfBelongsToUser(int shelfId, int userId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Shelves WHERE ShelfID = :shelfId AND UserID = :userId");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":userId", userId);
    if(query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
