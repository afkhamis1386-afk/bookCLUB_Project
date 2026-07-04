#ifndef NOTIFICATIONREPOSITORY_H
#define NOTIFICATIONREPOSITORY_H
#include "../common/Notification.h"
#include <QVector>
class NotificationRepository {
public:
    NotificationRepository();
    int insertNotification(const Notification &notification);
    Notification* loadNotificationById(int notificationId);
    QVector<int> getNotificationIdsByUser(int userId);
    QVector<int> getUnreadNotificationIds(int userId);
    bool markAsRead(int notificationId);
    bool markAsUnread(int notificationId);
    int getUnreadCount(int userId);
    bool deleteNotification(int notificationId);
    bool notificationBelongsToUser(int notificationId, int userId);
};
#endif // NOTIFICATIONREPOSITORY_H
