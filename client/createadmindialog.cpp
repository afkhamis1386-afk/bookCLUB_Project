#include "createadmindialog.h"
#include "ui_createadmindialog.h"
#include <QMessageBox>
#include <QDialogButtonBox>

CreateAdminDialog::CreateAdminDialog(NetworkManager *networkManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateAdminDialog)
    , adminController(new AdminController(networkManager, this)) {
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CreateAdminDialog::onAccepted);
    connect(adminController, &AdminController::adminCreated, this, &CreateAdminDialog::onAdminCreated);
    connect(adminController, &AdminController::adminCreateFailed, this, &CreateAdminDialog::onAdminCreateFailed);
    connect(adminController, &AdminController::validationError, this, &CreateAdminDialog::onValidationError);
}
CreateAdminDialog::~CreateAdminDialog() {
    delete ui;
}
void CreateAdminDialog::onAccepted() {
    adminController->createAdmin(
        ui->usernameLineEdit->text(),
        ui->passwordLineEdit->text(),
        ui->securityAnswerLineEdit->text(),
        ui->firstNameLineEdit->text(),
        ui->lastNameLineEdit->text()
        );
}
void CreateAdminDialog::onAdminCreated(const QString &message) {
    QMessageBox::information(this, "موفق", message);
    accept();
}
void CreateAdminDialog::onAdminCreateFailed(const QString &message) {
    QMessageBox::warning(this, "خطا", message);
}
void CreateAdminDialog::onValidationError(const QString &message) {
    QMessageBox::warning(this, "خطا", message);
}