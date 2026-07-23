#ifndef BOOKDETAILSWINDOW_C_H
#define BOOKDETAILSWINDOW_C_H

#include <QMainWindow>
#include "NetworkManager.h"
#include "BookStoreController.h"
#include "CartController.h"
#include "ReviewController.h"
#include "RatingController.h"
#include "SavedBookController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class BookDetailsWindow_c;
}
QT_END_NAMESPACE

class BookDetailsWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit BookDetailsWindow_c(NetworkManager *networkManager, int bookId, QWidget *parent = nullptr);
    ~BookDetailsWindow_c() override;
signals:
    void backRequested();
private slots:
    void onBookDetailsReceived(const QVariantMap &bookData);
    void onBookDetailsFailed(const QString &message);
    void onCoverImageLoaded(int loadedBookId, const QByteArray &imageData);
    void onReviewsLoaded(const QVariantList &reviews);
    void onReviewsLoadFailed(const QString &message);
    void onAddToCartButtonClicked();
    void onSaveBookButtonClicked();
    void onBackButtonClicked();
    void onSubmitReviewButtonClicked();
    void onSubmitRatingButtonClicked();
    void onAddToCartSucceeded(const QString &message);
    void onAddToCartFailed(const QString &message);
    void onFreeBookClaimed(const QString &message);
    void onFreeBookClaimFailed(const QString &message);
    void onBookSaved(const QString &message);
    void onBookSaveFailed(const QString &message);
    void onReviewSubmitted(int reviewId, const QString &message);
    void onReviewSubmitFailed(const QString &message);
    void onRatingSubmitted(double newAverage, int ratingCount, const QString &message);
    void onRatingSubmitFailed(const QString &message);
    void onValidationError(const QString &message);
    void onRatingSummaryLoaded(double averageRating, int ratingCount);
    void onBookLiveUpdateReceived(const QString &updateType, const QVariantMap &data);
private:
    Ui::BookDetailsWindow_c *ui;
    NetworkManager *networkManager;
    int bookId;
    BookStoreController *bookStoreController;
    CartController *cartController;
    ReviewController *reviewController;
    RatingController *ratingController;
    SavedBookController *savedBookController;
    bool isBookFree = false;
    void populateReviewsList(const QVariantList &reviews);
};

#endif // BOOKDETAILSWINDOW_C_H
