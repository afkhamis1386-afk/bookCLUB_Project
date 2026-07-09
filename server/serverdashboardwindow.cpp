#include "ServerDashboardWindow.h"
#include "ui_serverdashboardwindow.h"
#include <QDateTime>

ServerDashboardWindow::ServerDashboardWindow(ServerApplication *server, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ServerDashboardWindow),
    server(server), monitor(new ServerMonitorManager(this)) {
    ui->setupUi(this);
    setWindowTitle("BookClub Server Dashboard");
    connect(server, &ServerApplication::logMessage, this, &ServerDashboardWindow::onLogMessage);
    connect(server, &ServerApplication::clientConnected, this, &ServerDashboardWindow::onClientConnected);
    connect(server, &ServerApplication::clientDisconnected, this, &ServerDashboardWindow::onClientDisconnected);
    connect(server, &ServerApplication::requestProcessed, this, &ServerDashboardWindow::onRequestProcessed);
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &ServerDashboardWindow::refreshStats);
    refreshTimer->start(2000);
    refreshStats();
}
ServerDashboardWindow::~ServerDashboardWindow() {
    delete ui;
}
void ServerDashboardWindow::onLogMessage(const QString &message) {
    ui->logListWidget->addItem(QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss"), message));
    ui->logListWidget->scrollToBottom();
}
void ServerDashboardWindow::onClientConnected(qintptr socketDescriptor) {
    monitor->setOnlineClientCount(server->getOnlineClientCount());
    monitor->addRequestLog(QString("اتصال جدید (Socket: %1)").arg(socketDescriptor), 0);
}
void ServerDashboardWindow::onClientDisconnected(qintptr socketDescriptor) {
    monitor->setOnlineClientCount(server->getOnlineClientCount());
    monitor->addRequestLog(QString("قطع اتصال (Socket: %1)").arg(socketDescriptor), 0);
}
void ServerDashboardWindow::onRequestProcessed(const QString &requestType, int statusCode) {
    monitor->addRequestLog(QString("Request: %1").arg(requestType), statusCode);
    ui->logListWidget->addItem(QString("[%1] Request Type=%2 → Status=%3").arg(QDateTime::currentDateTime().toString("hh:mm:ss"), requestType)
                                   .arg(statusCode));
    ui->logListWidget->scrollToBottom();
}
void ServerDashboardWindow::refreshStats() {
    monitor->setOnlineClientCount(server->getOnlineClientCount());
    ui->onlineCountLabel->setText(QString("کاربران آنلاین: %1").arg(monitor->getOnlineClientCount()));
    ui->healthStatusLabel->setText(QString("وضعیت: %1").arg(monitor->getHealthStatusText()));
    ui->cpuLabel->setText(QString("CPU: %1%").arg(monitor->getEstimatedCpuUsage(), 0, 'f', 1));
    ui->ramLabel->setText(QString("RAM: %1 MB").arg(monitor->getEstimatedRamUsageMB(), 0, 'f', 1));
}