#ifndef PUBLISHERDASHBOARDCONTROLLER_H
#define PUBLISHERDASHBOARDCONTROLLER_H

#include <QObject>
#include <QVariantMap>
#include "NetworkManager.h"
class PublisherDashboardController : public QObject {
    Q_OBJECT
public:
    explicit PublisherDashboardController(NetworkManager *networkManager, QObject *parent = nullptr);
    void refreshDashboard();
signals:
    void dashboardLoaded(const QVariantMap &dashboardData);
    void dashboardLoadFailed(const QString &message);
private slots:
    void onResponseReceived(RequestType type, const Response &response);
private:
    NetworkManager *networkManager;
};

#endif // PUBLISHERDASHBOARDCONTROLLER_H
