#include "ServerDashboardWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QWidget>
#include <QDateTime>

ServerDashboardWindow::ServerDashboardWindow(ServerApplication *server, QWidget *parent)
    : QMainWindow(parent), server(server), monitor(new ServerMonitorManager(this))
{
    setupUi();

    connect(server, &ServerApplication::logMessage, this, &ServerDashboardWindow::onLogMessage);
    connect(server, &ServerApplication::clientConnected, this, &ServerDashboardWindow::onClientConnected);
    connect(server, &ServerApplication::clientDisconnected, this, &ServerDashboardWindow::onClientDisconnected);

    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &ServerDashboardWindow::refreshStats);
    refreshTimer->start(2000);

    refreshStats();
}

ServerDashboardWindow::~ServerDashboardWindow()
{}

void ServerDashboardWindow::setupUi() {
    setWindowTitle("BookClub Server Dashboard");
    resize(700, 500);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QGroupBox *statsGroup = new QGroupBox("وضعیت سرور", centralWidget);
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    onlineCountLabel = new QLabel("کاربران آنلاین: 0", statsGroup);
    healthStatusLabel = new QLabel("وضعیت: Healthy", statsGroup);
    cpuLabel = new QLabel("CPU: 0%", statsGroup);
    ramLabel = new QLabel("RAM: 0 MB", statsGroup);

    statsLayout->addWidget(onlineCountLabel);
    statsLayout->addWidget(healthStatusLabel);
    statsLayout->addWidget(cpuLabel);
    statsLayout->addWidget(ramLabel);

    QGroupBox *logGroup = new QGroupBox("لاگ درخواست ها و رویدادها", centralWidget);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logListWidget = new QListWidget(logGroup);
    logLayout->addWidget(logListWidget);

    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(logGroup);

    setCentralWidget(centralWidget);
}

void ServerDashboardWindow::onLogMessage(const QString &message) {
    logListWidget->addItem(QString("[%1] %2")
                               .arg(QDateTime::currentDateTime().toString("hh:mm:ss"), message));
    logListWidget->scrollToBottom();
}

void ServerDashboardWindow::onClientConnected(qintptr socketDescriptor) {
    monitor->setOnlineClientCount(server->getOnlineClientCount());
    monitor->addRequestLog(QString("اتصال جدید (Socket: %1)").arg(socketDescriptor), 0);
}

void ServerDashboardWindow::onClientDisconnected(qintptr socketDescriptor) {
    monitor->setOnlineClientCount(server->getOnlineClientCount());
    monitor->addRequestLog(QString("قطع اتصال (Socket: %1)").arg(socketDescriptor), 0);
}

void ServerDashboardWindow::refreshStats() {
    monitor->setOnlineClientCount(server->getOnlineClientCount());

    onlineCountLabel->setText(QString("کاربران آنلاین: %1").arg(monitor->getOnlineClientCount()));
    healthStatusLabel->setText(QString("وضعیت: %1").arg(monitor->getHealthStatusText()));
    cpuLabel->setText(QString("CPU: %1%").arg(monitor->getEstimatedCpuUsage(), 0, 'f', 1));
    ramLabel->setText(QString("RAM: %1 MB").arg(monitor->getEstimatedRamUsageMB(), 0, 'f', 1));
}