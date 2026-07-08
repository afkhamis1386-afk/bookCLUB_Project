#include "TimedDiscountRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
TimedDiscountRepository::TimedDiscountRepository(){}
int TimedDiscountRepository::insertTimedDiscount(const TimedDiscount &discount){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO TimedDiscount (BookID, DiscountPercent, StartDate, EndDate) "
        "OUTPUT INSERTED.DiscountID "
        "VALUES (:bookId, :percent, :startDate, :endDate)");
    query.bindValue(":bookId", discount.getBookId());
    query.bindValue(":percent", discount.getDiscountPercent());
    query.bindValue(":startDate", discount.getStartDate());
    query.bindValue(":endDate", discount.getEndDate());
    if(!query.exec() || !query.next()){
        qWarning() << "خطا در ثبت تخفیف زمان دار:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
TimedDiscount* TimedDiscountRepository::loadDiscountById(int discountId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT DiscountID, BookID, DiscountPercent, StartDate, EndDate "
        "FROM TimedDiscount WHERE DiscountID = :discountId");
    query.bindValue(":discountId", discountId);
    if(!query.exec() || !query.next()){
        qWarning() << "تخفیف زمان دار یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    return new TimedDiscount(
        query.value(0).toInt(),
        query.value(1).toInt(),
        query.value(2).toDouble(),
        query.value(3).toDateTime(),
        query.value(4).toDateTime()
        );
}
TimedDiscount* TimedDiscountRepository::getActiveDiscountForBook(int bookId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT TOP 1 DiscountID, BookID, DiscountPercent, StartDate, EndDate "
        "FROM TimedDiscount "
        "WHERE BookID = :bookId AND StartDate <= GETDATE() AND EndDate > GETDATE() "
        "ORDER BY StartDate DESC");
    query.bindValue(":bookId", bookId);
    if(!query.exec() || !query.next()){
        return nullptr;
    }
    return new TimedDiscount(
        query.value(0).toInt(),
        query.value(1).toInt(),
        query.value(2).toDouble(),
        query.value(3).toDateTime(),
        query.value(4).toDateTime());
}
QVector<int> TimedDiscountRepository::getAllDiscountIdsForBook(int bookId){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT DiscountID FROM TimedDiscount WHERE BookID = :bookId ORDER BY StartDate DESC");
    query.bindValue(":bookId", bookId);
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool TimedDiscountRepository::updateDates(int discountId, const QDateTime &startDate, const QDateTime &endDate){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "UPDATE TimedDiscount SET StartDate = :startDate, EndDate = :endDate "
        "WHERE DiscountID = :discountId");
    query.bindValue(":startDate", startDate);
    query.bindValue(":endDate", endDate);
    query.bindValue(":discountId", discountId);
    if(!query.exec()){
        qWarning() << "خطا در ویرایش تاریخ تخفیف:" << query.lastError().text();
        return false;
    }
    return true;
}
bool TimedDiscountRepository::deleteDiscount(int discountId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM TimedDiscount WHERE DiscountID = :discountId");
    query.bindValue(":discountId", discountId);
    if(!query.exec()){
        qWarning() << "خطا در حذف تخفیف زمان دار:" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<int> TimedDiscountRepository::getExpiredDiscountIds(){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT DiscountID FROM TimedDiscount WHERE EndDate <= GETDATE()");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}