#include "DatabaseManager.h"
#include "ShelfRepository.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ShelfRepository::ShelfRepository() {}

int ShelfRepository::insertShelf(const Shelf &shelf) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.isOpen()) {
        qWarning() << "اتصال دیتابیس برای ساخت قفسه باز نیست:" << db.lastError().text();
        return -1;
    }
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش ساخت قفسه:" << db.lastError().text();
        return -1;
    }

    QSqlQuery orderQuery(db);
    orderQuery.prepare(
        "SELECT COALESCE(MAX(DisplayOrder), -1) + 1 "
        "FROM Shelves WITH (UPDLOCK, HOLDLOCK) WHERE UserID = :userId");
    orderQuery.bindValue(":userId", shelf.getUserId());
    if (!orderQuery.exec() || !orderQuery.next()) {
        qWarning() << "خطا در محاسبه ترتیب قفسه جدید:" << orderQuery.lastError().text();
        db.rollback();
        return -1;
    }
    const int displayOrder = orderQuery.value(0).toInt();

    QSqlQuery insertQuery(db);
    insertQuery.prepare(
        "INSERT INTO Shelves (UserID, ShelfName, DisplayOrder) "
        "VALUES (:userId, :shelfName, :displayOrder)");
    insertQuery.bindValue(":userId", shelf.getUserId());
    insertQuery.bindValue(":shelfName", shelf.getShelfName());
    insertQuery.bindValue(":displayOrder", displayOrder);
    if (!insertQuery.exec()) {
        qWarning() << "خطا در ساخت قفسه جدید:" << insertQuery.lastError().text();
        db.rollback();
        return -1;
    }

    QSqlQuery idQuery(db);
    idQuery.prepare(
        "SELECT ShelfID FROM Shelves "
        "WHERE UserID = :userId AND ShelfName = :shelfName");
    idQuery.bindValue(":userId", shelf.getUserId());
    idQuery.bindValue(":shelfName", shelf.getShelfName());
    if (!idQuery.exec() || !idQuery.next()) {
        qWarning() << "قفسه ایجاد شد اما شناسه آن بازیابی نشد:" << idQuery.lastError().text();
        db.rollback();
        return -1;
    }
    const int shelfId = idQuery.value(0).toInt();

    if (!db.commit()) {
        qWarning() << "خطا در نهایی سازی ساخت قفسه:" << db.lastError().text();
        db.rollback();
        return -1;
    }
    return shelfId;
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
    if (!db.isOpen()) {
        qWarning() << "اتصال دیتابیس برای افزودن کتاب به قفسه باز نیست:" << db.lastError().text();
        return false;
    }

    QSqlQuery orderQuery(db);
    orderQuery.prepare(
        "SELECT COALESCE(MAX(DisplayOrder), -1) + 1 "
        "FROM ShelfBooks WHERE ShelfID = :shelfId");
    orderQuery.bindValue(":shelfId", shelfId);
    if (!orderQuery.exec() || !orderQuery.next()) {
        qWarning() << "خطا در محاسبه ترتیب کتاب قفسه:" << orderQuery.lastError().text();
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare(
        "INSERT INTO ShelfBooks (ShelfID, BookID, DisplayOrder) "
        "VALUES (:shelfId, :bookId, :displayOrder)");
    insertQuery.bindValue(":shelfId", shelfId);
    insertQuery.bindValue(":bookId", bookId);
    insertQuery.bindValue(":displayOrder", orderQuery.value(0).toInt());
    if (!insertQuery.exec()) {
        qWarning() << "خطا در افزودن کتاب به قفسه:" << insertQuery.lastError().text();
        return false;
    }
    return true;
}

bool ShelfRepository::shelfContainsBook(int shelfId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT COUNT(*) FROM ShelfBooks "
        "WHERE ShelfID = :shelfId AND BookID = :bookId");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":bookId", bookId);
    if (!query.exec() || !query.next()) {
        qWarning() << "خطا در بررسی وجود کتاب در قفسه:" << query.lastError().text();
        return false;
    }
    return query.value(0).toInt() > 0;
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
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش مرتب سازی کتاب های قفسه:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "UPDATE ShelfBooks SET DisplayOrder = :displayOrder "
        "WHERE ShelfID = :shelfId AND BookID = :bookId");
    for (int i = 0; i < bookIds.size(); ++i) {
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
    query.prepare(
        "SELECT COUNT(*) FROM Shelves WHERE UserID = :userId AND ShelfName = :name");
    query.bindValue(":userId", userId);
    query.bindValue(":name", shelfName);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}

bool ShelfRepository::shelfBelongsToUser(int shelfId, int userId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Shelves WHERE ShelfID = :shelfId AND UserID = :userId");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":userId", userId);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
