#include "cartwindow_c.h"
#include "ui_cartwindow_c.h"
#include "paymentwindow_c.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
CartWindow_c::CartWindow_c(NetworkManager *networkManager, QWidget *parent):QMainWindow(parent)
    , ui(new Ui::CartWindow_c)
    , networkManager(networkManager)
    , cartController(new CartController(networkManager, this)) {
    ui->setupUi(this);
    QStringList headers = {"نام کتاب", "قیمت", "تخفیف", "قیمت نهایی"};
    ui->cartTableWidget->setColumnCount(4);
    ui->cartTableWidget->setHorizontalHeaderLabels(headers);
    ui->cartTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->cartTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cartTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->removeSelectedButton, &QPushButton::clicked, this, &CartWindow_c::onRemoveSelectedButtonClicked);
    connect(ui->checkoutButton, &QPushButton::clicked, this, &CartWindow_c::onCheckoutButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &CartWindow_c::onBackButtonClicked);
    connect(cartController, &CartController::cartLoaded, this, &CartWindow_c::onCartLoaded);
    connect(cartController, &CartController::cartLoadFailed, this, &CartWindow_c::onCartLoadFailed);
    connect(cartController, &CartController::removeFromCartSucceeded, this, &CartWindow_c::onRemoveSucceeded);
    connect(cartController, &CartController::removeFromCartFailed, this, &CartWindow_c::onRemoveFailed);
    connect(cartController, &CartController::checkoutSucceeded, this, &CartWindow_c::onCheckoutSucceeded);
    connect(cartController, &CartController::checkoutFailed, this, &CartWindow_c::onCheckoutFailed);
    cartController->refreshCart(); }
CartWindow_c::~CartWindow_c() {
    delete ui; }
void CartWindow_c::populateTable(const QVariantList &items) {
    ui->cartTableWidget->setRowCount(items.size());
    for (int i = 0; i < items.size(); ++i) {
        QVariantMap item = items[i].toMap();
        int bookId = item.value("bookId").toInt();
        QString bookName = item.value("bookName").toString();
        double price = item.value("price").toDouble();
        double discountPercent = item.value("discountPercent").toDouble();
        double finalPrice = item.value("finalPrice").toDouble();
        QTableWidgetItem *bookItem = new QTableWidgetItem(bookName.isEmpty() ? QString("کتاب نامشخص (شناسه %1)").arg(bookId) : bookName);
        bookItem->setData(Qt::UserRole, bookId);
        ui->cartTableWidget->setItem(i, 0, bookItem);
        ui->cartTableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(price, 'f', 0)));
        ui->cartTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(discountPercent, 'f', 0) + "%"));
        ui->cartTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(finalPrice, 'f', 0)));
    }
}
void CartWindow_c::onCartLoaded(const QVariantMap &cartData) {
    ui->statusLabel->clear();
    populateTable(cartData.value("items").toList());
    double totalPrice = cartData.value("totalPrice").toDouble();
    double totalDiscount = cartData.value("totalDiscount").toDouble();
    double finalPrice = cartData.value("finalPrice").toDouble();
    currentFinalPrice = finalPrice;
    ui->totalPriceLabel->setText(QString("جمع کل: %1 تومان").arg(totalPrice, 0, 'f', 0));
    ui->discountLabel->setText(QString("تخفیف: %1 تومان").arg(totalDiscount, 0, 'f', 0));
    ui->finalPriceLabel->setText(QString("مبلغ قابل پرداخت: %1 تومان").arg(finalPrice, 0, 'f', 0));
    ui->checkoutButton->setEnabled(cartData.value("itemCount").toInt() > 0); }
void CartWindow_c::onCartLoadFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void CartWindow_c::onRemoveSelectedButtonClicked() {
    int row = ui->cartTableWidget->currentRow();
    if (row < 0) {
        ui->statusLabel->setText("ابتدا یک آیتم را انتخاب کنید");
        return;
    }
    int bookId = ui->cartTableWidget->item(row, 0)->data(Qt::UserRole).toInt();
    cartController->removeBook(bookId);
}
void CartWindow_c::onRemoveSucceeded(const QString &message) {
    ui->statusLabel->setText(message);
    cartController->refreshCart();
}
void CartWindow_c::onRemoveFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void CartWindow_c::onCheckoutButtonClicked() {
    PaymentWindow_c paymentDialog(currentFinalPrice, networkManager->getCurrentUsername(), this);
    if (paymentDialog.exec() == QDialog::Accepted) {
        cartController->checkout(paymentDialog.getCardNumber());
    }
}
void CartWindow_c::onCheckoutSucceeded(const QVariantMap &orderData) {
    double finalPrice = orderData.value("finalPrice").toDouble();
    QString transactionCode = orderData.value("transactionCode").toString();
    QString cardLastFour = orderData.value("cardLastFour").toString();
    QMessageBox::information(this, "خرید موفق", QString("خرید شما با موفقیت ثبت شد!\nمبلغ پرداخت شده: %1 تومان\nکارت: **** **** **** %2\nکد پیگیری: %3\nکتاب ها به کتابخانه ی شما اضافه شدند.").arg(finalPrice, 0, 'f', 0).arg(cardLastFour, transactionCode));
    cartController->refreshCart();
}
void CartWindow_c::onCheckoutFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void CartWindow_c::onBackButtonClicked() {
    emit backRequested();
}
