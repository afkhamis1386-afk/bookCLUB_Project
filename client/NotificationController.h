#ifndef NOTIFICATIONCONTROLLER_H
#define NOTIFICATIONCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "NetworkManager.h"
class NotificationController : public QObject {
    Q_OBJECT

public:
    explicit NotificationController(NetworkManager *networkManager, QObject *parent = nullptr);
    void refreshNotifications();
    void markAsRead(int notificationId);
    void refreshUnreadCount();
signals:
    void notificationsLoaded(const QVariantList &notifications);
    void notificationsLoadFailed(const QString &message);
    void notificationMarkedRead(const QString &message);
    void notificationMarkReadFailed(const QString &message);
    void unreadCountLoaded(int unreadCount);
    void unreadCountLoadFailed(const QString &message);
    void newNotificationArrived(const QVariantMap &notificationData);
    void validationError(const QString &message);
private slots:
    void onResponseReceived(RequestType type, const Response &response);
    void onPushNotificationReceived(const QVariantMap &data);
private:
    NetworkManager *networkManager;
};
#endif // NOTIFICATIONCONTROLLER_H
