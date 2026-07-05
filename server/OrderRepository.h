#ifndef ORDERREPOSITORY_H
#define ORDERREPOSITORY_H
#include "../common/Order.h"
#include <QVector>
class OrderRepository {
public:
    OrderRepository();
    int insertOrder(const Order &order);
    Order* loadOrderById(int orderId);
    QVector<int> getOrderIdsByUser(int userId);
    QVector<int> getAllOrderIds();
    bool updateStatus(int orderId, OrderStatus newStatus);
};

#endif // ORDERREPOSITORY_H

