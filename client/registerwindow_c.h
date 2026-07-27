#ifndef REGISTERWINDOW_C_H
#define REGISTERWINDOW_C_H
#include <QMainWindow>
#include <QVariantMap>
#include "NetworkManager.h"
#include "LoginController.h"
#include "ProfileController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class RegisterWindow_c;
}
QT_END_NAMESPACE

class RegisterWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    enum class Mode {
        Registration,
        AccountEdit
    };

    explicit RegisterWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    RegisterWindow_c(NetworkManager *networkManager, Mode mode, const QVariantMap &accountData, QWidget *parent = nullptr);
    ~RegisterWindow_c() override;

signals:
    void backToLoginRequested();
    void backToProfileRequested();

private slots:
    void onRoleToggled();
    void onRegisterButtonClicked();
    void onBackToLoginButtonClicked();
    void onRegistrationSucceeded(const QString &message);
    void onRegistrationFailed(const QString &message);
    void onAccountUpdated(const QString &message);
    void onAccountUpdateFailed(const QString &message);
    void onValidationError(const QString &message);

private:
    Ui::RegisterWindow_c *ui;
    NetworkManager *networkManager;
    LoginController *loginController;
    ProfileController *profileController;
    Mode mode;
    UserRole editRole;

    void configureAccountEdit(const QVariantMap &accountData);
    QVariantMap buildAccountUpdatePayload() const;
};

#endif // REGISTERWINDOW_C_H
