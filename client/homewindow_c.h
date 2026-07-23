#ifndef HOMEWINDOW_C_H
#define HOMEWINDOW_C_H
#include <QMainWindow>
#include <QGridLayout>
#include <QListWidgetItem>
#include <QWidget>
#include "NetworkManager.h"
#include "BookStoreController.h"
#include "CartController.h"
#include "NotificationController.h"
#include "bookcardwidget.h"
#include "bookdetailswindow_c.h"
#include "notificationwindow_c.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class HomeWindow_c;
}
QT_END_NAMESPACE

class HomeWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~HomeWindow_c() override;

private slots:
    void onSearchButtonClicked();
    void onRecommendedTabClicked();
    void onNewestTabClicked();
    void onFreeTabClicked();
    void onBestSellersTabClicked();
    void onPopularTabClicked();
    void onAllTabClicked();
    void onCartButtonClicked();
    void onNotificationsButtonClicked();
    void onLibraryButtonClicked();
    void onProfileButtonClicked();
    void onBooksLoaded(const QVariantList &bookIds);
    void onBooksLoadFailed(const QString &message);
    void onRecommendedBooksLoaded(const QVariantList &bookIds);
    void onRecommendedBooksLoadFailed(const QString &message);
    void onSearchResultsReceived(const QVariantList &bookIds);
    void onSearchFailed(const QString &message);
    void onCoverImageLoaded(int bookId, const QByteArray &imageData);
    void onCardClicked(int bookId);
    void onCartLoaded(const QVariantMap &cartData);
    void onUnreadCountLoaded(int count);
    void onBookDetailsReceived(const QVariantMap &bookData);
    void onNewNotificationArrived(const QVariantMap &notificationData);
    void onBookLiveUpdateReceived(const QString &updateType, const QVariantMap &data);

private:
    Ui::HomeWindow_c *ui;
    NetworkManager *networkManager;
    BookStoreController *bookStoreController;
    CartController *cartController;
    NotificationController *notificationController;
    QWidget *gridContainer;
    QGridLayout *gridLayout;
    QMap<int, BookCardWidget*> cardsByBookId;
    RequestType currentTab = RequestType::GetRecommendedBooks;
    void clearBookGrid();
    void displayBooks(const QVariantList &bookIds);
    void loadBookNameFromDetails(int bookId, BookCardWidget *card);
    BookDetailsWindow_c *bookDetailsWindow = nullptr;
};

#endif // HOMEWINDOW_C_H