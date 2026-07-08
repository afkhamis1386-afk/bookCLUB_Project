#include "NotificationRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

NotificationRepository::NotificationRepository() {}
int NotificationRepository::insertNotification(const Notification &notification) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Notifications "
        "(UserID, NotificationTypeID, Title, Message, IsRead, CreatedAt, TargetID, SenderID) "
        "OUTPUT INSERTED.NotificationID "
        "VALUES (:userId, :typeId, :title, :message, :isRead, :createdAt, :targetId, :senderId)"
        );
    query.bindValue(":userId", notification.getUserId());
    query.bindValue(":typeId", static_cast<int>(notification.getNotificationType()));
    query.bindValue(":title", notification.getTitle());
    query.bindValue(":message", notification.getMessage());
    query.bindValue(":isRead", notification.isRead());
    query.bindValue(":createdAt", notification.getCreatedAt());
    if (notification.getTargetId() == -1)
        query.bindValue(":targetId", QVariant(QMetaType(QMetaType::Int)));
    else
        query.bindValue(":targetId", notification.getTargetId());

    if (notification.getSenderId() == -1)
        query.bindValue(":senderId", QVariant(QMetaType(QMetaType::Int)));
    else
        query.bindValue(":senderId", notification.getSenderId());

    if (!query.exec() || !query.next()) {
        qWarning() << "خطا در ثبت اعلان:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Notification* NotificationRepository::loadNotificationById(int notificationId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT NotificationID, UserID, NotificationTypeID, Title, Message, IsRead, CreatedAt, "
        "TargetID, SenderID "
        "FROM Notifications WHERE NotificationID = :notificationId"
        );
    query.bindValue(":notificationId", notificationId);
    if (!query.exec() || !query.next()) {
        qWarning() << "اعلان یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    int targetId = query.value(7).isNull() ? -1 : query.value(7).toInt();
    int senderId = query.value(8).isNull() ? -1 : query.value(8).toInt();
    return new Notification(
        query.value(0).toInt(),
        query.value(1).toInt(),
        static_cast<NotificationType>(query.value(2).toInt()),
        query.value(3).toString(),
        query.value(4).toString(),
        query.value(5).toBool(),
        query.value(6).toDateTime(),
        targetId,
        senderId
        );
}
QVector<int> NotificationRepository::getNotificationIdsByUser(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT NotificationID FROM Notifications WHERE UserID = :userId ORDER BY CreatedAt DESC");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> NotificationRepository::getUnreadNotificationIds(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT NotificationID FROM Notifications WHERE UserID = :userId AND IsRead = 0 "
        "ORDER BY CreatedAt DESC"
        );
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool NotificationRepository::markAsRead(int notificationId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Notifications SET IsRead = 1 WHERE NotificationID = :notificationId");
    query.bindValue(":notificationId", notificationId);
    if (!query.exec()) {
        qWarning() << "خطا در علامت گذاری اعلان به عنوان خوانده شده:" << query.lastError().text();
        return false;
    }
    return true;
}
bool NotificationRepository::markAsUnread(int notificationId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Notifications SET IsRead = 0 WHERE NotificationID = :notificationId");
    query.bindValue(":notificationId", notificationId);
    if (!query.exec()) {
        qWarning() << "خطا در علامت گذاری اعلان به عنوان نخوانده شده:" << query.lastError().text();
        return false;
    }
    return true;
}
int NotificationRepository::getUnreadCount(int userId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Notifications WHERE UserID = :userId AND IsRead = 0");
    query.bindValue(":userId", userId);
    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}
bool NotificationRepository::deleteNotification(int notificationId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Notifications WHERE NotificationID = :notificationId");
    query.bindValue(":notificationId", notificationId);
    if (!query.exec()) {
        qWarning() << "خطا در حذف اعلان:" << query.lastError().text();
        return false;
    }
    return true;
}
bool NotificationRepository::notificationBelongsToUser(int notificationId, int userId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Notifications WHERE NotificationID = :notificationId AND UserID = :userId");
    query.bindValue(":notificationId", notificationId);
    query.bindValue(":userId", userId);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}