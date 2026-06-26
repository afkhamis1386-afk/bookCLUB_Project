#include "loginwindow_s.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow_s w;
    w.show();
    return QApplication::exec();
}
