#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H
#include "../common/Response.h"

class OrderManager {
public:
    OrderManager();
    Response checkout(int userId, const QString &cardNumber);
    Response getOrderHistory(int userId);
    Response getOrderDetails(int userId, int orderId);
    Response cancelorder(int userId, int orderId);
};
#endif // ORDERMANAGER_H
