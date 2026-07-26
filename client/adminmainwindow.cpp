#include "adminmainwindow.h"
#include "ui_adminmainwindow.h"
#include "createadmindialog.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QInputDialog>
#include <QLineEdit>
#include <QComboBox>
AdminMainWindow::AdminMainWindow(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminMainWindow)
    , networkManager(networkManager)
    , adminController(new AdminController(networkManager, this)) {
    ui->setupUi(this);
    ui->usersTableWidget->setColumnCount(6);
    ui->usersTableWidget->setHorizontalHeaderLabels({"شناسه", "نام کاربری", "نقش", "مسدود", "فعال", "تاریخ عضویت"});
    ui->usersTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->usersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->usersTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->booksTableWidget->setColumnCount(5);
    ui->booksTableWidget->setHorizontalHeaderLabels({"شناسه", "نام کتاب", "شناسه ناشر", "فعال", "حذف‌شده"});
    ui->booksTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->booksTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->booksTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->reviewsTableWidget->setColumnCount(5);
    ui->reviewsTableWidget->setHorizontalHeaderLabels({"شناسه", "کاربر", "کتاب", "متن نظر", "وضعیت"});
    ui->reviewsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->reviewsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->reviewsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->refreshUsersButton, &QPushButton::clicked, adminController, &AdminController::loadAllUsers);
    connect(ui->blockUserButton, &QPushButton::clicked, this, &AdminMainWindow::onBlockUserButtonClicked);
    connect(ui->unblockUserButton, &QPushButton::clicked, this, &AdminMainWindow::onUnblockUserButtonClicked);
    connect(ui->toggleActiveButton, &QPushButton::clicked, this, &AdminMainWindow::onToggleActiveButtonClicked);
    connect(ui->deleteUserButton, &QPushButton::clicked, this, &AdminMainWindow::onDeleteUserButtonClicked);
    connect(ui->refreshBooksButton, &QPushButton::clicked, adminController, &AdminController::loadAllBooks);
    connect(ui->viewBookDetailsButton, &QPushButton::clicked, this, &AdminMainWindow::onViewBookDetailsButtonClicked);
    connect(ui->deleteBookButton, &QPushButton::clicked, this, &AdminMainWindow::onDeleteBookButtonClicked);
    connect(ui->editBookButton, &QPushButton::clicked, this, &AdminMainWindow::onEditBookButtonClicked);
    connect(ui->refreshReviewsButton, &QPushButton::clicked, adminController, &AdminController::loadAllReviews);
    connect(ui->deleteReviewButton, &QPushButton::clicked, this, &AdminMainWindow::onDeleteReviewButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminMainWindow::onLogoutButtonClicked);
    connect(adminController, &AdminController::usersLoaded, this, &AdminMainWindow::onUsersLoaded);
    connect(adminController, &AdminController::usersLoadFailed, this, &AdminMainWindow::onUsersLoadFailed);
    connect(adminController, &AdminController::userBlocked, this, &AdminMainWindow::onUserBlocked);
    connect(adminController, &AdminController::userBlockFailed, this, &AdminMainWindow::onUserBlockFailed);
    connect(adminController, &AdminController::userUnblocked, this, &AdminMainWindow::onUserUnblocked);
    connect(adminController, &AdminController::userUnblockFailed, this, &AdminMainWindow::onUserUnblockFailed);
    connect(adminController, &AdminController::userActiveStatusChanged, this, &AdminMainWindow::onUserActiveStatusChanged);
    connect(adminController, &AdminController::userActiveStatusChangeFailed, this, &AdminMainWindow::onUserActiveStatusChangeFailed);
    connect(adminController, &AdminController::userDeleted, this, &AdminMainWindow::onUserDeleted);
    connect(adminController, &AdminController::userDeleteFailed, this, &AdminMainWindow::onUserDeleteFailed);
    connect(adminController, &AdminController::allBooksLoaded, this, &AdminMainWindow::onAllBooksLoaded);
    connect(adminController, &AdminController::allBooksLoadFailed, this, &AdminMainWindow::onAllBooksLoadFailed);
    connect(adminController, &AdminController::bookDetailsForReviewLoaded, this, &AdminMainWindow::onBookDetailsForReviewLoaded);
    connect(adminController, &AdminController::bookDetailsForReviewLoadFailed, this, &AdminMainWindow::onBookDetailsForReviewLoadFailed);
    connect(adminController, &AdminController::bookDeleted, this, &AdminMainWindow::onBookDeleted);
    connect(adminController, &AdminController::bookDeleteFailed, this, &AdminMainWindow::onBookDeleteFailed);
    connect(adminController, &AdminController::bookUpdated, this, &AdminMainWindow::onBookUpdated);
    connect(adminController, &AdminController::bookUpdateFailed, this, &AdminMainWindow::onBookUpdateFailed);
    connect(adminController, &AdminController::allReviewsLoaded, this, &AdminMainWindow::onAllReviewsLoaded);
    connect(adminController, &AdminController::allReviewsLoadFailed, this, &AdminMainWindow::onAllReviewsLoadFailed);
    connect(adminController, &AdminController::reviewDeleted, this, &AdminMainWindow::onReviewDeleted);
    connect(adminController, &AdminController::reviewDeleteFailed, this, &AdminMainWindow::onReviewDeleteFailed);
    connect(adminController, &AdminController::validationError, this, &AdminMainWindow::onValidationError);
    connect(ui->createAdminButton, &QPushButton::clicked, this, &AdminMainWindow::onCreateAdminButtonClicked);
    connect(ui->userSearchLineEdit, &QLineEdit::textChanged, this, &AdminMainWindow::onUserSearchOrFilterChanged);
    connect(ui->userRoleFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminMainWindow::onUserSearchOrFilterChanged);
    adminController->loadAllUsers();
    adminController->loadAllBooks();
    adminController->loadAllReviews();
}
AdminMainWindow::~AdminMainWindow() {
    delete ui;
}
int AdminMainWindow::getSelectedUserId() const {
    int row = ui->usersTableWidget->currentRow();
    if (row < 0) return -1;
    return ui->usersTableWidget->item(row, 0)->text().toInt();
}
int AdminMainWindow::getSelectedBookId() const {
    int row = ui->booksTableWidget->currentRow();
    if (row < 0) return -1;
    return ui->booksTableWidget->item(row, 0)->text().toInt();
}
int AdminMainWindow::getSelectedReviewId() const {
    int row = ui->reviewsTableWidget->currentRow();
    if (row < 0) return -1;
    return ui->reviewsTableWidget->item(row, 0)->text().toInt();
}
void AdminMainWindow::onUsersLoaded(const QVariantList &users) {
    allUsersCache = users;
    applyUserSearchAndFilter();
}
void AdminMainWindow::renderUsersTable(const QVariantList &users) {
    ui->usersTableWidget->setRowCount(users.size());
    for (int i = 0; i < users.size(); ++i) {
        QVariantMap u = users[i].toMap();
        ui->usersTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(u.value("userId").toInt())));
        ui->usersTableWidget->setItem(i, 1, new QTableWidgetItem(u.value("username").toString()));
        ui->usersTableWidget->setItem(i, 2, new QTableWidgetItem(u.value("role").toString()));
        ui->usersTableWidget->setItem(i, 3, new QTableWidgetItem(u.value("isBlocked").toBool() ? "بله" : "خیر"));
        ui->usersTableWidget->setItem(i, 4, new QTableWidgetItem(u.value("isActive").toBool() ? "فعال" : "غیرفعال"));
        ui->usersTableWidget->setItem(i, 5, new QTableWidgetItem(u.value("registerDate").toDateTime().toString("yyyy/MM/dd")));
    }
}
void AdminMainWindow::applyUserSearchAndFilter() {
    const QString searchText = ui->userSearchLineEdit->text().trimmed();
    const int roleFilterIndex = ui->userRoleFilterComboBox->currentIndex();
    QVariantList filtered;
    for (const QVariant &v : allUsersCache) {
        QVariantMap u = v.toMap();
        const QString role = u.value("role").toString();
        if (roleFilterIndex == 1 && role != "NormalUser") continue;
        if (roleFilterIndex == 2 && role != "Publisher") continue;
        if (roleFilterIndex == 3 && role != "Admin") continue;
        if (!searchText.isEmpty()) {
            const QString username = u.value("username").toString();
            const QString publicationName = u.value("publicationName").toString();
            const bool matches = username.contains(searchText, Qt::CaseInsensitive)
                                 || publicationName.contains(searchText, Qt::CaseInsensitive);
            if (!matches) continue;
        }
        filtered.append(u);
    }
    renderUsersTable(filtered);
}
void AdminMainWindow::onUserSearchOrFilterChanged() {
    applyUserSearchAndFilter();
}
void AdminMainWindow::onUsersLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onBlockUserButtonClicked() {
    int id = getSelectedUserId();
    if (id <= 0) { ui->statusLabel->setText("ابتدا یک کاربر را انتخاب کنید"); return; }
    adminController->blockUser(id);
}
void AdminMainWindow::onUnblockUserButtonClicked() {
    int id = getSelectedUserId();
    if (id <= 0) { ui->statusLabel->setText("ابتدا یک کاربر را انتخاب کنید"); return; }
    adminController->unblockUser(id);
}
void AdminMainWindow::onToggleActiveButtonClicked() {
    int id = getSelectedUserId();
    if (id <= 0) { ui->statusLabel->setText("ابتدا یک کاربر را انتخاب کنید"); return; }
    int row = ui->usersTableWidget->currentRow();
    bool currentlyActive = ui->usersTableWidget->item(row, 4)->text() == "فعال";
    adminController->setUserActiveStatus(id, !currentlyActive);
}
void AdminMainWindow::onDeleteUserButtonClicked() {
    int id = getSelectedUserId();
    if (id <= 0) { ui->statusLabel->setText("ابتدا یک کاربر را انتخاب کنید"); return; }
    if (QMessageBox::question(this, "حذف کاربر", "مطمئن هستید؟") == QMessageBox::Yes)
        adminController->deleteUser(id);
}
void AdminMainWindow::onUserBlocked(const QString &message) { ui->statusLabel->setText(message); adminController->loadAllUsers(); }
void AdminMainWindow::onUserBlockFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onUserUnblocked(const QString &message) { ui->statusLabel->setText(message); adminController->loadAllUsers(); }
void AdminMainWindow::onUserUnblockFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onUserActiveStatusChanged(const QString &message) { ui->statusLabel->setText(message); adminController->loadAllUsers(); }
void AdminMainWindow::onUserActiveStatusChangeFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onUserDeleted(const QString &message) { ui->statusLabel->setText(message); adminController->loadAllUsers(); }
void AdminMainWindow::onUserDeleteFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onAllBooksLoaded(const QVariantList &books) {
    ui->booksTableWidget->setRowCount(books.size());
    for (int i = 0; i < books.size(); ++i) {
        QVariantMap b = books[i].toMap();
        ui->booksTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(b.value("bookId").toInt())));
        ui->booksTableWidget->setItem(i, 1, new QTableWidgetItem(b.value("bookName").toString()));
        ui->booksTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(b.value("publisherUserId").toInt())));
        ui->booksTableWidget->setItem(i, 3, new QTableWidgetItem(b.value("isActive").toBool() ? "بله" : "خیر"));
        ui->booksTableWidget->setItem(i, 4, new QTableWidgetItem(b.value("isDeleted").toBool() ? "بله" : "خیر"));
    }
}
void AdminMainWindow::onAllBooksLoadFailed(const QString &message) { ui->statusLabel->setText(message); }

void AdminMainWindow::onViewBookDetailsButtonClicked() {
    int id = getSelectedBookId();
    if (id <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }
    adminController->loadBookDetailsForReview(id);
}
void AdminMainWindow::onBookDetailsForReviewLoaded(const QVariantMap &bookData) {
    QMessageBox::information(this, "جزئیات کتاب", QString("نام: %1\nتوضیحات: %2\nقیمت: %3")
                                                      .arg(bookData.value("bookName").toString())
                                                      .arg(bookData.value("description").toString())
                                                      .arg(bookData.value("price").toDouble()));
}
void AdminMainWindow::onBookDetailsForReviewLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onDeleteBookButtonClicked() {
    int id = getSelectedBookId();
    if (id <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }
    if (QMessageBox::question(this, "حذف کتاب", "مطمئن هستید؟") == QMessageBox::Yes)
        adminController->deleteBook(id);
}
void AdminMainWindow::onBookDeleted(const QString &message) { ui->statusLabel->setText(message); adminController->loadAllBooks(); }
void AdminMainWindow::onEditBookButtonClicked() {
    int id = getSelectedBookId();
    if (id <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }
    int row = ui->booksTableWidget->currentRow();
    QString currentName = (row >= 0 && ui->booksTableWidget->item(row, 1))
                              ? ui->booksTableWidget->item(row, 1)->text() : QString();
    bool ok;
    QString newName = QInputDialog::getText(this, "ویرایش کتاب", "نام جدید کتاب:", QLineEdit::Normal, currentName, &ok);
    if (!ok) return;
    QString newDescription = QInputDialog::getMultiLineText(this, "ویرایش کتاب", "توضیحات جدید:", "", &ok);
    if (!ok) return;
    double newPrice = QInputDialog::getDouble(this, "ویرایش کتاب", "قیمت جدید:", 0, 0, 100000000, 0, &ok);
    if (!ok) return;
    adminController->updateBook(id, newName, newDescription, newPrice);
}
void AdminMainWindow::onBookUpdated(const QString &message) { ui->statusLabel->setText(message); adminController->loadAllBooks(); }
void AdminMainWindow::onBookUpdateFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onBookDeleteFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onAllReviewsLoaded(const QVariantList &reviews) {
    ui->reviewsTableWidget->setRowCount(reviews.size());
    for (int i = 0; i < reviews.size(); ++i) {
        QVariantMap r = reviews[i].toMap();
        ui->reviewsTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(r.value("reviewId").toInt())));
        ui->reviewsTableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(r.value("userId").toInt())));
        ui->reviewsTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(r.value("bookId").toInt())));
        ui->reviewsTableWidget->setItem(i, 3, new QTableWidgetItem(r.value("commentText").toString()));
        ui->reviewsTableWidget->setItem(i, 4, new QTableWidgetItem(r.value("isDeleted").toBool() ? "حذف شده" : "فعال"));
    }
}
void AdminMainWindow::onAllReviewsLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onDeleteReviewButtonClicked() {
    int id = getSelectedReviewId();
    if (id <= 0) {
        ui->statusLabel->setText("ابتدا یک نظر را انتخاب کنید");
        return;
    }
    int row = ui->reviewsTableWidget->currentRow();
    if (row >= 0 && ui->reviewsTableWidget->item(row, 4)
        && ui->reviewsTableWidget->item(row, 4)->text() == "حذف شده") {
        ui->statusLabel->setText("این نظر قبلاً حذف شده است");
        return;
    }
    if (QMessageBox::question(this, "حذف نظر", "مطمئن هستید؟") == QMessageBox::Yes) {
        adminController->deleteReview(id);
    }
}
void AdminMainWindow::onReviewDeleted(const QString &message) { ui->statusLabel->setText(message); adminController->loadAllReviews(); }
void AdminMainWindow::onReviewDeleteFailed(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onValidationError(const QString &message) { ui->statusLabel->setText(message); }
void AdminMainWindow::onLogoutButtonClicked() {
    if (QMessageBox::question(this, "خروج از حساب کاربری",
                              "آیا مطمئن هستید که می خواهید از حساب کاربری خود خارج شوید؟",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        networkManager->logout();
        emit logoutRequested();
    }
}
void AdminMainWindow::onCreateAdminButtonClicked() {
    CreateAdminDialog dialog(networkManager, this);
    dialog.exec();
}