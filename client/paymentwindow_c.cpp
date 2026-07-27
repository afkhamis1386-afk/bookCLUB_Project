#include "paymentwindow_c.h"
#include "ui_paymentwindow_c.h"
#include <QRegularExpression>

PaymentWindow_c::PaymentWindow_c(double totalAmount, const QString &expectedCardHolderName, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PaymentWindow_c)
    , expectedCardHolderName(expectedCardHolderName.trimmed())
{
    ui->setupUi(this);
    setWindowTitle("پرداخت");
    ui->amountLabel->setText(QString("مبلغ قابل پرداخت: %1 تومان").arg(totalAmount, 0, 'f', 0));
    ui->cvvLineEdit->setEchoMode(QLineEdit::Password);
    connect(ui->payButton, &QPushButton::clicked, this, &PaymentWindow_c::onPayButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &PaymentWindow_c::onCancelButtonClicked);
    setFixedSize(480, 460);
}
PaymentWindow_c::~PaymentWindow_c()
{
    delete ui;
}
QString PaymentWindow_c::getCardNumber() const
{
    QString cardNumber = ui->cardNumberLineEdit->text();
    cardNumber.remove(' ');
    return cardNumber;
}
void PaymentWindow_c::onPayButtonClicked()
{
    ui->statusLabel->clear();
    QString cardNumber = getCardNumber();
    static const QRegularExpression cardRegex(R"(^\d{16}$)");
    if (!cardRegex.match(cardNumber).hasMatch()) {
        ui->statusLabel->setText("شماره کارت باید دقیقاً ۱۶ رقم باشد");
        return;
    }
    QString enteredCardHolder = ui->cardHolderLineEdit->text().trimmed();
    if(enteredCardHolder.isEmpty()){
        ui->statusLabel->setText("نام دارنده کارت را وارد کنید");
        return;
    }
    if(QString::compare(enteredCardHolder, expectedCardHolderName, Qt::CaseSensitive) != 0){
        ui->statusLabel->setText("نام دارنده کارت باید دقیقاً همان نام کاربری حساب شما باشد");
        return;
    }
    static const QRegularExpression expiryRegex(R"(^(\d{2})/(\d{2})$)");
    QRegularExpressionMatch expiryMatch = expiryRegex.match(ui->expiryLineEdit->text().trimmed());
    if(!expiryMatch.hasMatch()) {
        ui->statusLabel->setText("تاریخ انقضا را به صورت MM/YY وارد کنید");
        return;
    }
    int expiryMonth = expiryMatch.captured(1).toInt();
    int expiryYear = expiryMatch.captured(2).toInt();
    if(expiryMonth < 1 || expiryMonth > 12){
        ui->statusLabel->setText("ماه تاریخ انقضا باید بین ۰۱ تا ۱۲ باشد");
        return;
    }
    if(expiryYear <= 5){
        ui->statusLabel->setText("تاریخ انقضای کارت نامعتبر یا منقضی شده است (سال باید بزرگ تر از ۰۵ باشد)");
        return;
    }
    static const QRegularExpression cvvRegex(R"(^\d{3,4}$)");
    if(!cvvRegex.match(ui->cvvLineEdit->text().trimmed()).hasMatch()) {
        ui->statusLabel->setText("CVV2 نامعتبر است");
        return;
    }
    accept();
}
void PaymentWindow_c::onCancelButtonClicked()
{
    reject();
}