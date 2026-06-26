#include "loginwindow_c.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow_c w;
    w.show();
    return QApplication::exec();
}
