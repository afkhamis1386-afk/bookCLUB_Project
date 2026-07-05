#include "OrderRepository.h"
#include "DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QtGlobal>
OrderRepository::OrderRepository() {}
int OrderRepository::insertOrder(const Order &order) {
    if (order.getItems().isEmpty()) {
        qWarning() << "سفارش خالی است و قابل ثبت نیست.";
        return -1;
    }
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش ثبت سفارش:" << db.lastError().text();
        return -1;
    }
    QSqlQuery insertOrderQuery(db);
    insertOrderQuery.prepare(
        "INSERT INTO Orders "
        "(UserID, OrderDate, TotalPrice, DiscountAmount, FinalPrice, StatusID) "
        "OUTPUT INSERTED.OrderID "
        "VALUES "
        "(:userId, :orderDate, :totalPrice, :discountAmount, :finalPrice, :statusId)"
        );
    insertOrderQuery.bindValue(":userId", order.getUserId());
    insertOrderQuery.bindValue(":orderDate", order.getOrderDate());
    insertOrderQuery.bindValue(":totalPrice", order.getTotalPrice());
    insertOrderQuery.bindValue(":discountAmount", order.getDiscountAmount());
    insertOrderQuery.bindValue(":finalPrice", order.getFinalPrice());
    insertOrderQuery.bindValue(":statusId", static_cast<int>(order.getStatus()));
    if (!insertOrderQuery.exec()) {
        qWarning() << "خطا در ثبت سفارش:" << insertOrderQuery.lastError().text();
        db.rollback();
        return -1;
    }
    if (!insertOrderQuery.next()) {
        qWarning() << "شناسه سفارش جدید دریافت نشد.";
        db.rollback();
        return -1;
    }
    int newOrderId = insertOrderQuery.value(0).toInt();
    QVector<OrderItem> items = order.getItems();
    for (const OrderItem &item : qAsConst(items)) {
        QSqlQuery insertItemQuery(db);
        insertItemQuery.prepare(
            "INSERT INTO OrderItems "
            "(OrderID, BookID, UnitPrice, DiscountPercent, DiscountAmount) "
            "VALUES "
            "(:orderId, :bookId, :unitPrice, :discountPercent, :discountAmount)"
            );
        insertItemQuery.bindValue(":orderId", newOrderId);
        insertItemQuery.bindValue(":bookId", item.getBookId());
        insertItemQuery.bindValue(":unitPrice", item.getUnitPrice());
        insertItemQuery.bindValue(":discountPercent", item.getDiscountPercent());
        insertItemQuery.bindValue(":discountAmount", item.getDiscountAmount());
        if (!insertItemQuery.exec()) {
            qWarning() << "خطا در ثبت آیتم سفارش:" << insertItemQuery.lastError().text();
            db.rollback();
            return -1;
        }
    }
    if (!db.commit()) {
        qWarning() << "خطا در تایید نهایی تراکنش ثبت سفارش:" << db.lastError().text();
        db.rollback();
        return -1;
    }
    return newOrderId;
}
Order* OrderRepository::loadOrderById(int orderId) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery orderQuery(db);
    orderQuery.prepare(
        "SELECT "
        "OrderID, UserID, OrderDate, TotalPrice, DiscountAmount, FinalPrice, StatusID "
        "FROM Orders "
        "WHERE OrderID = :orderId"
        );
    orderQuery.bindValue(":orderId", orderId);
    if (!orderQuery.exec()) {
        qWarning() << "خطا در اجرای کوئری دریافت سفارش:" << orderQuery.lastError().text();
        return nullptr;
    }
    if (!orderQuery.next()) {
        qWarning() << "سفارشی با این شناسه یافت نشد OrderID:" << orderId;
        return nullptr;
    }
    int loadedOrderId = orderQuery.value("OrderID").toInt();
    int userId = orderQuery.value("UserID").toInt();
    QDateTime orderDate = orderQuery.value("OrderDate").toDateTime();
    double totalPrice = orderQuery.value("TotalPrice").toDouble();
    double discountAmount = orderQuery.value("DiscountAmount").toDouble();
    double finalPrice = orderQuery.value("FinalPrice").toDouble();
    int statusId = orderQuery.value("StatusID").toInt();
    OrderStatus status = static_cast<OrderStatus>(statusId);
    QVector<OrderItem> items;
    QSqlQuery itemsQuery(db);
    itemsQuery.prepare(
        "SELECT "
        "OrderItemID, OrderID, BookID, UnitPrice, DiscountPercent, DiscountAmount "
        "FROM OrderItems "
        "WHERE OrderID = :orderId "
        "ORDER BY OrderItemID ASC"
        );
    itemsQuery.bindValue(":orderId", orderId);
    if (!itemsQuery.exec()) {
        qWarning() << "خطا در بارگذاری آیتم های سفارش:" << itemsQuery.lastError().text();
        return nullptr;
    }
    while (itemsQuery.next()) {
        int orderItemId = itemsQuery.value("OrderItemID").toInt();
        int loadedItemOrderId = itemsQuery.value("OrderID").toInt();
        int bookId = itemsQuery.value("BookID").toInt();
        double unitPrice = itemsQuery.value("UnitPrice").toDouble();
        double discountPercent = itemsQuery.value("DiscountPercent").toDouble();
        double discountAmountItem = itemsQuery.value("DiscountAmount").toDouble();
        OrderItem item(orderItemId, loadedItemOrderId, bookId, unitPrice, discountPercent, discountAmountItem);
        items.append(item);
    }
    Order *order = new Order(loadedOrderId, userId, orderDate, totalPrice, discountAmount, finalPrice,status );
    order->setItems(items);
    return order;
}
QVector<int> OrderRepository::getOrderIdsByUser(int userId) {
    QVector<int> orderIds;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT OrderID "
        "FROM Orders "
        "WHERE UserID = :userId "
        "ORDER BY OrderDate DESC"
        );
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qWarning() << "خطا در دریافت سفارش های کاربر:" << query.lastError().text();
        return orderIds;
    }
    while (query.next()) {
        int orderId = query.value("OrderID").toInt();
        orderIds.append(orderId);
    }
    return orderIds;
}
QVector<int> OrderRepository::getAllOrderIds() {
    QVector<int> orderIds;
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT OrderID "
        "FROM Orders "
        "ORDER BY OrderDate DESC"
        );
    if (!query.exec()) {
        qWarning() << "خطا در دریافت همه سفارش ها:" << query.lastError().text();
        return orderIds;
    }
    while (query.next()) {
        int orderId = query.value("OrderID").toInt();
        orderIds.append(orderId);
    }
    return orderIds;
}
bool OrderRepository::updateStatus(int orderId, OrderStatus newStatus) {
    QSqlDatabase &db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "UPDATE Orders "
        "SET StatusID = :statusId "
        "WHERE OrderID = :orderId"
        );
    query.bindValue(":statusId", static_cast<int>(newStatus));
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        qWarning() << "خطا در بروزرسانی وضعیت سفارش:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}
