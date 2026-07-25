#include "homewindow_c.h"
#include "ui_homewindow_c.h"
#include "cartwindow_c.h"
#include "profilewindow_c.h"
#include "librarywindow_c.h"
#include "notificationwindow_c.h"
#include "windownav.h"
#include <QMessageBox>
#include <QScrollArea>
HomeWindow_c::HomeWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow_c)
    , networkManager(networkManager)
    , bookStoreController(new BookStoreController(networkManager, this))
    , cartController(new CartController(networkManager, this))
    , notificationController(new NotificationController(networkManager, this))
    , profileController(new ProfileController(networkManager, this))
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
    connect(ui->bestSellersTabButton, &QPushButton::clicked, this, &HomeWindow_c::onBestSellersTabClicked);
    connect(ui->popularTabButton, &QPushButton::clicked, this, &HomeWindow_c::onPopularTabClicked);
    connect(ui->allTabButton, &QPushButton::clicked, this, &HomeWindow_c::onAllTabClicked);
    connect(ui->genreFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HomeWindow_c::onGenreFilterChanged);
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
    connect(notificationController, &NotificationController::newNotificationArrived, this, &HomeWindow_c::onNewNotificationArrived);
    connect(bookStoreController, &BookStoreController::bookDetailsReceived, this, &HomeWindow_c::onBookDetailsReceived);
    connect(networkManager, &NetworkManager::bookLiveUpdateReceived, this, &HomeWindow_c::onBookLiveUpdateReceived);
    connect(profileController, &ProfileController::genresLoaded, this, &HomeWindow_c::onGenresLoaded);
    bookStoreController->loadRecommendedBooks();
    cartController->refreshCart();
    notificationController->refreshUnreadCount();
    profileController->loadGenres();
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

    if(bookIds.isEmpty()){
        ui->statusLabel->setText("کتابی برای نمایش یافت نشد");
        return;
    }

    const int columnsPerRow = 4;
    int row = 0, col = 0;
    for(const QVariant &v : bookIds){
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
    int genreId = bookData.value("genreId").toInt();
    cardsByBookId[bookId]->setGenreName(genreNamesById.value(genreId));
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
    resetGenreFilterUi();
    if (query.isEmpty()) {
        bookStoreController->loadRecommendedBooks();
        return;
    }
    bookStoreController->search(query);
}

void HomeWindow_c::onRecommendedTabClicked() { resetGenreFilterUi(); currentTab = RequestType::GetRecommendedBooks; bookStoreController->loadRecommendedBooks(); }
void HomeWindow_c::onNewestTabClicked() { resetGenreFilterUi(); currentTab = RequestType::GetNewestBooks; bookStoreController->loadNewestBooks(20); }
void HomeWindow_c::onFreeTabClicked() { resetGenreFilterUi(); currentTab = RequestType::GetFreeBooks; bookStoreController->loadFreeBooks(); }
void HomeWindow_c::onBestSellersTabClicked() { resetGenreFilterUi(); currentTab = RequestType::GetBestSellers; bookStoreController->loadBestSellers(20); }
void HomeWindow_c::onPopularTabClicked() { resetGenreFilterUi(); currentTab = RequestType::GetPopularBooks; bookStoreController->loadPopularBooks(20); }
void HomeWindow_c::onAllTabClicked() { resetGenreFilterUi(); currentTab = RequestType::GetBooks; bookStoreController->loadAllBooks(); }

void HomeWindow_c::resetGenreFilterUi()
{
    ui->genreFilterComboBox->blockSignals(true);
    ui->genreFilterComboBox->setCurrentIndex(0);
    ui->genreFilterComboBox->blockSignals(false);
}

void HomeWindow_c::onGenresLoaded(const QVariantList &genres)
{
    genreNamesById.clear();
    ui->genreFilterComboBox->blockSignals(true);
    ui->genreFilterComboBox->clear();
    ui->genreFilterComboBox->addItem("همه ژانرها", -1);
    for (const QVariant &v : genres) {
        QVariantMap genreMap = v.toMap();
        int genreId = genreMap.value("genreId").toInt();
        QString genreTitle = genreMap.value("genreTitle").toString();
        genreNamesById[genreId] = genreTitle;
        ui->genreFilterComboBox->addItem(genreTitle, genreId);
    }
    ui->genreFilterComboBox->blockSignals(false);
}

void HomeWindow_c::onGenreFilterChanged(int index)
{
    int genreId = ui->genreFilterComboBox->itemData(index).toInt();
    if (genreId <= 0) {
        currentTab = RequestType::GetBooks;
        bookStoreController->loadAllBooks();
    } else {
        currentTab = RequestType::GetBooksByGenre;
        bookStoreController->loadBooksByGenre(genreId);
    }
}
void HomeWindow_c::onBookLiveUpdateReceived(const QString &updateType, const QVariantMap &data) {
    Q_UNUSED(data)
    if (updateType == "newRating" && currentTab == RequestType::GetPopularBooks) {
        bookStoreController->loadPopularBooks(20);
    } else if (updateType == "newSale" && currentTab == RequestType::GetBestSellers) {
        bookStoreController->loadBestSellers(20);
    }
}
void HomeWindow_c::onCardClicked(int bookId)
{
    if (bookDetailsWindow) {
        bookDetailsWindow->close();
        delete bookDetailsWindow;
    }
    bookDetailsWindow = new BookDetailsWindow_c(networkManager, bookId);
    connect(bookDetailsWindow, &BookDetailsWindow_c::backRequested, this, [this]() {
        bookDetailsWindow->close();
        this->show();
    });
    showFollowingState(bookDetailsWindow, this);
    this->hide();
}
void HomeWindow_c::onCartButtonClicked()
{
    CartWindow_c *cartWindow = new CartWindow_c(networkManager);
    cartWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(cartWindow, &CartWindow_c::backRequested, this, [this, cartWindow]() {
        cartWindow->close();
        cartController->refreshCart();
        this->show();
    });
    showFollowingState(cartWindow, this);
    this->hide();
}

void HomeWindow_c::onNotificationsButtonClicked()
{
    NotificationWindow_c *notificationWindow = new NotificationWindow_c(networkManager);
    notificationWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(notificationWindow, &NotificationWindow_c::backRequested, this, [this, notificationWindow]() {
        notificationWindow->close();
        notificationController->refreshUnreadCount();
        this->show();
    });
    showFollowingState(notificationWindow, this);
    this->hide();
}
void HomeWindow_c::onLibraryButtonClicked()
{
    LibraryWindow_c *libraryWindow =  new LibraryWindow_c(networkManager);
    libraryWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(libraryWindow,  &LibraryWindow_c::backRequested,  this,   [this, libraryWindow]()  {
        libraryWindow->close();
        this->show();
    });

    showFollowingState(libraryWindow, this);
    this->hide();
}
void HomeWindow_c::onProfileButtonClicked()
{
    ProfileWindow_c *profileWindow = new ProfileWindow_c(networkManager);
    profileWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(profileWindow, &ProfileWindow_c::backRequested, this, [this, profileWindow]() {
        profileWindow->close();
        this->show();
    });
    connect(profileWindow, &ProfileWindow_c::logoutRequested, this, [this, profileWindow]() {
        profileWindow->close();
        emit logoutRequested();
    });
    showFollowingState(profileWindow, this);
    this->hide();
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

void HomeWindow_c::onNewNotificationArrived(const QVariantMap &notificationData)
{
    notificationController->refreshUnreadCount();
    statusBar()->showMessage(
        QString("اعلان جدید: %1").arg(notificationData.value("title").toString()), 5000);
}