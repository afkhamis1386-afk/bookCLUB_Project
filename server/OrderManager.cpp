#include "OrderManager.h"
#include "PriceCalculator.h"
#include <QScopedPointer>

#include <QDebug>

OrderManager::OrderManager(QObject *parent)
    : QObject(parent) {
}
OrderManager::~OrderManager() {
}
int OrderManager::createOrder(int userId, const QVector<OrderItem> &items, double promoDiscount) {
    if (items.isEmpty()) {
        qWarning() << "امکان ثبت سفارش وجود ندارد: لیست آیتم‌ها خالی است";
        return -1;
    }
    double totalOriginalPrice = 0;
    double totalDiscountAmount = 0;
    double totalFinalPrice = 0;
    QVector<OrderItem> processedItems;
    for (const OrderItem &item : items) {
        double itemFinalPrice = PriceCalculator::calculateFinalPrice( item.getUnitPrice(), item.getDiscountPercent(), item.getDiscountAmount() );
        totalOriginalPrice += item.getUnitPrice();
        double itemDiscount = item.getUnitPrice() - itemFinalPrice;
        totalDiscountAmount += itemDiscount;
        totalFinalPrice += itemFinalPrice;
        processedItems.append(item);
    }
    double finalOrderPrice = totalFinalPrice;
    if (promoDiscount > 0) {
        finalOrderPrice = PriceCalculator::calculateFinalPrice(totalFinalPrice, 0, promoDiscount);
        double extraDiscount = totalFinalPrice - finalOrderPrice;
        totalDiscountAmount += extraDiscount;
    }
    Order order(userId);
    order.setItems(processedItems);
    order.setTotalPrice(totalOriginalPrice);
    order.setDiscountAmount(totalDiscountAmount);
    order.setFinalPrice(finalOrderPrice);
    order.setStatus(OrderStatus::Pending);
    int newOrderId = orderRepo.insertOrder(order);
    if (newOrderId == -1) {
        qCritical() << "خطا در ذخیره‌سازی سفارش در دیتابیس";
    }
    return newOrderId;
}
Order* OrderManager::getOrderById(int orderId) {
    return orderRepo.loadOrderById(orderId);
}
QVector<Order*> OrderManager::getUserOrders(int userId) {
    QVector<Order*> orders;
    QVector<int> ids = orderRepo.getOrderIdsByUser(userId);
    for (int id : qAsConst(ids)) {
        Order *order = orderRepo.loadOrderById(id);
        if (order) {
            orders.append(order);
        }
    }
    return orders;
}
void OrderManager::extracted(QVector<Order *> &orders, QVector<int> &ids) {
    for (const int id : ids) {
        Order *order = orderRepo.loadOrderById(id);
        if (order) {
            orders.append(order);
        }
    }
}
QVector<Order *> OrderManager::getAllOrders() {
    QVector<Order *> orders;
    QVector<int> ids = orderRepo.getAllOrderIds();
    extracted(orders, ids);
    return orders;
}
bool OrderManager::updateOrderStatus(int orderId, OrderStatus newStatus) {
    return orderRepo.updateStatus(orderId, newStatus);
}
bool OrderManager::payOrder(int orderId, double userWalletBalance, double &outNewBalance) {
    Order* order = orderRepo.loadOrderById(orderId);
    if (!order) {
        qWarning() << "پرداخت ناموفق: سفارش پیدا نشد. شناسه:" << orderId;
        return false;
    }
    QScopedPointer<Order> orderPtr(order);
    if (orderPtr->getStatus() != OrderStatus::Pending) {
        qWarning() << "پرداخت ناموفق: سفارش در وضعیت پرداخت معلق نیست";
        return false;
    }
    double finalPrice = orderPtr->getFinalPrice();
    if (userWalletBalance < finalPrice) {
        qWarning() << "پرداخت ناموفق: موجودی کیف پول کاربر کافی نیست";
        return false;
    }
    if (orderRepo.updateStatus(orderId, OrderStatus::Paid)) {
        outNewBalance = userWalletBalance - finalPrice;
        qInfo() << "پرداخت سفارش با موفقیت انجام شد. شناسه:" << orderId;
        return true;
    }
    qWarning() << "خطا در بروزرسانی وضعیت پرداخت سفارش در دیتابیس";
    return false;
}
bool OrderManager::cancelOrder(int orderId) {
    Order* order = orderRepo.loadOrderById(orderId);
    if (!order) {
        return false;
    }
    QScopedPointer<Order> orderPtr(order);
    if (orderPtr->getStatus() != OrderStatus::Pending) {
        qWarning() << "امکان لغو سفارش وجود ندارد؛ سفارش در وضعیت Pending نیست";
        return false;
    }
    return orderRepo.updateStatus(orderId, OrderStatus::Cancelled);
}
bool OrderManager::completeOrder(int orderId) {
    Order* order = orderRepo.loadOrderById(orderId);
    if (!order) {
        return false;
    }
    QScopedPointer<Order> orderPtr(order);
    if (orderPtr->getStatus() != OrderStatus::Paid) {
        qWarning() << "امکان تکمیل سفارش وجود ندارد؛ ابتدا باید پرداخت شود";
        return false;
    }
    return orderRepo.updateStatus(orderId, OrderStatus::Completed);
}
