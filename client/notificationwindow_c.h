#ifndef NOTIFICATIONWINDOW_C_H
#define NOTIFICATIONWINDOW_C_H

#include <QMainWindow>
#include "NetworkManager.h"
#include "NotificationController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class NotificationWindow_c;
}
QT_END_NAMESPACE

class NotificationWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit NotificationWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~NotificationWindow_c() override;

signals:
    void backRequested();

private slots:
    void onNotificationsLoaded(const QVariantList &notifications);
    void onNotificationsLoadFailed(const QString &message);
    void onNotificationItemClicked(QListWidgetItem *item);
    void onNotificationMarkedRead(const QString &message);
    void onNotificationMarkReadFailed(const QString &message);
    void onNewNotificationArrived(const QVariantMap &notificationData);
    void onRefreshButtonClicked();
    void onBackButtonClicked();

private:
    Ui::NotificationWindow_c *ui;
    NetworkManager *networkManager;
    NotificationController *notificationController;
    void addNotificationToList(int notificationId, const QString &title, const QString &message, const QString &relativeTime, bool isRead, bool prepend);
    QListWidgetItem* findItemByNotificationId(int notificationId) const;
};

#endif // NOTIFICATIONWINDOW_C_H
