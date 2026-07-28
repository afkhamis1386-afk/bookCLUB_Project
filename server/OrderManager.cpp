#include "OrderManager.h"
#include "CartRepository.h"
#include "NotificationManager.h"
#include "ClientRegistry.h"
#include "PaymentRepository.h"
#include "../common/Payment.h"
#include "OrderRepository.h"
#include "BookRepository.h"
#include "TimedDiscountRepository.h"
#include "DatabaseManager.h"
#include "PriceCalculator.h"
#include "../common/Cart.h"
#include "../common/Order.h"
#include "../common/Book.h"
#include "../common/Enums.h"
#include "../common/TimedDiscount.h"
#include "UserRepository.h"
#include "../common/normaluser.h"
#include <QRegularExpression>
#include <QDateTime>
#include <QStringList>
#include <memory>

OrderManager::OrderManager() {}
Response OrderManager::checkout(int userId, const QString &cardNumber) {
    QString cleanCardNumber = cardNumber;
    cleanCardNumber.remove(' ');
    static const QRegularExpression cardRegex(R"(^\d{16}$)");
    if (!cardRegex.match(cleanCardNumber).hasMatch()) {
        return Response(ResponseStatus::ValidationFailed, "شماره کارت نامعتبر است (باید ۱۶ رقم باشد)");
    }
    CartRepository cartRepo;
    std::unique_ptr<Cart> cart(cartRepo.loadCartByUserId(userId));
    if (!cart || cart->getItemCount() == 0) {
        return Response(ResponseStatus::ValidationFailed, "سبد خرید شما خالی است");
    }
    BookRepository bookRepo;
    TimedDiscountRepository timedDiscountRepo;
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    QVector<CartItem> cartItems = cart->getItems();
    Order newOrder(userId);
    double totalPrice = 0;
    double totalDiscountAmount = 0;
    struct SoldBookInfo {
        int publisherUserId;
        int bookId;
        QString bookName;
    };
    QVector<SoldBookInfo> soldBooks;
    for (const CartItem &cartItem : qAsConst(cartItems)) {
        std::unique_ptr<Book> book(bookRepo.loadBookById(cartItem.getBookId()));
        if (!book || !book->isAvailableForPurchase()) {
            return Response(ResponseStatus::Error, QString("کتاب با شناسه %1 دیگر برای خرید موجود نیست").arg(cartItem.getBookId()));
        }
        if (user && user->hasPurchased(cartItem.getBookId())) {
            return Response(ResponseStatus::ValidationFailed, QString("شما قبلاً کتاب «%1» را خریداری کرده اید").arg(book->getBookName()));
        }
        std::unique_ptr<TimedDiscount> activeDiscount( timedDiscountRepo.getActiveDiscountForBook(cartItem.getBookId()) );
        double timedPercent = activeDiscount ? activeDiscount->getDiscountPercent() : 0;
        double effectivePercent = PriceCalculator::calculateEffectivePercent(
            book->getDiscountPercent(), timedPercent
            );
        double itemPrice = book->getBookPrice();
        double itemFinalPrice = PriceCalculator::calculateFinalPrice(
            itemPrice, effectivePercent, book->getDiscountAmount()
            );
        double itemDiscountTotal = itemPrice - itemFinalPrice;
        OrderItem orderItem(book->getBookId(), itemPrice, effectivePercent, book->getDiscountAmount());
        newOrder.addItem(orderItem);
        soldBooks.push_back(SoldBookInfo{
            book->getPublisherUserId(),
            book->getBookId(),
            book->getBookName()
        });
        totalPrice += itemPrice;
        totalDiscountAmount += itemDiscountTotal;
    }
    double finalPrice = totalPrice - totalDiscountAmount;
    if (finalPrice < 0) finalPrice = 0;
    newOrder.setTotalPrice(totalPrice);
    newOrder.setDiscountAmount(totalDiscountAmount);
    newOrder.setFinalPrice(finalPrice);
    newOrder.setStatus(OrderStatus::Paid);
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.transaction()) {
        return Response(ResponseStatus::Error, "خطا در شروع تراکنش ثبت سفارش");
    }
    OrderRepository orderRepo;
    int newOrderId = orderRepo.insertOrder(newOrder);
    if (newOrderId == -1) {
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در ثبت سفارش. لطفاً دوباره تلاش کنید");
    }
    QString lastFourDigits = cleanCardNumber.right(4);
    QString transactionCode = QString("TXN-%1-%2-%3")
                                  .arg(newOrderId)
                                  .arg(lastFourDigits)
                                  .arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
    Payment newPayment(newOrderId, finalPrice, transactionCode);
    newPayment.setPaymentStatusId(static_cast<int>(PaymentStatus::Successful));
    PaymentRepository paymentRepo;
    int newPaymentId = paymentRepo.insertPayment(newPayment);
    if (newPaymentId == -1) {
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در ثبت پرداخت. لطفاً دوباره تلاش کنید");
    }
    int cartId = cartRepo.getOrCreateCartId(userId);
    if (!cartRepo.clearCart(cartId)) {
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در خالی کردن سبد خرید پس از ثبت سفارش");
    }
    if (!db.commit()) {
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در نهایی سازی تراکنش خرید");
    }
    NotificationManager notifManager;
    for (const SoldBookInfo &sold : qAsConst(soldBooks)) {
        notifManager.sendNotification( sold.publisherUserId, NotificationType::NewSaleForPublisher, "فروش جدید", QString("کتاب «%1» شما به فروش رسید").arg(sold.bookName), sold.bookId, userId );
        QVariantMap saleLiveData;
        saleLiveData["bookId"] = sold.bookId;
        ClientRegistry::getInstance()->broadcastLiveUpdate("newSale", saleLiveData);
    }
    QVariantMap data;
    data["orderId"] = newOrderId;
    data["finalPrice"] = finalPrice;
    data["totalPrice"] = totalPrice;
    data["discountAmount"] = totalDiscountAmount;
    data["paymentId"] = newPaymentId;
    data["transactionCode"] = transactionCode;
    data["cardLastFour"] = lastFourDigits;
    return Response(ResponseStatus::Success, "خرید با موفقیت انجام شد", data);
}
Response OrderManager::getOrderHistory(int userId) {
    OrderRepository orderRepo;
    QVector<int> orderIds = orderRepo.getOrderIdsByUser(userId);
    QVariantList orderList;
    for (int orderId : qAsConst(orderIds)) {
        std::unique_ptr<Order> order(orderRepo.loadOrderById(orderId));
        if (!order) continue;
        QVariantMap orderData;
        orderData["orderId"] = order->getOrderId();
        const QStringList bookNames = orderRepo.getBookNamesByOrderId(orderId);
        orderData["bookNames"] = bookNames.isEmpty()
                                     ? QString("ثبت نشده")
                                     : bookNames.join(QStringLiteral("، "));
        orderData["orderDate"] = order->getOrderDate();
        orderData["finalPrice"] = order->getFinalPrice();
        orderData["status"] = order->getStatusTitle();
        orderData["itemCount"] = order->getItemCount();
        orderList.append(orderData);
    }
    QVariantMap data;
    data["orders"] = orderList;
    return Response(ResponseStatus::Success, "تاریخچه خرید بازیابی شد", data);
}
Response OrderManager::getOrderDetails(int userId, int orderId) {
    OrderRepository orderRepo;
    std::unique_ptr<Order> order(orderRepo.loadOrderById(orderId));
    if (!order) {
        return Response(ResponseStatus::NotFound, "سفارش یافت نشد");
    }
    if (order->getUserId() != userId) {
        return Response(ResponseStatus::Unauthorized, "شما اجازه مشاهده این سفارش را ندارید");
    }
    QVariantList itemList;
    for (const OrderItem &item : order->getItems()) {
        QVariantMap itemData;
        itemData["bookId"] = item.getBookId();
        itemData["unitPrice"] = item.getUnitPrice();
        itemData["discountPercent"] = item.getDiscountPercent();
        itemData["discountAmount"] = item.getDiscountAmount();
        itemData["finalPrice"] = item.getFinalPrice();
        itemList.append(itemData);
    }
    QVariantMap data;
    data["orderId"] = order->getOrderId();
    data["orderDate"] = order->getOrderDate();
    data["totalPrice"] = order->getTotalPrice();
    data["discountAmount"] = order->getDiscountAmount();
    data["finalPrice"] = order->getFinalPrice();
    data["status"] = order->getStatusTitle();
    data["items"] = itemList;
    return Response(ResponseStatus::Success, "جزئیات سفارش بازیابی شد", data);
}
Response OrderManager::cancelorder(int userId, int orderId) {
    OrderRepository orderRepo;
    std::unique_ptr<Order> order(orderRepo.loadOrderById(orderId));
    if (!order) {
        return Response(ResponseStatus::NotFound, "سفارش یافت نشد");
    }
    if (order->getUserId() != userId) {
        return Response(ResponseStatus::Unauthorized, "شما اجازه لغو این سفارش را ندارید");
    }
    if (!order->isPending()) {
        return Response(ResponseStatus::Error, "فقط سفارش های در انتظار قابل لغو هستند");
    }
    if (!orderRepo.updateStatus(orderId, OrderStatus::Cancelled)) {
        return Response(ResponseStatus::Error, "خطا در لغو سفارش");
    }
    return Response(ResponseStatus::Success, "سفارش با موفقیت لغو شد");
}