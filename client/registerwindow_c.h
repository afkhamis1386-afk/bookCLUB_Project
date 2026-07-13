#ifndef REGISTERWINDOW_C_H
#define REGISTERWINDOW_C_H
#include <QMainWindow>
#include "NetworkManager.h"
#include "LoginController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class RegisterWindow_c;
}
QT_END_NAMESPACE

class RegisterWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegisterWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~RegisterWindow_c() override;

signals:
    void backToLoginRequested();

private slots:
    void onRoleToggled();
    void onRegisterButtonClicked();
    void onBackToLoginButtonClicked();
    void onRegistrationSucceeded(const QString &message);
    void onRegistrationFailed(const QString &message);
    void onValidationError(const QString &message);

private:
    Ui::RegisterWindow_c *ui;
    NetworkManager *networkManager;
    LoginController *loginController;
};

#endif // REGISTERWINDOW_C_H