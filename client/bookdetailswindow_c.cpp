#include "bookdetailswindow_c.h"
#include "ui_bookdetailswindow_c.h"
#include <QMessageBox>
#include <QPixmap>

BookDetailsWindow_c::BookDetailsWindow_c(NetworkManager *networkManager, int bookId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BookDetailsWindow_c)
    , networkManager(networkManager)
    , bookId(bookId)
    , bookStoreController(new BookStoreController(networkManager, this))
    , cartController(new CartController(networkManager, this))
    , reviewController(new ReviewController(networkManager, this))
    , ratingController(new RatingController(networkManager, this))
    , savedBookController(new SavedBookController(networkManager, this)) {
    ui->setupUi(this);
    ui->descriptionTextEdit->setReadOnly(true);
    connect(ui->addToCartButton, &QPushButton::clicked, this, &BookDetailsWindow_c::onAddToCartButtonClicked);
    connect(ui->saveBookButton, &QPushButton::clicked, this, &BookDetailsWindow_c::onSaveBookButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &BookDetailsWindow_c::onBackButtonClicked);
    connect(ui->submitReviewButton, &QPushButton::clicked, this, &BookDetailsWindow_c::onSubmitReviewButtonClicked);
    connect(ui->submitRatingButton, &QPushButton::clicked, this, &BookDetailsWindow_c::onSubmitRatingButtonClicked);
    connect(bookStoreController, &BookStoreController::bookDetailsReceived, this, &BookDetailsWindow_c::onBookDetailsReceived);
    connect(bookStoreController, &BookStoreController::bookDetailsFailed, this, &BookDetailsWindow_c::onBookDetailsFailed);
    connect(bookStoreController, &BookStoreController::coverImageLoaded, this, &BookDetailsWindow_c::onCoverImageLoaded);
    connect(bookStoreController, &BookStoreController::validationError, this, &BookDetailsWindow_c::onValidationError);
    connect(bookStoreController, &BookStoreController::freeBookClaimed, this, &BookDetailsWindow_c::onFreeBookClaimed);
    connect(bookStoreController, &BookStoreController::freeBookClaimFailed, this, &BookDetailsWindow_c::onFreeBookClaimFailed);
    connect(cartController, &CartController::addToCartSucceeded, this, &BookDetailsWindow_c::onAddToCartSucceeded);
    connect(cartController, &CartController::addToCartFailed, this, &BookDetailsWindow_c::onAddToCartFailed);
    connect(savedBookController, &SavedBookController::bookSaved, this, &BookDetailsWindow_c::onBookSaved);
    connect(savedBookController, &SavedBookController::bookSaveFailed, this, &BookDetailsWindow_c::onBookSaveFailed);
    connect(reviewController, &ReviewController::reviewsLoaded, this, &BookDetailsWindow_c::onReviewsLoaded);
    connect(reviewController, &ReviewController::reviewsLoadFailed, this, &BookDetailsWindow_c::onReviewsLoadFailed);
    connect(reviewController, &ReviewController::reviewSubmitted, this, &BookDetailsWindow_c::onReviewSubmitted);
    connect(reviewController, &ReviewController::reviewSubmitFailed, this, &BookDetailsWindow_c::onReviewSubmitFailed);
    connect(ratingController, &RatingController::ratingSubmitted, this, &BookDetailsWindow_c::onRatingSubmitted);
    connect(ratingController, &RatingController::ratingSubmitFailed, this, &BookDetailsWindow_c::onRatingSubmitFailed);
    connect(ratingController, &RatingController::ratingSummaryLoaded, this, &BookDetailsWindow_c::onRatingSummaryLoaded);
    connect(networkManager, &NetworkManager::bookLiveUpdateReceived, this, &BookDetailsWindow_c::onBookLiveUpdateReceived);
    bookStoreController->loadBookDetails(bookId);
    bookStoreController->loadCoverImage(bookId);
    reviewController->loadReviewsForBook(bookId);
}
BookDetailsWindow_c::~BookDetailsWindow_c() {
    delete ui;
}
void BookDetailsWindow_c::onBookDetailsReceived(const QVariantMap &bookData) {
    ui->bookNameLabel->setText(bookData.value("bookName").toString());
    ui->descriptionTextEdit->setPlainText(bookData.value("description").toString());
    double finalPrice = bookData.value("finalPrice").toDouble();
    double price = bookData.value("price").toDouble();
    isBookFree = (price == 0);
    if (isBookFree)
        ui->priceLabel->setText("رایگان");
    else if (finalPrice < price)
        ui->priceLabel->setText(QString("%1 تومان (تخفیف از %2)").arg(finalPrice, 0, 'f', 0).arg(price, 0, 'f', 0));
    else
        ui->priceLabel->setText(QString("%1 تومان").arg(price, 0, 'f', 0));
    ui->addToCartButton->setText(isBookFree ? "دریافت رایگان" : "افزودن به سبد خرید");
    double avgRating = bookData.value("averageRating").toDouble();
    int ratingCount = bookData.value("ratingCount").toInt();
    ui->ratingLabel->setText(QString("امتیاز: %1 از ۵ (%2 رأی)").arg(avgRating, 0, 'f', 1).arg(ratingCount));
}
void BookDetailsWindow_c::onBookDetailsFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void BookDetailsWindow_c::onCoverImageLoaded(int loadedBookId, const QByteArray &imageData) {
    if (loadedBookId != bookId) return;
    QPixmap pixmap;
    if (pixmap.loadFromData(imageData))
        ui->coverImageLabel->setPixmap(pixmap.scaled(ui->coverImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void BookDetailsWindow_c::populateReviewsList(const QVariantList &reviews) {
    ui->reviewsListWidget->clear();
    for (const QVariant &v : reviews) {
        QVariantMap review = v.toMap();
        ui->reviewsListWidget->addItem("💬 " + review.value("commentText").toString());
        QVariantList replies = review.value("replies").toList();
        for (const QVariant &rv : qAsConst(replies)) {
            QVariantMap reply = rv.toMap();
            ui->reviewsListWidget->addItem("     ↳ " + reply.value("commentText").toString());
        }
    }
}
void BookDetailsWindow_c::onReviewsLoaded(const QVariantList &reviews) { populateReviewsList(reviews); }
void BookDetailsWindow_c::onReviewsLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onAddToCartButtonClicked() {
    if (isBookFree)
        bookStoreController->claimFreeBook(bookId);
    else
        cartController->addBook(bookId);
}
void BookDetailsWindow_c::onFreeBookClaimed(const QString &message) {
    QMessageBox::information(this, "دریافت رایگان", message);
}
void BookDetailsWindow_c::onFreeBookClaimFailed(const QString &message) {
    QMessageBox::warning(this, "دریافت رایگان", message);
}
void BookDetailsWindow_c::onSaveBookButtonClicked() { savedBookController->saveBook(bookId); }
void BookDetailsWindow_c::onBackButtonClicked() { emit backRequested(); }
void BookDetailsWindow_c::onSubmitReviewButtonClicked() {
    QString text = ui->newReviewTextEdit->toPlainText().trimmed();
    reviewController->submitReview(bookId, text, -1);
}
void BookDetailsWindow_c::onSubmitRatingButtonClicked() {
    int ratingValue = ui->ratingComboBox->currentIndex() + 1;
    ratingController->submitRating(bookId, ratingValue);
}
void BookDetailsWindow_c::onAddToCartSucceeded(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onAddToCartFailed(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onBookSaved(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onBookSaveFailed(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onReviewSubmitted(int reviewId, const QString &message) {
    Q_UNUSED(reviewId)
    ui->statusLabel->setText(message);
    ui->newReviewTextEdit->clear();
    reviewController->loadReviewsForBook(bookId);
}
void BookDetailsWindow_c::onReviewSubmitFailed(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onRatingSubmitted(double newAverage, int ratingCount, const QString &message) {
    ui->statusLabel->setText(message);
    ui->ratingLabel->setText(QString("امتیاز: %1 از ۵ (%2 رأی)").arg(newAverage, 0, 'f', 1).arg(ratingCount));
}
void BookDetailsWindow_c::onRatingSubmitFailed(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onValidationError(const QString &message) { ui->statusLabel->setText(message); }
void BookDetailsWindow_c::onRatingSummaryLoaded(double averageRating, int ratingCount) {
    ui->ratingLabel->setText(QString("امتیاز: %1 از ۵ (%2 رأی)").arg(averageRating, 0, 'f', 1).arg(ratingCount));
}
void BookDetailsWindow_c::onBookLiveUpdateReceived(const QString &updateType, const QVariantMap &data) {
    if (data.value("bookId").toInt() != bookId)
        return;
    if (updateType == "newReview") {
        reviewController->loadReviewsForBook(bookId);
    } else if (updateType == "newRating") {
        ratingController->loadRatingSummary(bookId);
    }
}