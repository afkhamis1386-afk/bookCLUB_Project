#ifndef CREATEADMINDIALOG_H
#define CREATEADMINDIALOG_H

#include <QDialog>
#include "NetworkManager.h"
#include "AdminController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class CreateAdminDialog;
}
QT_END_NAMESPACE

class CreateAdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateAdminDialog(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~CreateAdminDialog() override;

private slots:
    void onAccepted();
    void onAdminCreated(const QString &message);
    void onAdminCreateFailed(const QString &message);
    void onValidationError(const QString &message);
private:
    Ui::CreateAdminDialog *ui;
    AdminController *adminController;
};

#endif // CREATEADMINDIALOG_H
