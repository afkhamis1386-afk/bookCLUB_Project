#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>
#include "NetworkManager.h"
#include "AdminController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AdminMainWindow;
}
QT_END_NAMESPACE

class AdminMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminMainWindow(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~AdminMainWindow() override;

signals:
    void logoutRequested();

private slots:
    void onUsersLoaded(const QVariantList &users);
    void onUsersLoadFailed(const QString &message);
    void onBlockUserButtonClicked();
    void onUnblockUserButtonClicked();
    void onToggleActiveButtonClicked();
    void onDeleteUserButtonClicked();
    void onUserBlocked(const QString &message);
    void onUserBlockFailed(const QString &message);
    void onUserUnblocked(const QString &message);
    void onUserUnblockFailed(const QString &message);
    void onUserActiveStatusChanged(const QString &message);
    void onUserActiveStatusChangeFailed(const QString &message);
    void onUserDeleted(const QString &message);
    void onUserDeleteFailed(const QString &message);
    void onAllBooksLoaded(const QVariantList &books);
    void onAllBooksLoadFailed(const QString &message);
    void onViewBookDetailsButtonClicked();
    void onBookDetailsForReviewLoaded(const QVariantMap &bookData);
    void onBookDetailsForReviewLoadFailed(const QString &message);
    void onDeleteBookButtonClicked();
    void onBookDeleted(const QString &message);
    void onBookDeleteFailed(const QString &message);
    void onAllReviewsLoaded(const QVariantList &reviews);
    void onAllReviewsLoadFailed(const QString &message);
    void onDeleteReviewButtonClicked();
    void onReviewDeleted(const QString &message);
    void onReviewDeleteFailed(const QString &message);
    void onValidationError(const QString &message);
    void onLogoutButtonClicked();

private:
    Ui::AdminMainWindow *ui;
    NetworkManager *networkManager;
    AdminController *adminController;
    int getSelectedUserId() const;
    int getSelectedBookId() const;
    int getSelectedReviewId() const;
};

#endif // ADMINMAINWINDOW_H
