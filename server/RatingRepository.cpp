#include "DatabaseManager.h"
#include "RatingRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
RatingRepository::RatingRepository(){}
bool RatingRepository::upsertRating(const Rating &rating) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "MERGE INTO Ratings AS target "
        "USING (SELECT :userId AS UserID, :bookId AS BookID) AS src "
        "ON target.UserID = src.UserID AND target.BookID = src.BookID "
        "WHEN MATCHED THEN UPDATE SET Rating = :ratingValue "
        "WHEN NOT MATCHED THEN INSERT (UserID, BookID, Rating) "
        "VALUES (:userId, :bookId, :ratingValue);");
    query.bindValue(":userId", rating.getUserId());
    query.bindValue(":bookId", rating.getBookId());
    query.bindValue(":ratingValue", rating.getRatingValue());
    if(!query.exec()){
        qWarning() << "خطا در ثبت امتیاز:" << query.lastError().text();
        return false;
    }
    return true;
}
Rating* RatingRepository::loadRating(int userId, int bookId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT RatingID, UserID, BookID, Rating FROM Ratings "
        "WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if(!query.exec() || !query.next()){
        return nullptr;
    }
    return new Rating(
        query.value(0).toInt(),
        query.value(1).toInt(),
        query.value(2).toInt(),
        query.value(3).toInt());
}
double RatingRepository::getAverageRating(int bookId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT ISNULL(AVG(CAST(Rating AS FLOAT)), 0) FROM Ratings WHERE BookID = :bookId");
    query.bindValue(":bookId", bookId);
    if(query.exec() && query.next())
        return query.value(0).toDouble();
    return 0.0;
}
int RatingRepository::getRatingCount(int bookId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Ratings WHERE BookID = :bookId");
    query.bindValue(":bookId", bookId);
    if(query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}
bool RatingRepository::deleteRating(int userId, int bookId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Ratings WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if(!query.exec()){
        qWarning() << "خطا در حذف امتیاز:" << query.lastError().text();
        return false;
    }
    return true;
}

