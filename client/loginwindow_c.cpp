#include "loginwindow_c.h"
#include "ui_loginwindow_c.h"
#include "homewindow_c.h"
#include <QMessageBox>
LoginWindow_c::LoginWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow_c)
    , networkManager(networkManager)
    , loginController(new LoginController(networkManager, this))
{
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow_c::onLoginButtonClicked);
    connect(ui->goToRegisterButton, &QPushButton::clicked, this, &LoginWindow_c::onGoToRegisterButtonClicked);
    connect(ui->forgotPasswordButton, &QPushButton::clicked, this, &LoginWindow_c::onForgotPasswordButtonClicked);
    connect(loginController, &LoginController::loginSucceeded, this, &LoginWindow_c::onLoginSucceeded);
    connect(loginController, &LoginController::loginFailed, this, &LoginWindow_c::onLoginFailed);
    connect(loginController, &LoginController::validationError, this, &LoginWindow_c::onValidationError);
}
LoginWindow_c::~LoginWindow_c()
{
    delete ui;
}
void LoginWindow_c::onLoginButtonClicked()
{
    ui->statusLabel->clear();
    loginController->attemptLogin(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}
void LoginWindow_c::onGoToRegisterButtonClicked()
{
    if(!registerWindow){
        registerWindow = new RegisterWindow_c(networkManager);
        connect(registerWindow, &RegisterWindow_c::backToLoginRequested, this, [this](){
            registerWindow->close();
            this->show();
        });
    }
    registerWindow->show();
    this->hide();
}
void LoginWindow_c::onForgotPasswordButtonClicked()
{
    if(!forgotPasswordWindow) {
        forgotPasswordWindow = new ForgotPasswordWindow_c(networkManager);
        connect(forgotPasswordWindow, &ForgotPasswordWindow_c::backToLoginRequested, this, [this]() {
            forgotPasswordWindow->close();
            this->show();
        });
    }
    forgotPasswordWindow->show();
    this->hide();
}
void LoginWindow_c::onLoginSucceeded(UserRole role){
    if(role == UserRole::NormalUser){
        if(!genreSelectionWindow){
            genreSelectionWindow = new GenreSelectionWindow_c(networkManager);
            connect(genreSelectionWindow, &GenreSelectionWindow_c::genresConfirmed, this, [this]() {
            genreSelectionWindow->close();
            HomeWindow_c *homeWindow = new HomeWindow_c(networkManager);
            homeWindow->setAttribute(Qt::WA_DeleteOnClose);
            homeWindow->show();
            });
        }
        HomeWindow_c *homeWindow = new HomeWindow_c(networkManager);
        homeWindow->setAttribute(Qt::WA_DeleteOnClose);
        homeWindow->show();
        this->hide();
    }
    else if(role == UserRole::Admin){
        AdminMainWindow *adminWindow = new AdminMainWindow(networkManager);
        adminWindow->setAttribute(Qt::WA_DeleteOnClose);
        adminWindow->show();
        this->hide();
    }
    else {
        QMessageBox::information(this, "ورود موفق", "پنل ناشر در قدم بعدی ساخته می شود.");
    }
}
void LoginWindow_c::onLoginFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LoginWindow_c::onValidationError(const QString &message)
{
    ui->statusLabel->setText(message);
}