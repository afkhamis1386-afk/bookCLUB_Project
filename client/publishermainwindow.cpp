#include "publishermainwindow.h"
#include "publisheraddbookwindow_c.h"
#include "registerwindow_c.h"
#include "notificationwindow_c.h"
#include "ui_publishermainwindow.h"
#include "windownav.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>

PublisherMainWindow::PublisherMainWindow(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PublisherMainWindow)
    , networkManager(networkManager)
    , dashboardController(new PublisherDashboardController(networkManager, this))
    , bookController(new PublisherBookController(networkManager, this))
    , profileController(new ProfileController(networkManager, this))
    , notificationController(new NotificationController(networkManager, this)) {
    ui->setupUi(this);
    ui->myBooksTableWidget->setColumnCount(7);
    ui->myBooksTableWidget->setHorizontalHeaderLabels({"شناسه", "نام کتاب", "میانگین امتیاز", "تعداد فروش", "وضعیت", "تخفیف عادی", "تخفیف زمان دار"});
    ui->myBooksTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->myBooksTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->myBooksTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->refreshDashboardButton, &QPushButton::clicked, dashboardController, &PublisherDashboardController::refreshDashboard);
    connect(ui->addNewBookButton, &QPushButton::clicked, this, &PublisherMainWindow::onAddNewBookButtonClicked);
    connect(ui->editBookButton, &QPushButton::clicked, this, &PublisherMainWindow::onEditBookButtonClicked);
    connect(ui->toggleBookActiveButton, &QPushButton::clicked, this, &PublisherMainWindow::onToggleBookActiveButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &PublisherMainWindow::onLogoutButtonClicked);
    connect(ui->editAccountButton, &QPushButton::clicked, this, &PublisherMainWindow::onEditAccountButtonClicked);
    connect(ui->notificationsButton, &QPushButton::clicked, this, &PublisherMainWindow::onNotificationsButtonClicked);
    ui->editAccountButton->setEnabled(false);
    connect(dashboardController, &PublisherDashboardController::dashboardLoaded, this, &PublisherMainWindow::onDashboardLoaded);
    connect(dashboardController, &PublisherDashboardController::dashboardLoadFailed, this, &PublisherMainWindow::onDashboardLoadFailed);
    connect(bookController, &PublisherBookController::bookDeactivated, this, &PublisherMainWindow::onBookDeactivated);
    connect(bookController, &PublisherBookController::bookDeactivateFailed, this, &PublisherMainWindow::onBookDeactivateFailed);
    connect(bookController, &PublisherBookController::bookReactivated, this, &PublisherMainWindow::onBookReactivated);
    connect(bookController, &PublisherBookController::bookReactivateFailed, this, &PublisherMainWindow::onBookReactivateFailed);
    connect(bookController, &PublisherBookController::validationError, this, &PublisherMainWindow::onValidationError);
    connect(profileController, &ProfileController::accountInfoLoaded, this, &PublisherMainWindow::onAccountInfoLoaded);
    connect(profileController, &ProfileController::accountInfoLoadFailed, this, &PublisherMainWindow::onAccountInfoLoadFailed);
    connect(notificationController, &NotificationController::unreadCountLoaded, this, &PublisherMainWindow::onUnreadCountLoaded);
    connect(notificationController, &NotificationController::unreadCountLoadFailed, this, &PublisherMainWindow::onUnreadCountLoadFailed);
    connect(notificationController, &NotificationController::newNotificationArrived, this, &PublisherMainWindow::onNewNotificationArrived);
    connect(notificationController, &NotificationController::notificationMarkedRead, this, &PublisherMainWindow::onNotificationMarkedRead);
    dashboardController->refreshDashboard();
    profileController->loadAccountInfo();
    notificationController->refreshUnreadCount();
}
PublisherMainWindow::~PublisherMainWindow() {
    delete ui;
}
void PublisherMainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    notificationController->refreshUnreadCount();
}
void PublisherMainWindow::populateBooksTable(const QVariantList &books) {
    currentBooks = books;
    ui->myBooksTableWidget->setRowCount(books.size());
    for (int i = 0; i < books.size(); ++i) {
        QVariantMap b = books[i].toMap();
        ui->myBooksTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(b.value("bookId").toInt())));
        ui->myBooksTableWidget->setItem(i, 1, new QTableWidgetItem(b.value("bookName").toString()));
        ui->myBooksTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(b.value("averageRating").toDouble(), 'f', 1)));
        ui->myBooksTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(b.value("soldCount").toInt())));
        ui->myBooksTableWidget->setItem(i, 4, new QTableWidgetItem(b.value("isActive").toBool() ? "فعال" : "غیرفعال"));
        double discountPercent = b.value("discountPercent").toDouble();
        QString normalDiscountText = discountPercent > 0 ? QString("%1 %").arg(discountPercent, 0, 'f', 0) : "ندارد";
        ui->myBooksTableWidget->setItem(i, 5, new QTableWidgetItem(normalDiscountText));
        QString timedDiscountText = "ندارد";
        if (b.contains("timedDiscountPercent")) {
            double timedPercent = b.value("timedDiscountPercent").toDouble();
            QDateTime start = b.value("timedDiscountStart").toDateTime();
            QDateTime end = b.value("timedDiscountEnd").toDateTime();
            timedDiscountText = QString("%1 % (از %2 تا %3)").arg(timedPercent, 0, 'f', 0).arg(start.toString("yyyy/MM/dd HH:mm")).arg(end.toString("yyyy/MM/dd HH:mm"));
        }
        ui->myBooksTableWidget->setItem(i, 6, new QTableWidgetItem(timedDiscountText));
    }
}
int PublisherMainWindow::getSelectedBookId() const {
    int row = ui->myBooksTableWidget->currentRow();
    if (row < 0) return -1;
    return ui->myBooksTableWidget->item(row, 0)->text().toInt();
}
bool PublisherMainWindow::getSelectedBookActiveStatus() const {
    int row = ui->myBooksTableWidget->currentRow();
    if (row < 0) return false;
    return ui->myBooksTableWidget->item(row, 4)->text() == "فعال";
}
void PublisherMainWindow::onDashboardLoaded(const QVariantMap &dashboardData) {
    ui->statusLabel->clear();
    ui->totalBooksLabel->setText("تعداد کل کتاب ها: " + QString::number(dashboardData.value("totalBooksCount").toInt()));
    ui->totalRevenueLabel->setText("مجموع درآمد: " + QString::number(dashboardData.value("totalRevenue").toDouble(), 'f', 0) + " تومان");
    ui->topSellingListWidget->clear();
    for(const QVariant &v : dashboardData.value("topSellingBooks").toList()){
        QVariantMap item = v.toMap();
        ui->topSellingListWidget->addItem(QString("%1 — %2 فروش").arg(item.value("bookName").toString()).arg(item.value("soldCount").toInt()));
    }
    ui->leastSellingListWidget->clear();
    for(const QVariant &v : dashboardData.value("leastSellingBooks").toList()){
        QVariantMap item = v.toMap();
        ui->leastSellingListWidget->addItem(QString("%1 — %2 فروش").arg(item.value("bookName").toString()).arg(item.value("soldCount").toInt()));
    }
    populateBooksTable(dashboardData.value("books").toList());
}
void PublisherMainWindow::onDashboardLoadFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void PublisherMainWindow::onAddNewBookButtonClicked() {
    PublisherAddBookWindow_c *addBookWindow = new PublisherAddBookWindow_c(networkManager);
    addBookWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(addBookWindow, &PublisherAddBookWindow_c::backRequested, this, [this, addBookWindow]() {
        addBookWindow->close();
        dashboardController->refreshDashboard();
        this->show();
    });
    showFollowingState(addBookWindow, this);
    this->hide();
}
void PublisherMainWindow::onEditBookButtonClicked() {
    int bookId = getSelectedBookId();
    if(bookId <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }
    PublisherAddBookWindow_c *editBookWindow = new PublisherAddBookWindow_c(networkManager, bookId);
    editBookWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(editBookWindow, &PublisherAddBookWindow_c::backRequested, this, [this, editBookWindow]() {
        editBookWindow->close();
        dashboardController->refreshDashboard();
        this->show();
    });
    showFollowingState(editBookWindow, this);
    this->hide();
}
void PublisherMainWindow::onToggleBookActiveButtonClicked() {
    int bookId = getSelectedBookId();
    if(bookId <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }

    bool currentlyActive = getSelectedBookActiveStatus();
    if(currentlyActive)
        bookController->deactivateBook(bookId);
    else
        bookController->reactivateBook(bookId);
}
void PublisherMainWindow::onBookDeactivated(const QString &message) { ui->statusLabel->setText(message); dashboardController->refreshDashboard(); }
void PublisherMainWindow::onBookDeactivateFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onBookReactivated(const QString &message) { ui->statusLabel->setText(message); dashboardController->refreshDashboard(); }
void PublisherMainWindow::onBookReactivateFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onValidationError(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onAccountInfoLoaded(const QVariantMap &accountData) {
    currentAccountData = accountData;
    ui->editAccountButton->setEnabled(true);
    ui->nameLabel->setText("نام و نام خانوادگی: " + accountData.value("firstName").toString() + " " + accountData.value("lastName").toString());
    ui->emailLabel->setText("ایمیل: " + accountData.value("email").toString());
    ui->publicationNameLabel->setText("نام انتشارات: " + accountData.value("publicationName").toString());
    ui->licenseNumberLabel->setText("شماره پروانه نشر: " + accountData.value("publisherLicenseNumber").toString());
}
void PublisherMainWindow::onAccountInfoLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onEditAccountButtonClicked() {
    if (currentAccountData.isEmpty()) {
        ui->statusLabel->setText("اطلاعات حساب هنوز بارگذاری نشده است");
        return;
    }
    RegisterWindow_c *editWindow = new RegisterWindow_c(
        networkManager, RegisterWindow_c::Mode::AccountEdit, currentAccountData);
    editWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(editWindow, &RegisterWindow_c::backToProfileRequested, editWindow, &QWidget::close);
    connect(editWindow, &QObject::destroyed, this, [this]() {
        profileController->loadAccountInfo();
        this->show();
    });
    showFollowingState(editWindow, this);
    this->hide();
}
void PublisherMainWindow::onNotificationsButtonClicked() {
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
void PublisherMainWindow::onUnreadCountLoaded(int count) {
    ui->notificationsButton->setText(QString("اعلان ها (%1)").arg(count));
}
void PublisherMainWindow::onUnreadCountLoadFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void PublisherMainWindow::onNewNotificationArrived(const QVariantMap &notificationData) {
    notificationController->refreshUnreadCount();
    statusBar()->showMessage(
        QString("اعلان جدید: %1").arg(notificationData.value("title").toString()), 5000);
}
void PublisherMainWindow::onNotificationMarkedRead(const QString &message) {
    Q_UNUSED(message)
    notificationController->refreshUnreadCount();
}
void PublisherMainWindow::onLogoutButtonClicked() {
    if(QMessageBox::question(this, "خروج از حساب کاربری", "آیا مطمئن هستید که می خواهید از حساب کاربری خود خارج شوید؟",
    QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes){
        networkManager->logout();
        emit logoutRequested();
    }
}