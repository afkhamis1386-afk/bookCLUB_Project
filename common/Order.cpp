#include "Order.h"
OrderItem::OrderItem()
    : orderItemId(-1), orderId(-1), bookId(-1), unitPrice(0), discountPercent(0) {}
OrderItem::OrderItem(int bookId, double unitPrice, double discountPercent)
    : orderItemId(-1), orderId(-1), bookId(bookId),
    unitPrice(unitPrice >= 0 ? unitPrice : 0),
    discountPercent(discountPercent >= 0 && discountPercent <= 100 ? discountPercent : 0) {}
OrderItem::OrderItem(int orderItemId, int orderId, int bookId, double unitPrice, double discountPercent)
    : orderItemId(orderItemId), orderId(orderId), bookId(bookId),
    unitPrice(unitPrice >= 0 ? unitPrice : 0),
    discountPercent(discountPercent >= 0 && discountPercent <= 100 ? discountPercent : 0) {}
int OrderItem::getOrderItemId() const { return orderItemId; }
int OrderItem::getOrderId() const { return orderId; }
int OrderItem::getBookId() const { return bookId; }
double OrderItem::getUnitPrice() const { return unitPrice; }
double OrderItem::getDiscountPercent() const { return discountPercent; }
double OrderItem::getFinalPrice() const {
    return unitPrice - (unitPrice * discountPercent / 100);
}
void OrderItem::setOrderItemId(int id) { orderItemId = id; }
void OrderItem::setOrderId(int id) { orderId = id; }
QDataStream& operator<<(QDataStream& out, const OrderItem& item) {
    out << item.orderItemId << item.orderId << item.bookId
        << item.unitPrice << item.discountPercent;
    return out;
}
QDataStream& operator>>(QDataStream& in, OrderItem& item) {
    in >> item.orderItemId >> item.orderId >> item.bookId
        >> item.unitPrice >> item.discountPercent;
    return in;
}
Order::Order()
    : orderId(-1), userId(-1), orderDate(QDateTime::currentDateTime()),
    totalPrice(0), discountAmount(0), finalPrice(0), statusId(static_cast<int>(OrderStatus::Pending)) {}
Order::Order(int userId, const QVector<OrderItem>& items)
    : orderId(-1),
    userId(userId),
    items(items),
    orderDate(QDateTime::currentDateTime()),
    totalPrice(0),
    discountAmount(0),
    finalPrice(0),
    statusId(static_cast<int>(OrderStatus::Pending)) { recalculatePrices();}
Order::Order(int orderId, int userId, const QVector<OrderItem>& items,
             const QDateTime& orderDate, double totalPrice, double discountAmount,
             double finalPrice, int statusId)
    : orderId(orderId), userId(userId), items(items), orderDate(orderDate),
    totalPrice(totalPrice), discountAmount(discountAmount),
    finalPrice(finalPrice), statusId(statusId) {}
int Order::getOrderId() const { return orderId; }
int Order::getUserId() const { return userId; }
QVector<OrderItem> Order::getItems() const { return items; }
QDateTime Order::getOrderDate() const { return orderDate; }
double Order::getTotalPrice() const { return totalPrice; }
double Order::getDiscountAmount() const { return discountAmount; }
double Order::getFinalPrice() const { return finalPrice; }
int Order::getStatusId() const { return statusId; }
double Order::calculateTotalPrice() const {
    double total = 0;
    for (const auto& item : items) {
        total += item.getUnitPrice();
    }
    return total;
}
double Order::calculateFinalPrice() const {
    return totalPrice - discountAmount;
}
double Order::calculateDiscountAmount() const {
    double sumDiscount = 0;
    for (const auto& item : items) {
        sumDiscount += (item.getUnitPrice() * item.getDiscountPercent() / 100);
    }
    return sumDiscount;
}

bool Order::isValid() const {
    return !items.isEmpty() && userId > 0 && finalPrice >= 0;
}
int Order::getItemCount() const {
    return items.size();
}
void Order::setOrderId(int id) { orderId = id; }
void Order::setStatusId(int id) { statusId = id; }
QString Order::getStatusTitle() const {
    OrderStatus current = static_cast<OrderStatus>(statusId);
    switch (current) {
    case OrderStatus::Pending:
        return "Pending";
    case OrderStatus::Paid:
        return "Paid";
    case OrderStatus::Cancelled:
        return "Cancelled";
    case OrderStatus::Completed:
        return "Completed";
    default:
        return "Unknown";
    }
}
void Order::recalculatePrices() {
        totalPrice = calculateTotalPrice();
        discountAmount = calculateDiscountAmount();
        finalPrice = calculateFinalPrice();
    }
QDataStream& operator<<(QDataStream& out, const Order& order) {
    out << order.orderId
        << order.userId
        << order.orderDate
        << order.totalPrice
        << order.discountAmount
        << order.finalPrice
        << order.statusId;
    out << static_cast<quint32>(order.items.size());
    for (const OrderItem& item : order.items) {
        out << item;
    }
    return out;
}
QDataStream& operator>>(QDataStream& in, Order& order) {
    in >> order.orderId
        >> order.userId
        >> order.orderDate
        >> order.totalPrice
        >> order.discountAmount
        >> order.finalPrice
        >> order.statusId;
    quint32 itemCount = 0;
    in >> itemCount;
    order.items.clear();
    for (quint32 i = 0; i < itemCount; ++i) {
        OrderItem item;
        in >> item;
        order.items.append(item);
    }
    return in;
}