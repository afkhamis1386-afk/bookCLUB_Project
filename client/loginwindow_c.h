#ifndef LOGINWINDOW_C_H
#define LOGINWINDOW_C_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow_c;
}
QT_END_NAMESPACE

class LoginWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow_c(QWidget *parent = nullptr);
    ~LoginWindow_c() override;

private:
    Ui::LoginWindow_c *ui;
};
#endif // LOGINWINDOW_C_H
