#include <QApplication>
#include "DatabaseManager.h"
#include "ServerApplication.h"
#include "ServerDashboardWindow.h"
#include "../common/Notification.h"
#include "../common/Response.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<Notification>("Notification");
    qRegisterMetaType<Response>("Response");

    DatabaseManager::getInstance()->configure("YOUR_SERVER_NAME", "BookClub");
    ServerApplication server;
    if (!server.startListening(5555)) {
        return 1;
    }
    ServerDashboardWindow dashboard(&server);
    dashboard.show();
    return QApplication::exec();
}