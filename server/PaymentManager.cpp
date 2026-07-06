#include "PaymentManager.h"
#include "PaymentRepository.h"
#include "../common/Payment.h"
#include <memory>

PaymentManager::PaymentManager() {}
Response PaymentManager::recordPayment(int orderId, double amount) {
    if (amount < 0) {
        return Response(ResponseStatus::ValidationFailed, "مبلغ پرداخت نمی تواند منفی باشد");
    }
    Payment newPayment(orderId, amount);
    newPayment.setPaymentStatusId(static_cast<int>(PaymentStatus::Successful));
    PaymentRepository paymentRepo;
    int newPaymentId = paymentRepo.insertPayment(newPayment);
    if (newPaymentId == -1) {
        return Response(ResponseStatus::Error, "خطا در ثبت پرداخت");
    }
    QVariantMap data;
    data["paymentId"] = newPaymentId;
    return Response(ResponseStatus::Success, "پرداخت با موفقیت ثبت شد", data);
}
Response PaymentManager::getPaymentByOrderId(int orderId) {
    PaymentRepository paymentRepo;
    std::unique_ptr<Payment> payment(paymentRepo.loadPaymentByOrderId(orderId));
    if (!payment) {
        return Response(ResponseStatus::NotFound, "پرداختی برای این سفارش یافت نشد");
    }
    QVariantMap data;
    data["paymentId"] = payment->getPaymentId();
    data["orderId"] = payment->getOrderId();
    data["amount"] = payment->getAmount();
    data["transactionCode"] = payment->getTransactionCode();
    data["paymentDate"] = payment->getPaymentDate();
    data["statusTitle"] = payment->getStatusTitle();
    data["isSuccessful"] = payment->isSuccessful();
    return Response(ResponseStatus::Success, "اطلاعات پرداخت بازیابی شد", data);
}
Response PaymentManager::updatePaymentStatus(int paymentId, int newStatusId) {
    if (newStatusId < 1 || newStatusId > 3) {
        return Response(ResponseStatus::ValidationFailed, "وضعیت پرداخت نامعتبر است");
    }
    PaymentRepository paymentRepo;
    if (!paymentRepo.updatePaymentStatus(paymentId, newStatusId)) {
        return Response(ResponseStatus::Error, "خطا در به روزرسانی وضعیت پرداخت");
    }
    return Response(ResponseStatus::Success, "وضعیت پرداخت با موفقیت به روزرسانی شد");
}
