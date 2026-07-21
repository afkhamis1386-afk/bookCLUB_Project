#ifndef PUBLISHERMAINWINDOW_H
#define PUBLISHERMAINWINDOW_H
#include <QMainWindow>
#include "NetworkManager.h"
#include "PublisherDashboardController.h"
#include "PublisherBookController.h"
#include "ProfileController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class PublisherMainWindow;
}
QT_END_NAMESPACE

class PublisherMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PublisherMainWindow(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~PublisherMainWindow() override;

signals:
    void logoutRequested();

private slots:
    void onDashboardLoaded(const QVariantMap &dashboardData);
    void onDashboardLoadFailed(const QString &message);
    void onAddNewBookButtonClicked();
    void onEditBookButtonClicked();
    void onApplyDiscountButtonClicked();
    void onApplyTimedDiscountButtonClicked();
    void onToggleBookActiveButtonClicked();
    void onBookUpdated(const QString &message);
    void onBookUpdateFailed(const QString &message);
    void onDiscountApplied(const QString &message);
    void onDiscountApplyFailed(const QString &message);
    void onTimedDiscountApplied(const QString &message);
    void onTimedDiscountApplyFailed(const QString &message);
    void onBookDeactivated(const QString &message);
    void onBookDeactivateFailed(const QString &message);
    void onBookReactivated(const QString &message);
    void onBookReactivateFailed(const QString &message);
    void onValidationError(const QString &message);
    void onAccountInfoLoaded(const QVariantMap &accountData);
    void onAccountInfoLoadFailed(const QString &message);
    void onLogoutButtonClicked();

private:
    Ui::PublisherMainWindow *ui;
    NetworkManager *networkManager;
    PublisherDashboardController *dashboardController;
    PublisherBookController *bookController;
    ProfileController *profileController;
    QVariantList currentBooks;

    void populateBooksTable(const QVariantList &books);
    int getSelectedBookId() const;
    bool getSelectedBookActiveStatus() const;
};

#endif // PUBLISHERMAINWINDOW_H