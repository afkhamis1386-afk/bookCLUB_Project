#include "loginwindow_s.h"
#include "ui_loginwindow_s.h"

LoginWindow_s::LoginWindow_s(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow_s)
{
    ui->setupUi(this);
}

LoginWindow_s::~LoginWindow_s()
{
    delete ui;
}
