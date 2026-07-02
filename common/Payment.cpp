#include "payment.h"
Payment::Payment()
    : paymentId(-1)
    , orderId(-1)
    , amount(0)
    , transactionCode(QString())
    , paymentDate(QDateTime::currentDateTime())
    , paymentStatusId(static_cast<int>(PaymentStatus::Pending)) {}
Payment::Payment(int orderId, double amount, const QString& transactionCode)
    : paymentId(-1)
    , orderId(orderId)
    , paymentDate(QDateTime::currentDateTime())
    , paymentStatusId(static_cast<int>(PaymentStatus::Pending)) {
    if (!setAmount(amount)) {
        this->amount = 0.0;
    }
    if (!setTransactionCode(transactionCode)) {
        this->transactionCode = "";
    }
}
int Payment::getPaymentId() const { return paymentId; }
int Payment::getOrderId() const { return orderId; }
double Payment::getAmount() const { return amount; }
QString Payment::getTransactionCode() const { return transactionCode; }
QDateTime Payment::getPaymentDate() const { return paymentDate; }
int Payment::getPaymentStatusId() const { return paymentStatusId; }
void Payment::setPaymentId(int id) { paymentId = id; }
void Payment::setOrderId(int id) { orderId = id; }
bool Payment::setAmount(double amt) {
    if (amt <0)
        return false;
    amount = amt;
    return true;
}
bool Payment::setTransactionCode(const QString &code) {
    if (code.length() > 100)
        return false;
    transactionCode = code;
    return true;
}
void Payment::setPaymentDate(const QDateTime& date) { paymentDate = date; }
void Payment::setPaymentStatusId(int statusId) { paymentStatusId = statusId; }
QString Payment::getStatusTitle() const {
    switch (paymentStatusId) {
    case static_cast<int>(PaymentStatus::Pending):
        return "Pending";
    case static_cast<int>(PaymentStatus::Successful):
        return "Successful";
    case static_cast<int>(PaymentStatus::Failed):
        return "Failed";
    default:
        return "Unknown";
    }
}
QDataStream& operator<<(QDataStream& out, const Payment& payment) {
    out << payment.paymentId
        << payment.orderId
        << payment.amount
        << payment.transactionCode
        << payment.paymentDate
        << payment.paymentStatusId;
    return out;
}
QDataStream& operator>>(QDataStream& in, Payment& payment) {
    int paymentId, orderId, paymentStatusId;
    double amount;
    QString transactionCode;
    QDateTime paymentDate;
    in >> paymentId >> orderId >> amount >> transactionCode >> paymentDate >> paymentStatusId;
    if (in.status() == QDataStream::Ok) {
        payment.paymentId = paymentId;
        payment.orderId = orderId;
        payment.amount = amount;
        payment.transactionCode = transactionCode;
        payment.paymentDate = paymentDate;
        payment.paymentStatusId = paymentStatusId;
    }
    return in;
}