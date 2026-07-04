#ifndef ORDERREPOSITORY_H
#define ORDERREPOSITORY_H
#include "../common/Order.h"

class OrderRepository {
public:
    OrderRepository();
    int insertOrder(const Order &order);
    Order* loadOrderById(int orderId);
    QVector<int> getOrderIdsByUser(int userId);
    bool updateStatus(int orderId, OrderStatus newStatus);
    QVector<int> getAllOrderIds();
};

#endif // ORDERREPOSITORY_H
