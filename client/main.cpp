#include <QApplication>
#include "loginwindow_c.h"
#include "NetworkManager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkManager networkManager;
    networkManager.connectToServer("127.0.0.1", 5555);
    LoginWindow_c loginWindow(&networkManager);
    loginWindow.show();
    return a.exec();
}