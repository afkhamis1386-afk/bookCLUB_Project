#include "homewindow_c.h"
#include "ui_homewindow_c.h"
#include <QMessageBox>
#include <QScrollArea>

HomeWindow_c::HomeWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow_c)
    , networkManager(networkManager)
    , bookStoreController(new BookStoreController(networkManager, this))
    , cartController(new CartController(networkManager, this))
    , notificationController(new NotificationController(networkManager, this))
{
    ui->setupUi(this);
    gridContainer = new QWidget();
    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(15);
    ui->booksScrollArea->setWidget(gridContainer);
    ui->booksScrollArea->setWidgetResizable(true);
    connect(ui->searchButton, &QPushButton::clicked, this, &HomeWindow_c::onSearchButtonClicked);
    connect(ui->recommendedTabButton, &QPushButton::clicked, this, &HomeWindow_c::onRecommendedTabClicked);
    connect(ui->newestTabButton, &QPushButton::clicked, this, &HomeWindow_c::onNewestTabClicked);
    connect(ui->freeTabButton, &QPushButton::clicked, this, &HomeWindow_c::onFreeTabClicked);
    connect(ui->allTabButton, &QPushButton::clicked, this, &HomeWindow_c::onAllTabClicked);
    connect(ui->cartButton, &QPushButton::clicked, this, &HomeWindow_c::onCartButtonClicked);
    connect(ui->notificationsButton, &QPushButton::clicked, this, &HomeWindow_c::onNotificationsButtonClicked);
    connect(ui->libraryButton, &QPushButton::clicked, this, &HomeWindow_c::onLibraryButtonClicked);
    connect(ui->profileButton, &QPushButton::clicked, this, &HomeWindow_c::onProfileButtonClicked);
    connect(bookStoreController, &BookStoreController::booksLoaded, this, &HomeWindow_c::onBooksLoaded);
    connect(bookStoreController, &BookStoreController::booksLoadFailed, this, &HomeWindow_c::onBooksLoadFailed);
    connect(bookStoreController, &BookStoreController::recommendedBooksLoaded, this, &HomeWindow_c::onRecommendedBooksLoaded);
    connect(bookStoreController, &BookStoreController::recommendedBooksLoadFailed, this, &HomeWindow_c::onRecommendedBooksLoadFailed);
    connect(bookStoreController, &BookStoreController::searchResultsReceived, this, &HomeWindow_c::onSearchResultsReceived);
    connect(bookStoreController, &BookStoreController::searchFailed, this, &HomeWindow_c::onSearchFailed);
    connect(bookStoreController, &BookStoreController::coverImageLoaded, this, &HomeWindow_c::onCoverImageLoaded);
    connect(cartController, &CartController::cartLoaded, this, &HomeWindow_c::onCartLoaded);
    connect(notificationController, &NotificationController::unreadCountLoaded, this, &HomeWindow_c::onUnreadCountLoaded);
    connect(bookStoreController, &BookStoreController::bookDetailsReceived, this, &HomeWindow_c::onBookDetailsReceived);
    bookStoreController->loadRecommendedBooks();
    cartController->refreshCart();
    notificationController->refreshUnreadCount();
}

HomeWindow_c::~HomeWindow_c()
{
    delete ui;
}

void HomeWindow_c::clearBookGrid()
{
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    cardsByBookId.clear();
}

void HomeWindow_c::displayBooks(const QVariantList &bookIds)
{
    clearBookGrid();
    ui->statusLabel->clear();

    if (bookIds.isEmpty()) {
        ui->statusLabel->setText("کتابی برای نمایش یافت نشد");
        return;
    }

    const int columnsPerRow = 4;
    int row = 0, col = 0;
    for (const QVariant &v : bookIds) {
        int bookId = v.toInt();
        BookCardWidget *card = new BookCardWidget(bookId, gridContainer);
        connect(card, &BookCardWidget::clicked, this, &HomeWindow_c::onCardClicked);
        gridLayout->addWidget(card, row, col);
        cardsByBookId[bookId] = card;
        loadBookNameFromDetails(bookId, card);
        bookStoreController->loadCoverImage(bookId);
        col++;
        if (col >= columnsPerRow) { col = 0; row++; }
    }
}
void HomeWindow_c::loadBookNameFromDetails(int bookId, BookCardWidget *card)
{
    card->setBookInfo("در حال بارگذاری...", 0);
    bookStoreController->loadBookDetails(bookId);
}
void HomeWindow_c::onBookDetailsReceived(const QVariantMap &bookData)
{
    int bookId = bookData.value("bookId").toInt();
    if(!cardsByBookId.contains(bookId))
        return;
    QString bookName = bookData.value("bookName").toString();
    double finalPrice = bookData.value("finalPrice").toDouble();
    cardsByBookId[bookId]->setBookInfo(bookName, finalPrice);
}

void HomeWindow_c::onBooksLoaded(const QVariantList &bookIds) { displayBooks(bookIds); }
void HomeWindow_c::onBooksLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void HomeWindow_c::onRecommendedBooksLoaded(const QVariantList &bookIds) { displayBooks(bookIds); }
void HomeWindow_c::onRecommendedBooksLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void HomeWindow_c::onSearchResultsReceived(const QVariantList &bookIds) { displayBooks(bookIds); }
void HomeWindow_c::onSearchFailed(const QString &message) { ui->statusLabel->setText(message); }
void HomeWindow_c::onCoverImageLoaded(int bookId, const QByteArray &imageData)
{
    if (cardsByBookId.contains(bookId))
        cardsByBookId[bookId]->setCoverImage(imageData);
}

void HomeWindow_c::onSearchButtonClicked()
{
    QString query = ui->searchLineEdit->text().trimmed();
    if (query.isEmpty()) {
        bookStoreController->loadRecommendedBooks();
        return;
    }
    bookStoreController->search(query);
}

void HomeWindow_c::onRecommendedTabClicked() { bookStoreController->loadRecommendedBooks(); }
void HomeWindow_c::onNewestTabClicked() { bookStoreController->loadNewestBooks(20); }
void HomeWindow_c::onFreeTabClicked() { bookStoreController->loadFreeBooks(); }
void HomeWindow_c::onAllTabClicked() { bookStoreController->loadAllBooks(); }

void HomeWindow_c::onCardClicked(int bookId)
{
    QMessageBox::information(this, "جزئیات کتاب", QString("صفحه ی جزئیات کتاب #%1 در قدم بعدی ساخته می شود.").arg(bookId));
}

void HomeWindow_c::onCartButtonClicked()
{
    QMessageBox::information(this, "سبد خرید", "صفحه ی سبد خرید در قدم بعدی ساخته می شود.");
}

void HomeWindow_c::onNotificationsButtonClicked()
{
    QMessageBox::information(this, "اعلان ها", "صفحه ی اعلان ها در قدم بعدی ساخته می شود.");
}

void HomeWindow_c::onLibraryButtonClicked()
{
    QMessageBox::information(this, "کتابخانه من", "صفحه ی کتابخانه در قدم بعدی ساخته می شود.");
}

void HomeWindow_c::onProfileButtonClicked()
{
    QMessageBox::information(this, "پروفایل", "صفحه ی پروفایل در قدم بعدی ساخته می شود.");
}

void HomeWindow_c::onCartLoaded(const QVariantMap &cartData)
{
    int itemCount = cartData.value("itemCount").toInt();
    ui->cartButton->setText(QString("سبد خرید (%1)").arg(itemCount));
}

void HomeWindow_c::onUnreadCountLoaded(int count)
{
    ui->notificationsButton->setText(QString("اعلان ها (%1)").arg(count));
}