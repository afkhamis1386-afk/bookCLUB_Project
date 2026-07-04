#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#include <QString>
#include <QDateTime>
#include <QDataStream>
enum class NotificationType {
    NewBookInFavouriteGenre = 1,
    DiscountOnSavedBook     = 2,
    NewSaleForPublisher     = 3,
    NewReviewForPublisher   = 4
};
class Notification {
private:
    int notificationId;
    int userId;
    int targetId;
    int senderId;
    NotificationType notificationType;
    QString title;
    QString message;
    bool read;
    QDateTime createdAt;
public:
    Notification();
    Notification(int userId, NotificationType type, const QString &title, const QString &message, int targetId = -1, int senderId = -1);
    Notification(int notificationId, int userId, NotificationType type, const QString &title, const QString &message, bool read, const QDateTime &createdAt, int targetId = -1, int senderId = -1);
    int getNotificationId() const;
    int getUserId() const;
    int getTargetId() const;
    int getSenderId() const;
    NotificationType getNotificationType() const;
    QString getTitle() const;
    QString getMessage() const;
    bool isRead() const;
    QDateTime getCreatedAt() const;
    void setNotificationId(int id);
    bool setUserId(int id);
    void setTargetId(int id);
    bool setSenderId(int id);
    void markAsRead();
    void markAsUnread();
    bool setTitle(const QString &t);
    bool setMessage(const QString &m);
    QString getTypeDisplayName() const;
    QString getRelativeTime() const;
    friend QDataStream &operator<<(QDataStream &out, const Notification &n);
    friend QDataStream &operator>>(QDataStream &in, Notification &n);
};
#endif // NOTIFICATION_H
