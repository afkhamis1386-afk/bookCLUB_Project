#include "profilewindow_c.h"
#include "ui_profilewindow_c.h"
#include "GenreSelectionWindow_c.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDateTime>
ProfileWindow_c::ProfileWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProfileWindow_c)
    , networkManager(networkManager)
    , profileController(new ProfileController(networkManager, this))
{
    ui->setupUi(this);

    QStringList headers = {"شماره سفارش", "تاریخ", "مبلغ نهایی", "وضعیت"};
    ui->ordersTableWidget->setColumnCount(4);
    ui->ordersTableWidget->setHorizontalHeaderLabels(headers);
    ui->ordersTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->ordersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->changePasswordButton, &QPushButton::clicked, this, &ProfileWindow_c::onChangePasswordButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &ProfileWindow_c::onEditGenresButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &ProfileWindow_c::onBackButtonClicked);
    connect(profileController, &ProfileController::accountInfoLoaded, this, &ProfileWindow_c::onAccountInfoLoaded);
    connect(profileController, &ProfileController::accountInfoLoadFailed, this, &ProfileWindow_c::onAccountInfoLoadFailed);
    connect(profileController, &ProfileController::orderHistoryLoaded, this, &ProfileWindow_c::onOrderHistoryLoaded);
    connect(profileController, &ProfileController::orderHistoryLoadFailed, this, &ProfileWindow_c::onOrderHistoryLoadFailed);
    connect(profileController, &ProfileController::passwordChanged, this, &ProfileWindow_c::onPasswordChanged);
    connect(profileController, &ProfileController::passwordChangeFailed, this, &ProfileWindow_c::onPasswordChangeFailed);
    connect(profileController, &ProfileController::validationError, this, &ProfileWindow_c::onValidationError);
    profileController->loadAccountInfo();
    profileController->loadOrderHistory();
}

ProfileWindow_c::~ProfileWindow_c()
{
    delete ui;
}

void ProfileWindow_c::onAccountInfoLoaded(const QVariantMap &accountData)
{
    ui->usernameLabel->setText("نام کاربری: " + accountData.value("username").toString());
    ui->registerDateLabel->setText("تاریخ عضویت: " + accountData.value("registerDate").toDateTime().toString("yyyy/MM/dd"));
    ui->purchasedCountLabel->setText("تعداد کتاب های خریداری شده: " + QString::number(accountData.value("purchasedCount").toInt()));
}

void ProfileWindow_c::onAccountInfoLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void ProfileWindow_c::onOrderHistoryLoaded(const QVariantList &orders)
{
    ui->ordersTableWidget->setRowCount(orders.size());
    for (int i = 0; i < orders.size(); ++i) {
        QVariantMap order = orders[i].toMap();
        ui->ordersTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(order.value("orderId").toInt())));
        ui->ordersTableWidget->setItem(i, 1, new QTableWidgetItem(order.value("orderDate").toDateTime().toString("yyyy/MM/dd hh:mm")));
        ui->ordersTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(order.value("finalPrice").toDouble(), 'f', 0) + " تومان"));
        ui->ordersTableWidget->setItem(i, 3, new QTableWidgetItem(order.value("status").toString()));
    }
}

void ProfileWindow_c::onOrderHistoryLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void ProfileWindow_c::onChangePasswordButtonClicked()
{
    profileController->changePassword(ui->oldPasswordLineEdit->text(), ui->newPasswordLineEdit->text());
}

void ProfileWindow_c::onPasswordChanged(const QString &message)
{
    ui->statusLabel->setText(message);
    ui->oldPasswordLineEdit->clear();
    ui->newPasswordLineEdit->clear();
}

void ProfileWindow_c::onPasswordChangeFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void ProfileWindow_c::onEditGenresButtonClicked()
{
    GenreSelectionWindow_c *genreWindow = new GenreSelectionWindow_c(networkManager);
    genreWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(genreWindow, &GenreSelectionWindow_c::genresConfirmed, genreWindow, &QWidget::close);
    genreWindow->show();
}

void ProfileWindow_c::onValidationError(const QString &message)
{
    ui->statusLabel->setText(message);
}

void ProfileWindow_c::onBackButtonClicked()
{
    emit backRequested();
}