#include "notificationwindow_c.h"
#include "ui_notificationwindow_c.h"
#include <QListWidgetItem>
#include <QFont>

NotificationWindow_c::NotificationWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NotificationWindow_c)
    , networkManager(networkManager)
    , notificationController(new NotificationController(networkManager, this))
{
    ui->setupUi(this);
    connect(ui->refreshButton, &QPushButton::clicked, this, &NotificationWindow_c::onRefreshButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &NotificationWindow_c::onBackButtonClicked);
    connect(ui->notificationsListWidget, &QListWidget::itemClicked, this, &NotificationWindow_c::onNotificationItemClicked);
    connect(notificationController, &NotificationController::notificationsLoaded, this, &NotificationWindow_c::onNotificationsLoaded);
    connect(notificationController, &NotificationController::notificationsLoadFailed, this, &NotificationWindow_c::onNotificationsLoadFailed);
    connect(notificationController, &NotificationController::notificationMarkedRead, this, &NotificationWindow_c::onNotificationMarkedRead);
    connect(notificationController, &NotificationController::notificationMarkReadFailed, this, &NotificationWindow_c::onNotificationMarkReadFailed);
    connect(notificationController, &NotificationController::newNotificationArrived, this, &NotificationWindow_c::onNewNotificationArrived);
    notificationController->refreshNotifications();
}
NotificationWindow_c::~NotificationWindow_c()
{
    delete ui;
}
void NotificationWindow_c::addNotificationToList(int notificationId, const QString &title, const QString &message, const QString &relativeTime, bool isRead, bool prepend)
{
    QString displayText = QString("%1\n%2\n%3").arg(title, message, relativeTime);
    QListWidgetItem *item = new QListWidgetItem(displayText);
    item->setData(Qt::UserRole, notificationId);
    item->setData(Qt::UserRole + 1, isRead);
    QFont font = item->font();
    font.setBold(!isRead);
    item->setFont(font);
    item->setForeground(isRead ? QColor("#7f8c8d") : QColor("#2c3e50"));

    if (prepend)
        ui->notificationsListWidget->insertItem(0, item);
    else
        ui->notificationsListWidget->addItem(item);
}

QListWidgetItem* NotificationWindow_c::findItemByNotificationId(int notificationId) const
{
    for (int i = 0; i < ui->notificationsListWidget->count(); ++i) {
        QListWidgetItem *item = ui->notificationsListWidget->item(i);
        if (item->data(Qt::UserRole).toInt() == notificationId)
            return item;
    }
    return nullptr;
}
void NotificationWindow_c::onNotificationsLoaded(const QVariantList &notifications)
{
    ui->statusLabel->clear();
    ui->notificationsListWidget->clear();
    for (const QVariant &v : notifications) {
        QVariantMap notif = v.toMap();
        addNotificationToList(
            notif.value("notificationId").toInt(),
            notif.value("title").toString(),
            notif.value("message").toString(),
            notif.value("relativeTime").toString(),
            notif.value("isRead").toBool(),
            false
            );
    }
    if (notifications.isEmpty())
        ui->statusLabel->setText("هیچ اعلانی وجود ندارد");
}
void NotificationWindow_c::onNotificationsLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}
void NotificationWindow_c::onNotificationItemClicked(QListWidgetItem *item)
{
    bool alreadyRead = item->data(Qt::UserRole + 1).toBool();
    if (alreadyRead) return;
    int notificationId = item->data(Qt::UserRole).toInt();
    notificationController->markAsRead(notificationId);
}
void NotificationWindow_c::onNotificationMarkedRead(const QString &message)
{
    ui->statusLabel->setText(message);
    QListWidgetItem *item = ui->notificationsListWidget->currentItem();
    if (!item) return;
    item->setData(Qt::UserRole + 1, true);
    QFont font = item->font();
    font.setBold(false);
    item->setFont(font);
    item->setForeground(QColor("#7f8c8d"));
}

void NotificationWindow_c::onNotificationMarkReadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}
void NotificationWindow_c::onNewNotificationArrived(const QVariantMap &notificationData)
{
    addNotificationToList(
        notificationData.value("notificationId").toInt(),
        notificationData.value("title").toString(),
        notificationData.value("message").toString(),
        "همین الان",
        false,
        true
        );
}
void NotificationWindow_c::onRefreshButtonClicked()
{
    notificationController->refreshNotifications();
}
void NotificationWindow_c::onBackButtonClicked()
{
    emit backRequested();
}