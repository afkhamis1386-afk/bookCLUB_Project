#ifndef PAYMENT_H
#define PAYMENT_H
#include <QString>
#include <QDateTime>
#include <QDataStream>
enum class PaymentStatus {
    Pending = 1,
    Successful = 2,
    Failed = 3
};
class Payment {
private:
    int paymentId;
    int orderId;
    double amount;
    QString transactionCode;
    QDateTime paymentDate;
    int paymentStatusId;
public:
    Payment();
    Payment(int orderId, double amount, const QString& transactionCode = QString());
    int getPaymentId() const;
    int getOrderId() const;
    double getAmount() const;
    QString getTransactionCode() const;
    QDateTime getPaymentDate() const;
    int getPaymentStatusId() const;
    void setPaymentId(int id);
    void setOrderId(int id);
    bool setAmount(double amt);
    bool setTransactionCode(const QString& code);
    void setPaymentDate(const QDateTime& date);
    void setPaymentStatusId(int statusId);
    QString getStatusTitle() const;
    bool isSuccessful() const { return paymentStatusId == static_cast<int>(PaymentStatus::Successful); }
    bool isPending() const { return paymentStatusId == static_cast<int>(PaymentStatus::Pending); }
    bool isFailed() const { return paymentStatusId == static_cast<int>(PaymentStatus::Failed); }
    friend QDataStream& operator<<(QDataStream& out, const Payment& payment);
    friend QDataStream& operator>>(QDataStream& in, Payment& payment);
};
#endif // PAYMENT_H
