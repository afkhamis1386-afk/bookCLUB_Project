#ifndef SERVERDASHBOARDWINDOW_H
#define SERVERDASHBOARDWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ServerApplication.h"
#include "ServerMonitorManager.h"

namespace Ui {
class ServerDashboardWindow;
}
class ServerDashboardWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ServerDashboardWindow(ServerApplication *server, QWidget *parent = nullptr);
    ~ServerDashboardWindow();
private slots:
    void onLogMessage(const QString &message);
    void onClientConnected(qintptr socketDescriptor);
    void onClientDisconnected(qintptr socketDescriptor);
    void onRequestProcessed(const QString &requestType, int statusCode);
    void refreshStats();
private:
    Ui::ServerDashboardWindow *ui;
    ServerApplication *server;
    ServerMonitorManager *monitor;
    QTimer *refreshTimer;
};

#endif // SERVERDASHBOARDWINDOW_H
