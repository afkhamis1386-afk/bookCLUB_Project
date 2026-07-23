#include "loginwindow_c.h"
#include "ui_loginwindow_c.h"
#include "homewindow_c.h"
#include "adminmainwindow.h"
#include "publishermainwindow.h"
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
void LoginWindow_c::onLoginSucceeded(UserRole role)
{
    if(role == UserRole::NormalUser){
        if (!networkManager->getHasFavoriteGenres()) {
            if (!genreSelectionWindow) {
                genreSelectionConfirmed = false;
                genreSelectionWindow = new GenreSelectionWindow_c(networkManager);
                genreSelectionWindow->setAttribute(Qt::WA_DeleteOnClose);
                connect(genreSelectionWindow, &GenreSelectionWindow_c::genresConfirmed, this, [this]() {
                    genreSelectionConfirmed = true;
                    HomeWindow_c *homeWindow = new HomeWindow_c(networkManager);
                    homeWindow->setAttribute(Qt::WA_DeleteOnClose);
                    homeWindow->show();
                    genreSelectionWindow->close();
                });
                connect(genreSelectionWindow, &QObject::destroyed, this, [this]() {
                    genreSelectionWindow = nullptr;
                    if (!genreSelectionConfirmed)
                        this->show();
                });
            }
            genreSelectionWindow->show();
            this->hide();
            return;
        }
        HomeWindow_c *homeWindow = new HomeWindow_c(networkManager);
        homeWindow->setAttribute(Qt::WA_DeleteOnClose);
        homeWindow->show();
        this->hide();
    }
    else if (role == UserRole::Admin) {
        AdminMainWindow *adminWindow = new AdminMainWindow(networkManager);
        adminWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(adminWindow, &AdminMainWindow::logoutRequested, this, [this, adminWindow]() {
            adminWindow->close();
            this->show();
        });
        adminWindow->show();
        this->hide();
    }
    else {
        PublisherMainWindow *publisherWindow = new PublisherMainWindow(networkManager);
        publisherWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(publisherWindow, &PublisherMainWindow::logoutRequested, this, [this, publisherWindow]() {
            publisherWindow->close();
            this->show();
        });
        publisherWindow->show();
        this->hide();
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