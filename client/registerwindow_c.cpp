#include "registerwindow_c.h"
#include "ui_registerwindow_c.h"
#include <QMessageBox>
RegisterWindow_c::RegisterWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegisterWindow_c)
    , networkManager(networkManager)
    , loginController(new LoginController(networkManager, this))
{
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->publisherFieldsStack->setCurrentIndex(0);
    connect(ui->normalUserRadio, &QRadioButton::toggled, this, &RegisterWindow_c::onRoleToggled);
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWindow_c::onRegisterButtonClicked);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &RegisterWindow_c::onBackToLoginButtonClicked);

    connect(loginController, &LoginController::registrationSucceeded, this, &RegisterWindow_c::onRegistrationSucceeded);
    connect(loginController, &LoginController::registrationFailed, this, &RegisterWindow_c::onRegistrationFailed);
    connect(loginController, &LoginController::validationError, this, &RegisterWindow_c::onValidationError);
}

RegisterWindow_c::~RegisterWindow_c()
{
    delete ui;
}

void RegisterWindow_c::onRoleToggled()
{
    ui->publisherFieldsStack->setCurrentIndex(ui->normalUserRadio->isChecked() ? 0 : 1);
}

void RegisterWindow_c::onRegisterButtonClicked()
{
    ui->statusLabel->clear();

    if (ui->normalUserRadio->isChecked()) {
        loginController->attemptRegisterNormalUser(
            ui->usernameLineEdit->text(),
            ui->passwordLineEdit->text(),
            ui->confirmPasswordLineEdit->text(),
            ui->securityAnswerLineEdit->text()
            );
    } else if (ui->publisherRadio->isChecked()) {
        loginController->attemptRegisterPublisher(
            ui->usernameLineEdit->text(),
            ui->passwordLineEdit->text(),
            ui->confirmPasswordLineEdit->text(),
            ui->securityAnswerLineEdit->text(),
            ui->firstNameLineEdit->text(),
            ui->lastNameLineEdit->text(),
            ui->emailLineEdit->text(),
            ui->publicationNameLineEdit->text(),
            ui->licenseNumberLineEdit->text(),
            ui->shortDescriptionTextEdit->toPlainText()
            );
    } else {
        networkManager->bootstrapFirstAdmin(
            ui->usernameLineEdit->text(),
            ui->passwordLineEdit->text(),
            ui->securityAnswerLineEdit->text(),
            ui->firstNameLineEdit->text(),
            ui->lastNameLineEdit->text()
            );
    }
}
void RegisterWindow_c::onBackToLoginButtonClicked()
{
    emit backToLoginRequested();
}

void RegisterWindow_c::onRegistrationSucceeded(const QString &message)
{
    QMessageBox::information(this, "ثبت نام موفق", message);
    emit backToLoginRequested();
}

void RegisterWindow_c::onRegistrationFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void RegisterWindow_c::onValidationError(const QString &message)
{
    ui->statusLabel->setText(message);
}