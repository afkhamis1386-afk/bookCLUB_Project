#ifndef ORDER_H
#define ORDER_H

#include <QVector>
#include <QDateTime>
#include <QDataStream>
#include <QString>
enum class OrderStatus {
    Pending   = 1,
    Paid      = 2,
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
    double discountAmount;
public:
    OrderItem();
    OrderItem(int bookId, double unitPrice, double discountPercent = 0.0, double discountAmount = 0.0);
    OrderItem(int orderItemId, int orderId, int bookId, double unitPrice, double discountPercent, double discountAmount);
    int getOrderItemId() const;
    int getOrderId() const;
    int getBookId() const;
    double getUnitPrice() const;
    double getDiscountPercent() const;
    double getDiscountAmount() const;
    double getFinalPrice() const;
    void setOrderItemId(int id);
    void setOrderId(int id);
    friend QDataStream &operator<<(QDataStream &out, const OrderItem &item);
    friend QDataStream &operator>>(QDataStream &in, OrderItem &item);
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
    OrderStatus status;
public:
    Order();
    explicit Order(int userId);
    Order(int orderId, int userId, const QDateTime &orderDate, double totalPrice, double discountAmount, double finalPrice, OrderStatus status);
    int getOrderId() const;
    int getUserId() const;
    QVector<OrderItem> getItems() const;
    QDateTime getOrderDate() const;
    double getTotalPrice() const;
    double getDiscountAmount() const;
    double getFinalPrice() const;
    OrderStatus getStatus() const;
    int getStatusId() const;
    int getItemCount() const;
    void setOrderId(int id);
    bool setTotalPrice(double price);
    bool setDiscountAmount(double amount);
    bool setFinalPrice(double price);
    void setStatus(OrderStatus status);
    void setItems(const QVector<OrderItem> &items);
    void addItem(const OrderItem &item);
    QString getStatusTitle() const;
    bool isPending() const;
    bool isPaid() const;
    bool isCancelled() const;
    bool isCompleted() const;
    friend QDataStream &operator<<(QDataStream &out, const Order &order);
    friend QDataStream &operator>>(QDataStream &in, Order &order);
};

#endif // ORDER_H
