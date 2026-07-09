#include "NotificationManager.h"
#include "NotificationRepository.h"
#include "UserRepository.h"
#include "PublisherRepository.h"
#include "AdminRepository.h"
#include "ClientRegistry.h"
#include "../common/normaluser.h"
#include "../common/publisher.h"
#include "../common/Admin.h"
#include <memory>

NotificationManager::NotificationManager() {}
Response NotificationManager::sendNotification(int userId, NotificationType type, const QString &title, const QString &message, int targetId, int senderId) {
    Notification notification(userId, type, title, message, targetId, senderId);
    NotificationRepository notifRepo;
    int newId = notifRepo.insertNotification(notification);
    if (newId == -1) {
        return Response(ResponseStatus::Error, "خطا در ارسال اعلان");
    }
    notification.setNotificationId(newId);
    ClientRegistry::getInstance()->pushToUser(userId, notification);
    QVariantMap data;
    data["notificationId"] = newId;
    return Response(ResponseStatus::Success, "اعلان ارسال شد", data);
}
void NotificationManager::broadcastNotification(const QVector<int> &userIds, NotificationType type, const QString &title, const QString &message, int targetId, int senderId) {
    NotificationRepository notifRepo;
    for (int userId : userIds) {
        Notification notification(userId, type, title, message, targetId, senderId);
        int newId = notifRepo.insertNotification(notification);
        if (newId != -1) {
            notification.setNotificationId(newId);
            ClientRegistry::getInstance()->pushToUser(userId, notification);
        }
    }
}
Response NotificationManager::getUserNotifications(int userId) {
    NotificationRepository notifRepo;
    QVector<int> notifIds = notifRepo.getNotificationIdsByUser(userId);
    UserRepository userRepo;
    QVariantList notifList;
    for (int notifId : qAsConst(notifIds)) {
        std::unique_ptr<Notification> notif(notifRepo.loadNotificationById(notifId));
        if (!notif) continue;
        QVariantMap notifData;
        notifData["notificationId"] = notif->getNotificationId();
        notifData["type"] = static_cast<int>(notif->getNotificationType());
        notifData["typeDisplayName"] = notif->getTypeDisplayName();
        notifData["title"] = notif->getTitle();
        notifData["message"] = notif->getMessage();
        notifData["isRead"] = notif->isRead();
        notifData["createdAt"] = notif->getCreatedAt();
        notifData["relativeTime"] = notif->getRelativeTime();
        notifData["targetId"] = notif->getTargetId();
        QString senderDisplayName = "سیستم";
        int senderId = notif->getSenderId();
        if (senderId != -1) {
            UserRole senderRole;
            if (userRepo.findRoleById(senderId, senderRole)) {
                if (senderRole == UserRole::NormalUser) {
                    std::unique_ptr<NormalUser> senderUser(userRepo.loadNormalUserById(senderId));
                    if (senderUser)
                        senderDisplayName = senderUser->getUsername();
                }
                else if (senderRole == UserRole::Publisher) {
                    PublisherRepository publisherRepo;
                    std::unique_ptr<Publisher> senderPublisher(publisherRepo.loadPublisherById(senderId));
                    if (senderPublisher)
                        senderDisplayName = senderPublisher->getUsername();
                }
                else {
                    AdminRepository adminRepo;
                    std::unique_ptr<Admin> senderAdmin(adminRepo.loadAdminById(senderId));
                    if (senderAdmin)
                        senderDisplayName = senderAdmin->getUsername();
                }
            }
        }
        notifData["senderDisplayName"] = senderDisplayName;
        notifList.append(notifData);
    }
    QVariantMap data;
    data["notifications"] = notifList;
    return Response(ResponseStatus::Success, "اعلان ها بازیابی شدند", data);
}
Response NotificationManager::getUnreadCount(int userId) {
    NotificationRepository notifRepo;
    int count = notifRepo.getUnreadCount(userId);
    QVariantMap data;
    data["unreadCount"] = count;
    return Response(ResponseStatus::Success, "تعداد اعلان های نخوانده بازیابی شد", data);
}
Response NotificationManager::markAsRead(int userId, int notificationId) {
    NotificationRepository notifRepo;
    if (!notifRepo.notificationBelongsToUser(notificationId, userId)) {
        return Response(ResponseStatus::Unauthorized, "این اعلان متعلق به شما نیست");
    }
    if (!notifRepo.markAsRead(notificationId)) {
        return Response(ResponseStatus::Error, "خطا در علامت گذاری اعلان");
    }
    return Response(ResponseStatus::Success, "اعلان به عنوان خوانده شده علامت گذاری شد");
}