#include "DatabaseManager.h"
#include "ReviewRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
ReviewRepository::ReviewRepository(){}
int ReviewRepository::insertReview(const Review &review){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Reviews (UserID, BookID, CommentText, ParentID, IsDeleted, ReviewDate) "
        "OUTPUT INSERTED.ReviewID "
        "VALUES (:userId, :bookId, :comment, :parentId, :isDeleted, :reviewDate)");
    query.bindValue(":userId", review.getUserId());
    query.bindValue(":bookId", review.getBookId());
    query.bindValue(":comment", review.getCommentText());
    if(review.getParentId() == -1)
        query.bindValue(":parentId", QVariant(QMetaType(QMetaType::Int)));
    else
        query.bindValue(":parentId", review.getParentId());
    query.bindValue(":isDeleted", review.getIsDeleted());
    query.bindValue(":reviewDate", review.getReviewDate());
    if(!query.exec() || !query.next()){
        qWarning() << "خطا در ثبت نظر:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Review* ReviewRepository::loadReviewById(int reviewId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT ReviewID, UserID, BookID, CommentText, ParentID, IsDeleted, ReviewDate "
        "FROM Reviews WHERE ReviewID = :reviewId"
        );
    query.bindValue(":reviewId", reviewId);
    if(!query.exec() || !query.next()){
        qWarning() << "نظر یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    int parentId = query.value(4).isNull() ? -1 : query.value(4).toInt();
    return new Review(
        query.value(0).toInt(),
        query.value(1).toInt(),
        query.value(2).toInt(),
        query.value(3).toString(),
        parentId,
        query.value(5).toBool(),
        query.value(6).toDateTime());
}
bool ReviewRepository::updateCommentText(int reviewId, const QString &newComment){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Reviews SET CommentText = :comment WHERE ReviewID = :reviewId");
    query.bindValue(":comment", newComment);
    query.bindValue(":reviewId", reviewId);
    if(!query.exec()){
        qWarning() << "خطا در ویرایش نظر:" << query.lastError().text();
        return false;
    }
    return true;
}
bool ReviewRepository::setDeletedStatus(int reviewId, bool isDeleted){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Reviews SET IsDeleted = :isDeleted WHERE ReviewID = :reviewId");
    query.bindValue(":isDeleted", isDeleted);
    query.bindValue(":reviewId", reviewId);
    if(!query.exec()){
        qWarning() << "خطا در حذف نظر:" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<int> ReviewRepository::getReviewIdsByBook(int bookId){
    QVector<int> ids;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT ReviewID FROM Reviews WHERE BookID = :bookId AND IsDeleted = 0 "
        "ORDER BY ReviewDate DESC"
        );
    query.bindValue(":bookId", bookId);

    if(query.exec()){
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> ReviewRepository::getRepliesOf(int reviewId){
    QVector<int> ids;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT ReviewID FROM Reviews WHERE ParentID = :parentId AND IsDeleted = 0 "
        "ORDER BY ReviewDate ASC");
    query.bindValue(":parentId", reviewId);
    if(query.exec()){
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> ReviewRepository::getReviewIdsByUser(int userId){
    QVector<int> ids;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT ReviewID FROM Reviews WHERE UserID = :userId AND IsDeleted = 0");
    query.bindValue(":userId", userId);
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> ReviewRepository::getAllReviewIds(){
    QVector<int> ids;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT ReviewID FROM Reviews ORDER BY ReviewDate DESC");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool ReviewRepository::reviewBelongsToUser(int reviewId, int userId){
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Reviews WHERE ReviewID = :reviewId AND UserID = :userId");
    query.bindValue(":reviewId", reviewId);
    query.bindValue(":userId", userId);

    if(query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}

