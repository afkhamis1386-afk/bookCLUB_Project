#ifndef PROFILEWINDOW_C_H
#define PROFILEWINDOW_C_H
#include <QMainWindow>
#include "NetworkManager.h"
#include "ProfileController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ProfileWindow_c;
}
QT_END_NAMESPACE

class ProfileWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfileWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~ProfileWindow_c() override;

signals:
    void backRequested();

private slots:
    void onAccountInfoLoaded(const QVariantMap &accountData);
    void onAccountInfoLoadFailed(const QString &message);
    void onOrderHistoryLoaded(const QVariantList &orders);
    void onOrderHistoryLoadFailed(const QString &message);
    void onChangePasswordButtonClicked();
    void onPasswordChanged(const QString &message);
    void onPasswordChangeFailed(const QString &message);
    void onEditGenresButtonClicked();
    void onValidationError(const QString &message);
    void onBackButtonClicked();

private:
    Ui::ProfileWindow_c *ui;
    NetworkManager *networkManager;
    ProfileController *profileController;
};

#endif // PROFILEWINDOW_C_H