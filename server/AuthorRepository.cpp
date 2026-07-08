#include "AuthorRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
AuthorRepository::AuthorRepository(){}
int AuthorRepository::insertAuthor(const Author &author){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Authors (AuthorName) OUTPUT INSERTED.AuthorID VALUES (:name)");
    query.bindValue(":name", author.getAuthorName());
    if(!query.exec() || !query.next()){
        qWarning() << "خطا در ثبت نویسنده جدید:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Author* AuthorRepository::loadAuthorById(int authorId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT AuthorID, AuthorName FROM Authors WHERE AuthorID = :authorId");
    query.bindValue(":authorId", authorId);
    if(!query.exec() || !query.next()){
        qWarning() << "نویسنده یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    return new Author(query.value(0).toInt(), query.value(1).toString());
}
bool AuthorRepository::updateAuthorName(int authorId, const QString &newName){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Authors SET AuthorName = :name WHERE AuthorID = :authorId");
    query.bindValue(":name", newName);
    query.bindValue(":authorId", authorId);
    if(!query.exec()){
        qWarning() << "خطا در ویرایش نام نویسنده:" << query.lastError().text();
        return false;
    }
    return true;
}
bool AuthorRepository::deleteAuthor(int authorId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Authors WHERE AuthorID = :authorId");
    query.bindValue(":authorId", authorId);
    if(!query.exec()){
        qWarning() << "خطا در حذف نویسنده (احتمالاً کتابی به این نویسنده وابسته است):" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<Author> AuthorRepository::getAllAuthors(){
    QVector<Author> authors;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT AuthorID, AuthorName FROM Authors ORDER BY AuthorName");
    if(query.exec()){
        while(query.next())
            authors.append(Author(query.value(0).toInt(), query.value(1).toString()));
    }
    return authors;
}
bool AuthorRepository::authorExists(const QString &name){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Authors WHERE AuthorName = :name");
    query.bindValue(":name", name);
    if(query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
int AuthorRepository::getOrCreateAuthor(const QString &authorName){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery selectQuery(db);
    selectQuery.prepare("SELECT AuthorID FROM Authors WHERE AuthorName = :name");
    selectQuery.bindValue(":name", authorName);
    if(selectQuery.exec() && selectQuery.next()){
        return selectQuery.value(0).toInt();
    }
    Author newAuthor;
    newAuthor.setAuthorName(authorName);
    return insertAuthor(newAuthor);
}


