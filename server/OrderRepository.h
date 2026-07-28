#ifndef ORDERREPOSITORY_H
#define ORDERREPOSITORY_H
#include "../common/Order.h"
#include <QVector>
#include <QStringList>
class OrderRepository {
public:
    OrderRepository();
    int insertOrder(const Order &order);
    bool addBookToLibrary(int userId, int bookId);
    Order* loadOrderById(int orderId);
    QStringList getBookNamesByOrderId(int orderId);
    QVector<int> getOrderIdsByUser(int userId);
    QVector<int> getAllOrderIds();
    bool updateStatus(int orderId, OrderStatus newStatus);
};

#endif // ORDERREPOSITORY_H