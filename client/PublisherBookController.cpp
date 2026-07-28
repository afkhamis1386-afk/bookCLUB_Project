#include "PublisherBookController.h"
#include <QFile>
#include <QFileInfo>
PublisherBookController::PublisherBookController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager){
    connect(networkManager, &NetworkManager::responseReceived, this, &PublisherBookController::onResponseReceived);
}
void PublisherBookController::addBook(const QString &bookName, const QString &description, double price, const QString &genreTitle, const QString &categoryTitle,
                                      const QString &authorName, const QString &coverImageFilePath, const QString &pdfFilePath, double discountPercent) {
    if (bookName.trimmed().isEmpty() || bookName.length() > 60) {
        emit validationError("نام کتاب نامعتبر است (حداکثر ۶۰ کاراکتر)");
        return;
    }
    if (description.trimmed().isEmpty()) {
        emit validationError("توضیحات کتاب نمی تواند خالی باشد");
        return;
    }
    if (price < 0 || price > 99999999.99) {
        emit validationError("قیمت کتاب باید بین صفر و ۹۹٬۹۹۹٬۹۹۹٫۹۹ باشد");
        return;
    }
    if (discountPercent < 0 || discountPercent > 100) {
        emit validationError("درصد تخفیف باید بین ۰ تا ۱۰۰ باشد");
        return;
    }
    if (genreTitle.trimmed().isEmpty() || genreTitle.length() > 30 || categoryTitle.trimmed().isEmpty() || categoryTitle.length() > 50
        || authorName.trimmed().isEmpty() || authorName.length() > 60) {
        emit validationError("ژانر، دسته بندی و نام نویسنده باید معتبر و مطابق محدودیت طول فیلدها باشند");
        return;
    }
    if (pdfFilePath.trimmed().isEmpty()) {
        emit validationError("انتخاب فایل PDF الزامی است");
        return;
    }
    QFile pdfFile(pdfFilePath);
    if (!pdfFile.open(QIODevice::ReadOnly)) {
        emit bookFileReadFailed("امکان باز کردن فایل PDF انتخاب شده وجود ندارد");
        return;
    }
    QByteArray pdfData = pdfFile.readAll();
    pdfFile.close();
    QByteArray coverData;
    QString coverExtension;
    if (!coverImageFilePath.trimmed().isEmpty()) {
        QFile coverFile(coverImageFilePath);
        if (!coverFile.open(QIODevice::ReadOnly)) {
            emit bookFileReadFailed("امکان باز کردن عکس جلد انتخاب شده وجود ندارد");
            return;
        }
        coverData = coverFile.readAll();
        coverFile.close();
        coverExtension = QFileInfo(coverImageFilePath).suffix();
    }
    if (!networkManager->isConnected()) {
        emit bookAddFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->addBook(bookName.trimmed(), description.trimmed(), price, genreTitle.trimmed(), categoryTitle.trimmed(), authorName.trimmed(),
                            coverData, coverExtension, pdfData, discountPercent);
}
void PublisherBookController::updateBook(int bookId, const QString &bookName, const QString &description, double price,
                                         const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
                                         const QString &coverImageFilePath, const QString &pdfFilePath) {
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
    if (price < 0 || price > 99999999.99) {
        emit validationError("قیمت کتاب باید بین صفر و ۹۹٬۹۹۹٬۹۹۹٫۹۹ باشد");
        return;
    }
    if (genreTitle.trimmed().isEmpty() || genreTitle.length() > 30
        || categoryTitle.trimmed().isEmpty() || categoryTitle.length() > 50
        || authorName.trimmed().isEmpty() || authorName.length() > 60) {
        emit validationError("ژانر، دسته بندی و نام نویسنده باید معتبر و مطابق محدودیت طول فیلدها باشند");
        return;
    }
    QByteArray pdfData;
    if (!pdfFilePath.trimmed().isEmpty()) {
        QFile pdfFile(pdfFilePath);
        if (!pdfFile.open(QIODevice::ReadOnly)) {
            emit bookFileReadFailed("امکان باز کردن فایل PDF انتخاب شده وجود ندارد");
            return;
        }
        pdfData = pdfFile.readAll();
        pdfFile.close();
    }
    QByteArray coverData;
    QString coverExtension;
    if (!coverImageFilePath.trimmed().isEmpty()) {
        QFile coverFile(coverImageFilePath);
        if (!coverFile.open(QIODevice::ReadOnly)) {
            emit bookFileReadFailed("امکان باز کردن عکس جلد انتخاب شده وجود ندارد");
            return;
        }
        coverData = coverFile.readAll();
        coverFile.close();
        coverExtension = QFileInfo(coverImageFilePath).suffix();
    }
    if (!networkManager->isConnected()) {
        emit bookUpdateFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->updateBook(bookId, bookName.trimmed(), description.trimmed(), price, genreTitle.trimmed(), categoryTitle.trimmed(), authorName.trimmed(),
                               coverData, coverExtension, pdfData);
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
    if (discountPercent < 0 || discountAmount < 0 || discountAmount > 99999999.99) {
        emit validationError("مقدار تخفیف باید با محدودیت عددی پایگاه داده سازگار باشد");
        return;
    }
    if (!networkManager->isConnected()) { emit discountApplyFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->applyDiscount(bookId, discountPercent, discountAmount);
}
void PublisherBookController::applyTimedDiscount(int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate) {
    if (bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if (discountPercent <= 0 || discountPercent > 100) {
        emit validationError("درصد تخفیف زمان دار باید بین ۰ تا ۱۰۰ باشد");
        return;
    }
    if (!startDate.isValid() || !endDate.isValid()) {
        emit validationError("تاریخ شروع یا پایان نامعتبر است");
        return;
    }
    if (startDate >= endDate) {
        emit validationError("تاریخ شروع باید قبل از تاریخ پایان باشد");
        return;
    }
    if (!networkManager->isConnected()) { emit timedDiscountApplyFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->applyTimedDiscount(bookId, discountPercent, startDate, endDate);
}
void PublisherBookController::cancelTimedDiscount(int bookId) {
    if (bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if (!networkManager->isConnected()) { emit timedDiscountCancelFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->cancelTimedDiscount(bookId);
}
void PublisherBookController::loadBookForEdit(int bookId) {
    if (bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if (!networkManager->isConnected()) { emit bookDetailsForEditLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getBookDetails(bookId);
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
    case RequestType::ApplyTimedDiscount:
        if (response.isSuccess()) emit timedDiscountApplied(response.getMessage());
        else emit timedDiscountApplyFailed(response.getMessage());
        break;
    case RequestType::CancelTimedDiscount:
        if (response.isSuccess()) emit timedDiscountCancelled(response.getMessage());
        else emit timedDiscountCancelFailed(response.getMessage());
        break;
    case RequestType::GetBookDetails:
        if (response.isSuccess()) emit bookDetailsForEditLoaded(response.getData());
        else emit bookDetailsForEditLoadFailed(response.getMessage());
        break;
    default:
        break;
    }
}