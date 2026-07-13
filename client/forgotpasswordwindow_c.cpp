#include "forgotpasswordwindow_c.h"
#include "ui_forgotpasswordwindow_c.h"
#include <QMessageBox>
ForgotPasswordWindow_c::ForgotPasswordWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ForgotPasswordWindow_c)
    , networkManager(networkManager)
    , loginController(new LoginController(networkManager, this))
{
    ui->setupUi(this);
    ui->newPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmNewPasswordLineEdit->setEchoMode(QLineEdit::Password);

    connect(ui->recoverButton, &QPushButton::clicked, this, &ForgotPasswordWindow_c::onRecoverButtonClicked);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &ForgotPasswordWindow_c::onBackToLoginButtonClicked);

    connect(loginController, &LoginController::recoverySucceeded, this, &ForgotPasswordWindow_c::onRecoverySucceeded);
    connect(loginController, &LoginController::recoveryFailed, this, &ForgotPasswordWindow_c::onRecoveryFailed);
    connect(loginController, &LoginController::validationError, this, &ForgotPasswordWindow_c::onValidationError);
}

ForgotPasswordWindow_c::~ForgotPasswordWindow_c()
{
    delete ui;
}

void ForgotPasswordWindow_c::onRecoverButtonClicked()
{
    ui->statusLabel->clear();
    loginController->attemptRecoverPassword(
        ui->usernameLineEdit->text(),
        ui->securityAnswerLineEdit->text(),
        ui->newPasswordLineEdit->text(),
        ui->confirmNewPasswordLineEdit->text()
        );
}
void ForgotPasswordWindow_c::onBackToLoginButtonClicked()
{
    emit backToLoginRequested();
}

void ForgotPasswordWindow_c::onRecoverySucceeded(const QString &message)
{
    QMessageBox::information(this, "بازیابی موفق", message);
    emit backToLoginRequested();
}

void ForgotPasswordWindow_c::onRecoveryFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void ForgotPasswordWindow_c::onValidationError(const QString &message)
{
    ui->statusLabel->setText(message);
}