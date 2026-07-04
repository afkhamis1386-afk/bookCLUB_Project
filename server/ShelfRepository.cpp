#include "DatabaseManager.h"
#include "ShelfRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
ShelfRepository::ShelfRepository(){}
int ShelfRepository::insertShelf(const Shelf &shelf){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Shelves (UserID, ShelfName) OUTPUT INSERTED.ShelfID "
        "VALUES (:userId, :shelfName)");
    query.bindValue(":userId", shelf.getUserId());
    query.bindValue(":shelfName", shelf.getShelfName());
    if(!query.exec() || !query.next()) {
        qWarning() << "خطا در ساخت قفسه جدید:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Shelf* ShelfRepository::loadShelfById(int shelfId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT ShelfID, UserID, ShelfName FROM Shelves WHERE ShelfID = :shelfId");
    query.bindValue(":shelfId", shelfId);
    if(!query.exec() || !query.next()){
        qWarning() << "قفسه یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    Shelf *shelf = new Shelf(
        query.value(0).toInt(),
        query.value(1).toInt(),
        query.value(2).toString());
    QSqlQuery bookQuery(db);
    bookQuery.prepare("SELECT BookID FROM ShelfBooks WHERE ShelfID = :shelfId");
    bookQuery.bindValue(":shelfId", shelfId);
    QVector<int> bookIds;
    if(bookQuery.exec()){
        while (bookQuery.next())
            bookIds.append(bookQuery.value(0).toInt());
    }
    shelf->setBookIds(bookIds);

    return shelf;
}
QVector<int> ShelfRepository::getShelfIdsByUser(int userId){
    QVector<int> ids;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT ShelfID FROM Shelves WHERE UserID = :userId ORDER BY ShelfName");
    query.bindValue(":userId", userId);
    if(query.exec()){
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool ShelfRepository::updateShelfName(int shelfId, const QString &newName){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Shelves SET ShelfName = :name WHERE ShelfID = :shelfId");
    query.bindValue(":name", newName);
    query.bindValue(":shelfId", shelfId);
    if(!query.exec()){
        qWarning() << "خطا در ویرایش نام قفسه:" << query.lastError().text();
        return false;
    }
    return true;
}
bool ShelfRepository::deleteShelf(int shelfId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery deleteItems(db);
    deleteItems.prepare("DELETE FROM ShelfBooks WHERE ShelfID = :shelfId");
    deleteItems.bindValue(":shelfId", shelfId);
    if(!deleteItems.exec()){
        qWarning() << "خطا در حذف کتاب های قفسه:" << deleteItems.lastError().text();
        return false;
    }
    QSqlQuery deleteShelf(db);
    deleteShelf.prepare("DELETE FROM Shelves WHERE ShelfID = :shelfId");
    deleteShelf.bindValue(":shelfId", shelfId);
    if(!deleteShelf.exec()){
        qWarning() << "خطا در حذف قفسه:" << deleteShelf.lastError().text();
        return false;
    }
    return true;
}
bool ShelfRepository::addBookToShelf(int shelfId, int bookId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("INSERT INTO ShelfBooks (ShelfID, BookID) VALUES (:shelfId, :bookId)");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":bookId", bookId);
    if(!query.exec()){
        qWarning() << "خطا در افزودن کتاب به قفسه:" << query.lastError().text();
        return false;
    }
    return true;
}
bool ShelfRepository::removeBookFromShelf(int shelfId, int bookId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM ShelfBooks WHERE ShelfID = :shelfId AND BookID = :bookId");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":bookId", bookId);
    if(!query.exec()){
        qWarning() << "خطا در حذف کتاب از قفسه:" << query.lastError().text();
        return false;
    }
    return true;
}
bool ShelfRepository::shelfNameExistsForUser(int userId, const QString &shelfName){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT COUNT(*) FROM Shelves WHERE UserID = :userId AND ShelfName = :name");
    query.bindValue(":userId", userId);
    query.bindValue(":name", shelfName);
    if(query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
bool ShelfRepository::shelfBelongsToUser(int shelfId, int userId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Shelves WHERE ShelfID = :shelfId AND UserID = :userId");
    query.bindValue(":shelfId", shelfId);
    query.bindValue(":userId", userId);
    if(query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}