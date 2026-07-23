#ifndef LOGINWINDOW_C_H
#define LOGINWINDOW_C_H
#include <QMainWindow>
#include "NetworkManager.h"
#include "LoginController.h"
#include "registerwindow_c.h"
#include "forgotpasswordwindow_c.h"
#include "GenreSelectionWindow_c.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow_c;
}
QT_END_NAMESPACE

class LoginWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~LoginWindow_c() override;

private slots:
    void onLoginButtonClicked();
    void onGoToRegisterButtonClicked();
    void onForgotPasswordButtonClicked();
    void onLoginSucceeded(UserRole role);
    void onLoginFailed(const QString &message);
    void onValidationError(const QString &message);

private:
    Ui::LoginWindow_c *ui;
    NetworkManager *networkManager;
    LoginController *loginController;
    RegisterWindow_c *registerWindow = nullptr;
    ForgotPasswordWindow_c *forgotPasswordWindow = nullptr;
    GenreSelectionWindow_c *genreSelectionWindow = nullptr;
    bool genreSelectionConfirmed = false;
};

#endif // LOGINWINDOW_C_H