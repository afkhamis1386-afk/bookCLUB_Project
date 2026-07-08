#ifndef SERVERDASHBOARDWINDOW_H
#define SERVERDASHBOARDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
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
    private slots:
        void onLogMessage(const QString &message);
        void onClientConnected(qintptr socketDescriptor);
        void onClientDisconnected(qintptr socketDescriptor);
        void refreshStats();
        ~ServerDashboardWindow();
private:
        Ui::ServerDashboardWindow *ui;
        ServerApplication *server;
        ServerMonitorManager *monitor;
        QTimer *refreshTimer;
        QLabel *onlineCountLabel;
        QLabel *healthStatusLabel;
        QLabel *cpuLabel;
        QLabel *ramLabel;
        QListWidget *logListWidget;
        void setupUi();
};

#endif // SERVERDASHBOARDWINDOW_H

