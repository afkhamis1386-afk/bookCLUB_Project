#include "registerwindow_c.h"
#include "ui_registerwindow_c.h"
#include <QMessageBox>

RegisterWindow_c::RegisterWindow_c(NetworkManager *networkManager, QWidget *parent):RegisterWindow_c(networkManager, Mode::Registration, QVariantMap(), parent) {}
RegisterWindow_c::RegisterWindow_c(NetworkManager *networkManager, Mode mode, const QVariantMap &accountData, QWidget *parent)
    :QMainWindow(parent)
    , ui(new Ui::RegisterWindow_c) , networkManager(networkManager) , loginController(new LoginController(networkManager, this))
    , profileController(new ProfileController(networkManager, this)) , mode(mode) , editRole(networkManager->getCurrentUserRole()) {
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->publisherFieldsGroupBox->setVisible(false);
    onRoleToggled();
    connect(ui->normalUserRadio, &QRadioButton::toggled, this, &RegisterWindow_c::onRoleToggled);
    connect(ui->publisherRadio, &QRadioButton::toggled, this, &RegisterWindow_c::onRoleToggled);
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWindow_c::onRegisterButtonClicked);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &RegisterWindow_c::onBackToLoginButtonClicked);
    connect(loginController, &LoginController::registrationSucceeded, this, &RegisterWindow_c::onRegistrationSucceeded);
    connect(loginController, &LoginController::registrationFailed, this, &RegisterWindow_c::onRegistrationFailed);
    connect(loginController, &LoginController::validationError, this, &RegisterWindow_c::onValidationError);
    connect(profileController, &ProfileController::accountUpdated, this, &RegisterWindow_c::onAccountUpdated);
    connect(profileController, &ProfileController::accountUpdateFailed, this, &RegisterWindow_c::onAccountUpdateFailed);
    connect(profileController, &ProfileController::validationError, this, &RegisterWindow_c::onValidationError);
    if (mode == Mode::AccountEdit)
        configureAccountEdit(accountData);
}
RegisterWindow_c::~RegisterWindow_c() {
    delete ui;
}
void RegisterWindow_c::configureAccountEdit(const QVariantMap &accountData) {
    setWindowTitle("ویرایش حساب کاربری");
    ui->roleGroupBox->hide();
    ui->registerButton->setText("ذخیره تغییرات");
    ui->backToLoginButton->setText("بازگشت به پروفایل");
    const bool isPublisher = editRole == UserRole::Publisher;
    ui->normalUserRadio->setChecked(!isPublisher);
    ui->publisherRadio->setChecked(isPublisher);
    onRoleToggled();
    ui->usernameLineEdit->setText(accountData.value("username").toString());
    ui->passwordLineEdit->clear();
    ui->confirmPasswordLineEdit->clear();
    ui->securityAnswerLineEdit->clear();
    ui->passwordLineEdit->setPlaceholderText("رمز عبور جدید (برای عدم تغییر خالی بگذارید)");
    ui->confirmPasswordLineEdit->setPlaceholderText("تکرار رمز عبور جدید");
    ui->securityAnswerLineEdit->setEchoMode(QLineEdit::Password);
    ui->securityAnswerLineEdit->setPlaceholderText("پاسخ امنیتی جدید (برای عدم تغییر خالی بگذارید)");
    if(isPublisher){
        ui->firstNameLineEdit->setText(accountData.value("firstName").toString());
        ui->lastNameLineEdit->setText(accountData.value("lastName").toString());
        ui->emailLineEdit->setText(accountData.value("email").toString());
        ui->publicationNameLineEdit->setText(accountData.value("publicationName").toString());
        ui->licenseNumberLineEdit->setText(accountData.value("publisherLicenseNumber").toString());
        ui->shortDescriptionTextEdit->setPlainText(accountData.value("shortDescription").toString());
    }
    ui->statusLabel->setText("رمز عبور و پاسخ امنیتی فقط در صورت وارد کردن مقدار جدید تغییر می کنند.");
}
void RegisterWindow_c::onRoleToggled() {
    const bool isPublisher = ui->publisherRadio->isChecked();
    ui->publisherFieldsGroupBox->setVisible(isPublisher);
}
QVariantMap RegisterWindow_c::buildAccountUpdatePayload() const {
    QVariantMap payload;
    payload["username"] = ui->usernameLineEdit->text().trimmed();
    payload["password"] = ui->passwordLineEdit->text();
    payload["securityAnswer"] = ui->securityAnswerLineEdit->text().trimmed();
    payload["role"] = static_cast<int>(editRole);
    if (editRole == UserRole::Publisher) {
        payload["firstName"] = ui->firstNameLineEdit->text().trimmed();
        payload["lastName"] = ui->lastNameLineEdit->text().trimmed();
        payload["email"] = ui->emailLineEdit->text().trimmed();
        payload["publicationName"] = ui->publicationNameLineEdit->text().trimmed();
        payload["licenseNumber"] = ui->licenseNumberLineEdit->text().trimmed();
        payload["shortDescription"] = ui->shortDescriptionTextEdit->toPlainText().trimmed();
    }
    return payload;
}
void RegisterWindow_c::onRegisterButtonClicked() {
    ui->statusLabel->clear();
    if (mode == Mode::AccountEdit) {
        const QString password = ui->passwordLineEdit->text();
        const QString confirmPassword = ui->confirmPasswordLineEdit->text();
        if (password != confirmPassword) {
            ui->statusLabel->setText("رمز عبور جدید و تکرار آن یکسان نیستند");
            return;
        }
        if(editRole == UserRole::Publisher && (ui->firstNameLineEdit->text().trimmed().isEmpty() || ui->lastNameLineEdit->text().trimmed().isEmpty() || ui->emailLineEdit->text().trimmed().isEmpty() || ui->publicationNameLineEdit->text().trimmed().isEmpty() || ui->licenseNumberLineEdit->text().trimmed().isEmpty())) {
         ui->statusLabel->setText("تمامی فیلدهای اجباری ناشر را پر کنید");
            return;
        }
        ui->registerButton->setEnabled(false);
        profileController->updateAccount(buildAccountUpdatePayload());
        return;
    }
    if (ui->normalUserRadio->isChecked()) {
        loginController->attemptRegisterNormalUser(
            ui->usernameLineEdit->text(), ui->passwordLineEdit->text(), ui->confirmPasswordLineEdit->text(), ui->securityAnswerLineEdit->text() );
    } else if (ui->publisherRadio->isChecked()) {
        loginController->attemptRegisterPublisher(
            ui->usernameLineEdit->text(), ui->passwordLineEdit->text(),
            ui->confirmPasswordLineEdit->text(), ui->securityAnswerLineEdit->text(),
            ui->firstNameLineEdit->text(), ui->lastNameLineEdit->text(),
            ui->emailLineEdit->text(), ui->publicationNameLineEdit->text(),
            ui->licenseNumberLineEdit->text(), ui->shortDescriptionTextEdit->toPlainText() );
    }
    else {
        networkManager->bootstrapFirstAdmin(
            ui->usernameLineEdit->text(), ui->passwordLineEdit->text(),
            ui->securityAnswerLineEdit->text(), ui->firstNameLineEdit->text(), ui->lastNameLineEdit->text() );
    }
}
void RegisterWindow_c::onBackToLoginButtonClicked() {
    if (mode == Mode::AccountEdit)
        emit backToProfileRequested();
    else
        emit backToLoginRequested();
}

void RegisterWindow_c::onRegistrationSucceeded(const QString &message) {
    QMessageBox::information(this, "ثبت نام موفق", message);
    emit backToLoginRequested();
}
void RegisterWindow_c::onRegistrationFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void RegisterWindow_c::onAccountUpdated(const QString &message) {
    ui->registerButton->setEnabled(true);
    disconnect(profileController, &ProfileController::accountUpdated, this, &RegisterWindow_c::onAccountUpdated);

    QMessageBox::information(this, "ویرایش حساب", message);
    emit backToProfileRequested();
}
void RegisterWindow_c::onAccountUpdateFailed(const QString &message) {
    ui->registerButton->setEnabled(true);
    ui->statusLabel->setText(message);
}
void RegisterWindow_c::onValidationError(const QString &message) {
    ui->registerButton->setEnabled(true);
    ui->statusLabel->setText(message);
}
