#ifndef PAYMENTMANAGER_H
#define PAYMENTMANAGER_H
#include "../common/Response.h"
class PaymentManager {
public:
    PaymentManager();
    Response recordPayment(int orderId, double amount);
    Response getPaymentByOrderId(int orderId);
    Response updatePaymentStatus(int paymentId, int newStatusId);
};
#endif // PAYMENTMANAGER_H
