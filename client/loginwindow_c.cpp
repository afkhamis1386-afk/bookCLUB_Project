#include "loginwindow_c.h"
#include "ui_loginwindow_c.h"

LoginWindow_c::LoginWindow_c(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow_c)
{
    ui->setupUi(this);
}

LoginWindow_c::~LoginWindow_c()
{
    delete ui;
}
