#include "GenreRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
GenreRepository::GenreRepository(){}
int GenreRepository::insertGenre(const Genre &genre){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Genres (GenreTitle) OUTPUT INSERTED.GenreID VALUES (:title)");
    query.bindValue(":title", genre.getGenreTitle());
    if(!query.exec() || !query.next()){
        qWarning() << "خطا در ثبت ژانر جدید:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Genre* GenreRepository::loadGenreById(int genreId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT GenreID, GenreTitle FROM Genres WHERE GenreID = :genreId");
    query.bindValue(":genreId", genreId);
    if(!query.exec() || !query.next()){
        qWarning() << "ژانر یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    return new Genre(query.value(0).toInt(), query.value(1).toString());
}
bool GenreRepository::updateGenreTitle(int genreId, const QString &newTitle){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Genres SET GenreTitle = :title WHERE GenreID = :genreId");
    query.bindValue(":title", newTitle);
    query.bindValue(":genreId", genreId);
    if(!query.exec()){
        qWarning() << "خطا در ویرایش ژانر:" << query.lastError().text();
        return false;
    }
    return true;
}
bool GenreRepository::deleteGenre(int genreId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Genres WHERE GenreID = :genreId");
    query.bindValue(":genreId", genreId);
    if(!query.exec()){
        qWarning() << "خطا در حذف ژانر (احتمالاً کتابی به این ژانر وابسته است):" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<Genre> GenreRepository::getAllGenres(){
    QVector<Genre> genres;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT GenreID, GenreTitle FROM Genres ORDER BY GenreTitle");
    if(query.exec()){
        while(query.next())
            genres.append(Genre(query.value(0).toInt(), query.value(1).toString()));
    }
    return genres;
}
bool GenreRepository::genreExists(const QString &title) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Genres WHERE GenreTitle = :title");
    query.bindValue(":title", title);
    if(query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}


