#ifndef FORGOTPASSWORDWINDOW_C_H
#define FORGOTPASSWORDWINDOW_C_H

#include <QMainWindow>
#include "NetworkManager.h"
#include "LoginController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ForgotPasswordWindow_c;
}
QT_END_NAMESPACE

class ForgotPasswordWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit ForgotPasswordWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~ForgotPasswordWindow_c() override;

signals:
    void backToLoginRequested();

private slots:
    void onRecoverButtonClicked();
    void onBackToLoginButtonClicked();
    void onRecoverySucceeded(const QString &message);
    void onRecoveryFailed(const QString &message);
    void onValidationError(const QString &message);

private:
    Ui::ForgotPasswordWindow_c *ui;
    NetworkManager *networkManager;
    LoginController *loginController;
};

#endif // FORGOTPASSWORDWINDOW_C_H