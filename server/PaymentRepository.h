#ifndef PAYMENTREPOSITORY_H
#define PAYMENTREPOSITORY_H

#include "../common/Payment.h"
#include <QVector>
class PaymentRepository {
public:
    PaymentRepository();
    int insertPayment(const Payment &payment);
    Payment* loadPaymentById(int paymentId);
    Payment* loadPaymentByOrderId(int orderId);
    bool updatePaymentStatus(int paymentId, int newStatusId);
    bool setTransactionCode(int paymentId, const QString &code);
    QVector<int> getAllPaymentIds();
};

#endif // PAYMENTREPOSITORY_H
