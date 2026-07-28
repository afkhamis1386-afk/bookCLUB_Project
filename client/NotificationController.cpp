#include "NotificationController.h"

NotificationController::NotificationController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager) {
    connect(networkManager, &NetworkManager::responseReceived, this, &NotificationController::onResponseReceived);
    connect(networkManager, &NetworkManager::pushNotificationReceived, this, &NotificationController::onPushNotificationReceived);
}
void NotificationController::refreshNotifications() {
    if (!networkManager->isConnected()) {
        emit notificationsLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getNotifications();
}
void NotificationController::markAsRead(int notificationId) {
    if (notificationId <= 0) {
        emit validationError("شناسه اعلان نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit notificationMarkReadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->markNotificationRead(notificationId);
}
void NotificationController::refreshUnreadCount() {
    if (!networkManager->isConnected()) {
        emit unreadCountLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getUnreadNotificationCount();
}
void NotificationController::onResponseReceived(RequestType type, const Response &response) {
    switch (type) {
    case RequestType::GetNotifications:
        if (response.isSuccess())
            emit notificationsLoaded(response.getData().value("notifications").toList());
        else
            emit notificationsLoadFailed(response.getMessage());
        break;
    case RequestType::MarkNotificationRead:
        if (response.isSuccess())
            emit notificationMarkedRead(response.getMessage());
        else
            emit notificationMarkReadFailed(response.getMessage());
        break;
    case RequestType::GetUnreadNotificationCount:
        if (response.isSuccess())
            emit unreadCountLoaded(response.getData().value("unreadCount").toInt());
        else
            emit unreadCountLoadFailed(response.getMessage());
        break;
    default:
        break;
    }
}
void NotificationController::onPushNotificationReceived(const QVariantMap &data) {
    emit newNotificationArrived(data);
}