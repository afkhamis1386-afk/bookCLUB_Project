#include "adminmainwindow.h"
#include "ui_adminmainwindow.h"
#include "createadmindialog.h"
#include "publisheraddbookwindow_c.h"
#include "windownav.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
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
    connect(ui->viewUserDetailsButton, &QPushButton::clicked, this, &AdminMainWindow::onViewUserDetailsButtonClicked);
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
    for (const QVariant &v : qAsConst(allUsersCache)) {
        QVariantMap u = v.toMap();
        const QString role = u.value("role").toString();
        if (roleFilterIndex == 1 && role != "NormalUser") continue;
        if (roleFilterIndex == 2 && role != "Publisher") continue;
        if (roleFilterIndex == 3 && role != "Admin") continue;
        if (!searchText.isEmpty()) {
            const QString username = u.value("username").toString();
            const QString firstName = u.value("firstName").toString();
            const QString lastName = u.value("lastName").toString();
            const QString publicationName = u.value("publicationName").toString();
            const bool matches = username.contains(searchText, Qt::CaseInsensitive)
                                 || firstName.contains(searchText, Qt::CaseInsensitive)
                                 || lastName.contains(searchText, Qt::CaseInsensitive)
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
void AdminMainWindow::onViewUserDetailsButtonClicked() {
    const int userId = getSelectedUserId();
    if (userId <= 0) {
        ui->statusLabel->setText("ابتدا یک کاربر را انتخاب کنید");
        return;
    }
    QVariantMap selectedUser;
    for (const QVariant &value : qAsConst(allUsersCache)) {
        const QVariantMap userData = value.toMap();
        if (userData.value("userId").toInt() == userId) {
            selectedUser = userData;
            break;
        }
    }
    if (selectedUser.isEmpty()) {
        ui->statusLabel->setText("اطلاعات کاربر انتخاب شده در دسترس نیست؛ فهرست را به روزرسانی کنید");
        return;
    }

    const auto displayValue = [](const QString &value) {
        return value.trimmed().isEmpty() ? QString("ثبت نشده") : value.trimmed();
    };

    const QString role = selectedUser.value("role").toString();
    const QString username = displayValue(selectedUser.value("username").toString());
    QString details;

    if (role == "NormalUser") {
        QStringList genreTitles;
        const QVariantList genres = selectedUser.value("favoriteGenreTitles").toList();
        for (const QVariant &genre : genres) {
            const QString title = genre.toString().trimmed();
            if (!title.isEmpty())
                genreTitles.append(title);
        }
        const QString registerDate = selectedUser.value("registerDate").toDateTime().toString("yyyy/MM/dd");
        const QString activeStatus = selectedUser.value("isActive").toBool() ? "فعال" : "غیرفعال";
        const QString blockedStatus = selectedUser.value("isBlocked").toBool() ? "مسدود" : "مسدود نیست";
        details = QString("نوع حساب: کاربر عادی\nنام کاربری: %1\nژانرهای مورد علاقه: %2\nتعداد کتاب‌های خریداری‌شده: %3\nتاریخ ثبت‌نام: %4\nوضعیت حساب: %5\nوضعیت دسترسی: %6")
                      .arg(username)
                      .arg(genreTitles.isEmpty() ? QString("انتخاب نشده") : genreTitles.join("، "))
                      .arg(selectedUser.value("purchasedCount").toInt())
                      .arg(registerDate.isEmpty() ? QString("ثبت نشده") : registerDate)
                      .arg(activeStatus)
                      .arg(blockedStatus);
    } else if (role == "Publisher") {
        details = QString("نوع حساب: ناشر\nنام کاربری: %1\nنام: %2\nنام خانوادگی: %3\nنام انتشارات: %4\nشماره پروانه نشر: %5\nایمیل: %6")
                      .arg(username)
                      .arg(displayValue(selectedUser.value("firstName").toString()))
                      .arg(displayValue(selectedUser.value("lastName").toString()))
                      .arg(displayValue(selectedUser.value("publicationName").toString()))
                      .arg(displayValue(selectedUser.value("publisherLicenseNumber").toString()))
                      .arg(displayValue(selectedUser.value("email").toString()));
    } else if (role == "Admin") {
        details = QString("نوع حساب: ادمین\nنام کاربری: %1\nنام: %2\nنام خانوادگی: %3")
                      .arg(username)
                      .arg(displayValue(selectedUser.value("firstName").toString()))
                      .arg(displayValue(selectedUser.value("lastName").toString()));
    } else {
        ui->statusLabel->setText("نقش کاربر انتخاب شده نامعتبر است");
        return;
    }

    QMessageBox::information(this, "اطلاعات کامل کاربر", details);
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
    const QString price = QString::number(bookData.value("price").toDouble(), 'f', 2);
    const QString details = QString("نام کتاب: %1\nنام نویسنده: %2\nقیمت کتاب: %3\nنوع ژانر: %4\nتوضیحات: %5")
                                .arg(bookData.value("bookName").toString())
                                .arg(bookData.value("authorName").toString())
                                .arg(price)
                                .arg(bookData.value("genreTitle").toString())
                                .arg(bookData.value("description").toString());
    QMessageBox::information(this, "جزئیات کامل کتاب", details);
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
    const int bookId = getSelectedBookId();
    if (bookId <= 0) {
        ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید");
        return;
    }

    auto *editBookWindow = new PublisherAddBookWindow_c(networkManager, bookId);
    editBookWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(editBookWindow, &PublisherAddBookWindow_c::backRequested, this, [this, editBookWindow]() {
        editBookWindow->close();
        adminController->loadAllBooks();
        this->show();
    });
    showFollowingState(editBookWindow, this);
    this->hide();
}
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
                              "آیا مطمئن هستید که می خواهید از حساب کاربری خود خارج شوید؟", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        networkManager->logout();
        emit logoutRequested();
    }
}
void AdminMainWindow::onCreateAdminButtonClicked() {
    CreateAdminDialog dialog(networkManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        adminController->loadAllUsers();
    }
}