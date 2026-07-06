#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H
#include "../common/Response.h"
#include "../common/Notification.h"
#include <QVector>

class NotificationManager {
public:
    NotificationManager();
    Response sendNotification(int userId, NotificationType type, const QString &title, const QString &message, int targetId = -1, int senderId = -1);
    void broadcastNotification(const QVector<int> &userIds, NotificationType type, const QString &title, const QString &message, int targetId = -1, int senderId = -1);
    Response getUserNotifications(int userId);
    Response getUnreadCount(int userId);
    Response markAsRead(int userId, int notificationId);
};
#endif // NOTIFICATIONMANAGER_H
