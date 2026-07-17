#include "publishermainwindow.h"
#include "publisheraddbookwindow_c.h"
#include "ui_publishermainwindow.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>

PublisherMainWindow::PublisherMainWindow(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PublisherMainWindow)
    , networkManager(networkManager)
    , dashboardController(new PublisherDashboardController(networkManager, this))
    , bookController(new PublisherBookController(networkManager, this))
    , profileController(new ProfileController(networkManager, this))
{
    ui->setupUi(this);
    ui->myBooksTableWidget->setColumnCount(5);
    ui->myBooksTableWidget->setHorizontalHeaderLabels({"شناسه", "نام کتاب", "میانگین امتیاز", "تعداد فروش", "وضعیت"});
    ui->myBooksTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->myBooksTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->myBooksTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->refreshDashboardButton, &QPushButton::clicked, dashboardController, &PublisherDashboardController::refreshDashboard);
    connect(ui->addNewBookButton, &QPushButton::clicked, this, &PublisherMainWindow::onAddNewBookButtonClicked);
    connect(ui->editBookButton, &QPushButton::clicked, this, &PublisherMainWindow::onEditBookButtonClicked);
    connect(ui->applyDiscountButton, &QPushButton::clicked, this, &PublisherMainWindow::onApplyDiscountButtonClicked);
    connect(ui->toggleBookActiveButton, &QPushButton::clicked, this, &PublisherMainWindow::onToggleBookActiveButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &PublisherMainWindow::onLogoutButtonClicked);
    connect(dashboardController, &PublisherDashboardController::dashboardLoaded, this, &PublisherMainWindow::onDashboardLoaded);
    connect(dashboardController, &PublisherDashboardController::dashboardLoadFailed, this, &PublisherMainWindow::onDashboardLoadFailed);
    connect(bookController, &PublisherBookController::bookUpdated, this, &PublisherMainWindow::onBookUpdated);
    connect(bookController, &PublisherBookController::bookUpdateFailed, this, &PublisherMainWindow::onBookUpdateFailed);
    connect(bookController, &PublisherBookController::discountApplied, this, &PublisherMainWindow::onDiscountApplied);
    connect(bookController, &PublisherBookController::discountApplyFailed, this, &PublisherMainWindow::onDiscountApplyFailed);
    connect(bookController, &PublisherBookController::bookDeactivated, this, &PublisherMainWindow::onBookDeactivated);
    connect(bookController, &PublisherBookController::bookDeactivateFailed, this, &PublisherMainWindow::onBookDeactivateFailed);
    connect(bookController, &PublisherBookController::bookReactivated, this, &PublisherMainWindow::onBookReactivated);
    connect(bookController, &PublisherBookController::bookReactivateFailed, this, &PublisherMainWindow::onBookReactivateFailed);
    connect(bookController, &PublisherBookController::validationError, this, &PublisherMainWindow::onValidationError);
    connect(profileController, &ProfileController::accountInfoLoaded, this, &PublisherMainWindow::onAccountInfoLoaded);
    connect(profileController, &ProfileController::accountInfoLoadFailed, this, &PublisherMainWindow::onAccountInfoLoadFailed);

    dashboardController->refreshDashboard();
    profileController->loadAccountInfo();
}

PublisherMainWindow::~PublisherMainWindow()
{
    delete ui;
}

void PublisherMainWindow::populateBooksTable(const QVariantList &books)
{
    currentBooks = books;
    ui->myBooksTableWidget->setRowCount(books.size());
    for (int i = 0; i < books.size(); ++i) {
        QVariantMap b = books[i].toMap();
        ui->myBooksTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(b.value("bookId").toInt())));
        ui->myBooksTableWidget->setItem(i, 1, new QTableWidgetItem(b.value("bookName").toString()));
        ui->myBooksTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(b.value("averageRating").toDouble(), 'f', 1)));
        ui->myBooksTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(b.value("soldCount").toInt())));
        ui->myBooksTableWidget->setItem(i, 4, new QTableWidgetItem(b.value("isActive").toBool() ? "فعال" : "غیرفعال"));
    }
}

int PublisherMainWindow::getSelectedBookId() const
{
    int row = ui->myBooksTableWidget->currentRow();
    if (row < 0) return -1;
    return ui->myBooksTableWidget->item(row, 0)->text().toInt();
}

bool PublisherMainWindow::getSelectedBookActiveStatus() const
{
    int row = ui->myBooksTableWidget->currentRow();
    if (row < 0) return false;
    return ui->myBooksTableWidget->item(row, 4)->text() == "فعال";
}

void PublisherMainWindow::onDashboardLoaded(const QVariantMap &dashboardData)
{
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

void PublisherMainWindow::onDashboardLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void PublisherMainWindow::onAddNewBookButtonClicked()
{
    PublisherAddBookWindow_c *addBookWindow = new PublisherAddBookWindow_c(networkManager);
    addBookWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(addBookWindow, &PublisherAddBookWindow_c::backRequested, this, [this, addBookWindow]() {
        addBookWindow->close();
        dashboardController->refreshDashboard();
        this->show();
    });
    addBookWindow->show();
    this->hide();
}
void PublisherMainWindow::onEditBookButtonClicked()
{
    int bookId = getSelectedBookId();
    if(bookId <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }

    bool ok;
    QString newName = QInputDialog::getText(this, "ویرایش کتاب", "نام جدید کتاب:", QLineEdit::Normal, "", &ok);
    if(!ok || newName.trimmed().isEmpty()) return;

    QString newDescription = QInputDialog::getMultiLineText(this, "ویرایش کتاب", "توضیحات جدید:", "", &ok);
    if(!ok) return;

    double newPrice = QInputDialog::getDouble(this, "ویرایش کتاب", "قیمت جدید:", 0, 0, 100000000, 0, &ok);
    if(!ok) return;

    bookController->updateBook(bookId, newName, newDescription, newPrice);
}

void PublisherMainWindow::onApplyDiscountButtonClicked()
{
    int bookId = getSelectedBookId();
    if(bookId <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }

    bool ok;
    double percent = QInputDialog::getDouble(this, "اعمال تخفیف", "درصد تخفیف (۰ اگر مبلغی است):", 0, 0, 100, 0, &ok);
    if(!ok) return;
    double amount = QInputDialog::getDouble(this, "اعمال تخفیف", "مبلغ تخفیف (۰ اگر درصدی است):", 0, 0, 100000000, 0, &ok);
    if(!ok) return;

    bookController->applyDiscount(bookId, percent, amount);
}

void PublisherMainWindow::onToggleBookActiveButtonClicked()
{
    int bookId = getSelectedBookId();
    if(bookId <= 0) { ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید"); return; }

    bool currentlyActive = getSelectedBookActiveStatus();
    if(currentlyActive)
        bookController->deactivateBook(bookId);
    else
        bookController->reactivateBook(bookId);
}

void PublisherMainWindow::onBookUpdated(const QString &message) { ui->statusLabel->setText(message); dashboardController->refreshDashboard(); }
void PublisherMainWindow::onBookUpdateFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onDiscountApplied(const QString &message) { ui->statusLabel->setText(message); dashboardController->refreshDashboard(); }
void PublisherMainWindow::onDiscountApplyFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onBookDeactivated(const QString &message) { ui->statusLabel->setText(message); dashboardController->refreshDashboard(); }
void PublisherMainWindow::onBookDeactivateFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onBookReactivated(const QString &message) { ui->statusLabel->setText(message); dashboardController->refreshDashboard(); }
void PublisherMainWindow::onBookReactivateFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onValidationError(const QString &message) { ui->statusLabel->setText(message); }
void PublisherMainWindow::onAccountInfoLoaded(const QVariantMap &accountData)
{
    ui->nameLabel->setText("نام و نام خانوادگی: " + accountData.value("firstName").toString() + " " + accountData.value("lastName").toString());
    ui->emailLabel->setText("ایمیل: " + accountData.value("email").toString());
    ui->publicationNameLabel->setText("نام انتشارات: " + accountData.value("publicationName").toString());
    ui->licenseNumberLabel->setText("شماره پروانه نشر: " + accountData.value("publisherLicenseNumber").toString());
}
void PublisherMainWindow::onAccountInfoLoadFailed(const QString &message) { ui->statusLabel->setText(message); }

void PublisherMainWindow::onLogoutButtonClicked() { emit logoutRequested(); }