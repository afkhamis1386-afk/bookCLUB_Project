#ifndef LOGINWINDOW_S_H
#define LOGINWINDOW_S_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow_s;
}
QT_END_NAMESPACE

class LoginWindow_s : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow_s(QWidget *parent = nullptr);
    ~LoginWindow_s() override;

private:
    Ui::LoginWindow_s *ui;
};
#endif // LOGINWINDOW_S_H
