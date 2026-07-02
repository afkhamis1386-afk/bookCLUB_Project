#include "Notification.h"
Notification::Notification()
    : notificationId(-1),
    userId(-1),
    targetId(-1),
    senderId(-1),
    senderName("System"),
    notificationType(NotificationType::NewBookInFavouriteGenre),
    read(false),
    createdAt(QDateTime::currentDateTime()) {}
Notification::Notification(int userId, NotificationType type, const QString &title, const QString &message, int targetId, int senderId, const QString &senderName)
    : notificationId(-1),
    userId(userId > 0 ? userId : -1),
    targetId(targetId),
    senderId(senderId),
    notificationType(type),
    read(false),
    createdAt(QDateTime::currentDateTime()) {
    if (!setTitle(title)) {
        this->title = "System Notification";
    }
    if (!setMessage(message)) {
        this->message = "No Details Provided.";
    }
    if (!setSenderName(senderName)) {
        this->senderName = "System";
    }
}
Notification::Notification(int notificationId, int userId, NotificationType type, const QString &title, const QString &message, bool read, const QDateTime &createdAt, int targetId, int senderId, const QString &senderName)
    : notificationId(notificationId),
    userId(userId > 0 ? userId : -1),
    targetId(targetId),
    senderId(senderId),
    notificationType(type),
    read(read),
    createdAt(createdAt) {
    if (!setTitle(title)) {
        this->title = "System Notification";
    }
    if (!setMessage(message)) {
        this->message = "No Details Provided.";
    }
    if (!setSenderName(senderName)) {
        this->senderName = "System";
    }
}
int Notification::getNotificationId() const { return notificationId; }
int Notification::getUserId() const { return userId; }
int Notification::getTargetId() const { return targetId; }
int Notification::getSenderId() const { return senderId; }
QString Notification::getSenderName() const { return senderName; }
NotificationType Notification::getNotificationType() const { return notificationType; }
QString Notification::getTitle() const { return title; }
QString Notification::getMessage() const { return message; }
bool Notification::isRead() const { return read; }
QDateTime Notification::getCreatedAt() const { return createdAt; }
void Notification::setNotificationId(int id) { notificationId = id; }
bool Notification::setUserId(int id) {
    if (id <= 0)
        return false;
    userId = id;
    return true;
}
void Notification::setTargetId(int id) { targetId = id; }
bool Notification::setSenderId(int id) {
    if (id <= 0)
        return false;
    senderId = id;
    return true;
}
bool Notification::setSenderName(const QString &name) {
    if (name.trimmed().isEmpty() || name.length() > 100)
        return false;
    senderName = name.trimmed();
    return true;
}
void Notification::markAsRead() { read = true;}
void Notification::markAsUnread() { read = false; }
bool Notification::setTitle(const QString &t) {
    if (t.trimmed().isEmpty() || t.length() > 200)
        return false;
    title = t.trimmed();
    return true;
}
bool Notification::setMessage(const QString &m) {
    if (m.trimmed().isEmpty() || m.length() > 500)
        return false;
    message = m.trimmed();
    return true;
}
QString Notification::getTypeDisplayName() const {
    switch (notificationType) {
    case NotificationType::NewBookInFavouriteGenre:
        return "New Book in Favorite Genre";
    case NotificationType::DiscountOnSavedBook:
        return "Discount on Saved Book";
    case NotificationType::NewSaleForPublisher:
        return "New Sale Notification";
    case NotificationType::NewReviewForPublisher:
        return "New Review or Rating";
    default:
        return "Notification";
    }
}
QString Notification::getRelativeTime() const {
    qint64 seconds = createdAt.secsTo(QDateTime::currentDateTime());
    if (seconds < 60)
        return "Just now";
    if (seconds < 3600)
        return QString::number(seconds / 60) + " minutes ago";
    if (seconds < 86400)
        return QString::number(seconds / 3600) + " hours ago";
    if (seconds < 604800)
        return QString::number(seconds / 86400) + " days ago";
    return createdAt.toString("yyyy/MM/dd");
}
QDataStream &operator<<(QDataStream &out, const Notification &n) {
    out << n.notificationId
        << n.userId
        << n.targetId
        << n.senderId
        << n.senderName
        << static_cast<int>(n.notificationType)
        << n.title
        << n.message
        << n.read
        << n.createdAt;
    return out;
}
QDataStream &operator>>(QDataStream &in, Notification &n) {
    int typeInt;
    in >> n.notificationId
        >> n.userId
        >> n.targetId
        >> n.senderId
        >> n.senderName
        >> typeInt
        >> n.title
        >> n.message
        >> n.read
        >> n.createdAt;
    if (typeInt >= 1 && typeInt <= 4)
        n.notificationType = static_cast<NotificationType>(typeInt);
    else
        n.notificationType = NotificationType::NewBookInFavouriteGenre;
    return in;
}
