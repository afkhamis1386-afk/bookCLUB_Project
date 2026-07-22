#include "ServerLogRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
ServerLogRepository::ServerLogRepository(){}
bool ServerLogRepository::insertLog(int userId, const QString &requestType, int statusCode, const QString &details){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO ServerLogs (UserID, RequestType, StatusCode, Details) "
        "VALUES (:userId, :requestType, :statusCode, :details)");
    query.bindValue(":userId", userId > 0 ? QVariant(userId) : QVariant());
    query.bindValue(":requestType", requestType);
    query.bindValue(":statusCode", statusCode);
    query.bindValue(":details", details.isEmpty() ? QVariant() : QVariant(details));
    if(!query.exec()){
        qWarning() << "خطا در ثبت لاگ سرور:" << query.lastError().text();
        return false;
    }
    return true;
}
