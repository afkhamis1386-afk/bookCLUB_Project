#include "PublisherBookController.h"

PublisherBookController::PublisherBookController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager){
    connect(networkManager, &NetworkManager::responseReceived, this, &PublisherBookController::onResponseReceived);
}
void PublisherBookController::addBook(const QString &bookName, const QString &description, double price, const QString &genreTitle, const QString &categoryTitle,
                                      const QString &authorName, const QString &coverImagePath,
                                      const QString &pdfFilePath) {
    if (bookName.trimmed().isEmpty() || bookName.length() > 60) {
        emit validationError("نام کتاب نامعتبر است (حداکثر ۶۰ کاراکتر)");
        return;
    }
    if (description.trimmed().isEmpty()) {
        emit validationError("توضیحات کتاب نمی تواند خالی باشد");
        return;
    }
    if (price < 0) {
        emit validationError("قیمت کتاب نمی تواند منفی باشد");
        return;
    }
    if (genreTitle.trimmed().isEmpty() || categoryTitle.trimmed().isEmpty() || authorName.trimmed().isEmpty()) {
        emit validationError("ژانر، دسته بندی و نام نویسنده الزامی هستند");
        return;
    }
    if (!networkManager->isConnected()) {
        emit bookAddFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->addBook(bookName.trimmed(), description.trimmed(), price,genreTitle.trimmed(), categoryTitle.trimmed(), authorName.trimmed(),
                            coverImagePath, pdfFilePath);
}
void PublisherBookController::updateBook(int bookId, const QString &bookName, const QString &description, double price) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (bookName.trimmed().isEmpty() || bookName.length() > 60) {
        emit validationError("نام کتاب نامعتبر است (حداکثر ۶۰ کاراکتر)");
        return;
    }
    if (description.trimmed().isEmpty()) {
        emit validationError("توضیحات کتاب نمی تواند خالی باشد");
        return;
    }
    if (price < 0) {
        emit validationError("قیمت کتاب نمی تواند منفی باشد");
        return;
    }
    if (!networkManager->isConnected()) {
        emit bookUpdateFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->updateBook(bookId, bookName.trimmed(), description.trimmed(), price);
}
void PublisherBookController::deactivateBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit bookDeactivateFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->deactivateBook(bookId);
}
void PublisherBookController::reactivateBook(int bookId) {
    if (bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if (!networkManager->isConnected()) { emit bookReactivateFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->reactivateBook(bookId);
}
void PublisherBookController::applyDiscount(int bookId, double discountPercent, double discountAmount) {
    if (bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if (discountPercent > 0 && discountAmount > 0) {
        emit validationError("فقط یکی از تخفیف درصدی یا مبلغی را می توانید تعیین کنید");
        return;
    }
    if (discountPercent < 0 || discountAmount < 0) {
        emit validationError("مقدار تخفیف نمی تواند منفی باشد");
        return;
    }
    if (!networkManager->isConnected()) { emit discountApplyFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->applyDiscount(bookId, discountPercent, discountAmount);
}
void PublisherBookController::onResponseReceived(RequestType type, const Response &response) {
    switch (type) {
    case RequestType::AddBook:
        if (response.isSuccess())
            emit bookAdded(response.getData().value("bookId").toInt(), response.getMessage());
        else
            emit bookAddFailed(response.getMessage());
        break;
    case RequestType::UpdateBook:
        if (response.isSuccess())
            emit bookUpdated(response.getMessage());
        else
            emit bookUpdateFailed(response.getMessage());
        break;
    case RequestType::DeactivateBook:
        if (response.isSuccess())
            emit bookDeactivated(response.getMessage());
        else
            emit bookDeactivateFailed(response.getMessage());
        break;
    case RequestType::ReactivateBook:
        if (response.isSuccess()) emit bookReactivated(response.getMessage());
        else emit bookReactivateFailed(response.getMessage());
        break;
    case RequestType::ApplyDiscount:
        if (response.isSuccess()) emit discountApplied(response.getMessage());
        else emit discountApplyFailed(response.getMessage());
        break;
    default:
        break;
    }
}