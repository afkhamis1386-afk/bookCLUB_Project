#include <QApplication>
#include "DatabaseManager.h"
#include "ServerApplication.h"
#include "ServerDashboardWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseManager::getInstance()->configure("YOUR_SERVER_NAME", "BookClub");

    ServerApplication server;

    if (!server.startListening(5555)) {
        return 1;
    }

    ServerDashboardWindow dashboard(&server);
    dashboard.show();

    return QApplication::exec();
}