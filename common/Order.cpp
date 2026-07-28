#include "Order.h"
OrderItem::OrderItem() : orderItemId(-1), orderId(-1), bookId(-1), unitPrice(0), discountPercent(0), discountAmount(0) {}
OrderItem::OrderItem(int bookId, double unitPrice, double discountPercent, double discountAmount) : orderItemId(-1), orderId(-1), bookId(bookId),
    unitPrice(unitPrice >= 0 ? unitPrice : 0), discountPercent(discountPercent >= 0 && discountPercent <= 100 ? discountPercent : 0),
    discountAmount(discountAmount >= 0 ? discountAmount : 0) {}
OrderItem::OrderItem(int orderItemId, int orderId, int bookId, double unitPrice, double discountPercent, double discountAmount) : orderItemId(orderItemId), orderId(orderId), bookId(bookId),
    unitPrice(unitPrice >= 0 ? unitPrice : 0), discountPercent(discountPercent >= 0 && discountPercent <= 100 ? discountPercent : 0),
    discountAmount(discountAmount >= 0 ? discountAmount : 0) {}
int OrderItem::getOrderItemId() const { return orderItemId; }
int OrderItem::getOrderId() const { return orderId; }
int OrderItem::getBookId() const { return bookId; }
double OrderItem::getUnitPrice() const { return unitPrice; }
double OrderItem::getDiscountPercent() const { return discountPercent; }
double OrderItem::getDiscountAmount() const { return discountAmount; }
double OrderItem::getFinalPrice() const {
    double afterPercent = unitPrice - (unitPrice * discountPercent / 100);
    double final = afterPercent - discountAmount;
    return final < 0 ? 0 : final;
}
void OrderItem::setOrderItemId(int id) { orderItemId = id; }
void OrderItem::setOrderId(int id) { orderId = id; }
QDataStream &operator<<(QDataStream &out, const OrderItem &item) {
    out << item.orderItemId << item.orderId << item.bookId
        << item.unitPrice << item.discountPercent << item.discountAmount;
    return out;
}
QDataStream &operator>>(QDataStream &in, OrderItem &item) {
    in >> item.orderItemId >> item.orderId >> item.bookId
        >> item.unitPrice >> item.discountPercent >> item.discountAmount;
    if (item.unitPrice < 0)
        item.unitPrice = 0;
    if (item.discountPercent < 0 || item.discountPercent > 100)
        item.discountPercent = 0;
    if (item.discountAmount < 0)
        item.discountAmount = 0;
    return in;
}
Order::Order()
    : orderId(-1), userId(-1), orderDate(QDateTime::currentDateTime()),
    totalPrice(0), discountAmount(0), finalPrice(0), status(OrderStatus::Pending) {}
Order::Order(int userId)
    : orderId(-1), userId(userId), orderDate(QDateTime::currentDateTime()),
    totalPrice(0), discountAmount(0), finalPrice(0), status(OrderStatus::Pending) {}
Order::Order(int orderId, int userId, const QDateTime &orderDate, double totalPrice, double discountAmount, double finalPrice, OrderStatus status)
    : orderId(orderId), userId(userId), orderDate(orderDate), totalPrice(totalPrice),
    discountAmount(discountAmount), finalPrice(finalPrice), status(status) {}
int Order::getOrderId() const { return orderId; }
int Order::getUserId() const { return userId; }
QVector<OrderItem> Order::getItems() const { return items; }
QDateTime Order::getOrderDate() const { return orderDate; }
double Order::getTotalPrice() const { return totalPrice; }
double Order::getDiscountAmount() const { return discountAmount; }
double Order::getFinalPrice() const { return finalPrice; }
OrderStatus Order::getStatus() const { return status; }
int Order::getStatusId() const { return static_cast<int>(status); }
int Order::getItemCount() const { return items.size(); }
void Order::setOrderId(int id) { orderId = id; }
bool Order::setTotalPrice(double price) { if (price < 0) return false; totalPrice = price; return true; }
bool Order::setDiscountAmount(double amount) { if (amount < 0) return false; discountAmount = amount; return true; }
bool Order::setFinalPrice(double price) { if (price < 0) return false; finalPrice = price; return true; }
void Order::setStatus(OrderStatus s) { status = s; }
void Order::setItems(const QVector<OrderItem> &i) { items = i; }
void Order::addItem(const OrderItem &item) { items.append(item); }
QString Order::getStatusTitle() const {
    switch (status) {
    case OrderStatus::Pending:   return "در انتظار";
    case OrderStatus::Paid:      return "پرداخت شده";
    case OrderStatus::Cancelled: return "لغو شده ";
    case OrderStatus::Completed: return "تکمیل شده";
    default: return "نامشخص";
    }
}
bool Order::isPending() const{ return status == OrderStatus::Pending; }
bool Order::isPaid() const { return status == OrderStatus::Paid; }
bool Order::isCancelled() const { return status == OrderStatus::Cancelled; }
bool Order::isCompleted() const { return status == OrderStatus::Completed; }
QDataStream &operator<<(QDataStream &out, const Order &order) {
    out << order.orderId << order.userId << order.orderDate << order.totalPrice
        << order.discountAmount << order.finalPrice << static_cast<int>(order.status);
    out << static_cast<quint32>(order.items.size());
    for (const OrderItem &item : order.items) out << item;
    return out;
}
QDataStream &operator>>(QDataStream &in, Order &order) {
    int statusInt;
    in >> order.orderId >> order.userId >> order.orderDate >> order.totalPrice
        >> order.discountAmount >> order.finalPrice >> statusInt;
    if (statusInt < 1 || statusInt > 4)
        order.status = OrderStatus::Pending;
    else
        order.status = static_cast<OrderStatus>(statusInt);
    quint32 count;
    in >> count;
    order.items.clear();
    for (quint32 i = 0; i < count; ++i) {
        OrderItem item;
        in >> item;
        order.items.append(item);
    }
    return in;
}