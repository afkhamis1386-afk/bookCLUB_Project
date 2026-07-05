#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QObject>
#include <QVector>
#include "../common/Order.h"
#include "OrderRepository.h"
class OrderManager : public QObject {
    Q_OBJECT
private:
    OrderRepository orderRepo;
public:
    explicit OrderManager(QObject *parent = nullptr);
    ~OrderManager();
    int createOrder(int userId, const QVector<OrderItem> &items, double promoDiscount = 0);
    Order* getOrderById(int orderId);
    QVector<Order*> getUserOrders(int userId);
    void extracted(QVector<Order *> &orders, QVector<int> &ids);
    QVector<Order *> getAllOrders();
    bool updateOrderStatus(int orderId, OrderStatus newStatus);
    bool payOrder(int orderId, double userWalletBalance, double &outNewBalance);
    bool cancelOrder(int orderId);
    bool completeOrder(int orderId);
};
#endif // ORDERMANAGER_H
