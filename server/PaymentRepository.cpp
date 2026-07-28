#include "PaymentRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

PaymentRepository::PaymentRepository() {}
int PaymentRepository::insertPayment(const Payment &payment) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Payments (OrderID, Amount, TransactionCode, PaymentDate, PaymentStatusID) "
        "OUTPUT INSERTED.PaymentID "
        "VALUES (:orderId, :amount, :transactionCode, :paymentDate, :statusId)" );
    query.bindValue(":orderId", payment.getOrderId());
    query.bindValue(":amount", payment.getAmount());
    if (payment.getTransactionCode().isEmpty())
        query.bindValue(":transactionCode", QVariant(QMetaType(QMetaType::QString)));
    else
        query.bindValue(":transactionCode", payment.getTransactionCode());
    query.bindValue(":paymentDate", payment.getPaymentDate());
    query.bindValue(":statusId", payment.getPaymentStatusId());
    if (!query.exec() || !query.next()) {
        qWarning() << "خطا در ثبت پرداخت:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Payment* PaymentRepository::loadPaymentById(int paymentId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT PaymentID, OrderID, Amount, TransactionCode, PaymentDate, PaymentStatusID "
        "FROM Payments WHERE PaymentID = :paymentId"
        );
    query.bindValue(":paymentId", paymentId);
    if (!query.exec() || !query.next()) {
        qWarning() << "پرداخت یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    Payment *payment = new Payment();
    payment->setPaymentId(query.value(0).toInt());
    payment->setOrderId(query.value(1).toInt());
    payment->setAmount(query.value(2).toDouble());
    payment->setTransactionCode(query.value(3).toString());
    payment->setPaymentDate(query.value(4).toDateTime());
    payment->setPaymentStatusId(query.value(5).toInt());
    return payment;
}
Payment* PaymentRepository::loadPaymentByOrderId(int orderId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT PaymentID, OrderID, Amount, TransactionCode, PaymentDate, PaymentStatusID "
        "FROM Payments WHERE OrderID = :orderId" );
    query.bindValue(":orderId", orderId);
    if (!query.exec() || !query.next()) {
        qWarning() << "پرداخت برای این سفارش یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    Payment *payment = new Payment();
    payment->setPaymentId(query.value(0).toInt());
    payment->setOrderId(query.value(1).toInt());
    payment->setAmount(query.value(2).toDouble());
    payment->setTransactionCode(query.value(3).toString());
    payment->setPaymentDate(query.value(4).toDateTime());
    payment->setPaymentStatusId(query.value(5).toInt());
    return payment;
}
bool PaymentRepository::updatePaymentStatus(int paymentId, int newStatusId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Payments SET PaymentStatusID = :statusId WHERE PaymentID = :paymentId");
    query.bindValue(":statusId", newStatusId);
    query.bindValue(":paymentId", paymentId);
    if (!query.exec()) {
        qWarning() << "خطا در تغییر وضعیت پرداخت:" << query.lastError().text();
        return false;
    }
    return true;
}
bool PaymentRepository::setTransactionCode(int paymentId, const QString &code) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Payments SET TransactionCode = :code WHERE PaymentID = :paymentId");
    query.bindValue(":code", code);
    query.bindValue(":paymentId", paymentId);
    if (!query.exec()) {
        qWarning() << "خطا در ثبت کد تراکنش:" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<int> PaymentRepository::getAllPaymentIds() {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT PaymentID FROM Payments ORDER BY PaymentDate DESC");
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}