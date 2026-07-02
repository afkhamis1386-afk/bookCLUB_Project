#ifndef ORDER_H
#define ORDER_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QDataStream>
enum class OrderStatus {
    Pending = 1,
    Paid = 2,
    Cancelled = 3,
    Completed = 4
};
class OrderItem {
private:
    int orderItemId;
    int orderId;
    int bookId;
    double unitPrice;
    double discountPercent;
public:
    OrderItem();
    OrderItem(int bookId, double unitPrice, double discountPercent = 0);
    OrderItem(int orderItemId, int orderId, int bookId, double unitPrice, double discountPercent);
    int getOrderItemId() const;
    int getOrderId() const;
    int getBookId() const;
    double getUnitPrice() const;
    double getDiscountPercent() const;
    double getFinalPrice() const;
    void setOrderItemId(int id);
    void setOrderId(int id);
    friend QDataStream& operator<<(QDataStream& out, const OrderItem& item);
    friend QDataStream& operator>>(QDataStream& in, OrderItem& item);
};
class Order {
private:
    int orderId;
    int userId;
    QVector<OrderItem> items;
    QDateTime orderDate;
    double totalPrice;
    double discountAmount;
    double finalPrice;
    int statusId;
public:
    Order();
    Order(int userId, const QVector<OrderItem>& items);
    Order(int orderId, int userId, const QVector<OrderItem>& items, const QDateTime& orderDate, double totalPrice, double discountAmount, double finalPrice, int statusId);
    int getOrderId() const;
    int getUserId() const;
    QVector<OrderItem> getItems() const;
    QDateTime getOrderDate() const;
    double getTotalPrice() const;
    double getDiscountAmount() const;
    double getFinalPrice() const;
    int getStatusId() const;
    double calculateTotalPrice() const;
    double calculateFinalPrice() const;
    double calculateDiscountAmount() const;
    bool isValid() const;
    int getItemCount() const;
    void setOrderId(int id);
    void setStatusId(int statusId);
    QString getStatusTitle() const;
    bool isPending() const { return statusId == static_cast<int>(OrderStatus::Pending); }
    bool isPaid() const { return statusId == static_cast<int>(OrderStatus::Paid); }
    bool isCancelled() const { return statusId == static_cast<int>(OrderStatus::Cancelled); }
    bool isCompleted() const { return statusId == static_cast<int>(OrderStatus::Completed); }
    void recalculatePrices();
    friend QDataStream& operator<<(QDataStream& out, const Order& order);
    friend QDataStream& operator>>(QDataStream& in, Order& order);
};
#endif // ORDER_H
