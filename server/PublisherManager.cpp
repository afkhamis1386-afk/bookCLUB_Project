#include "PublisherManager.h"
#include "PublisherRepository.h"
#include "BookRepository.h"
#include "RatingRepository.h"
#include "../common/publisher.h"
#include "../common/Book.h"
#include <memory>

PublisherManager::PublisherManager() {}
Response PublisherManager::getProfile(int publisherUserId) {
    PublisherRepository publisherRepo;
    std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(publisherUserId));
    if (!publisher) {
        return Response(ResponseStatus::NotFound, "ناشر یافت نشد");
    }
    QVariantMap data;
    data["userId"] = publisher->getUserId();
    data["username"] = publisher->getUsername();
    data["firstName"] = publisher->getFirstName();
    data["lastName"] = publisher->getLastName();
    data["email"] = publisher->getEmail();
    data["shortDescription"] = publisher->getShortDescription();
    data["publicationName"] = publisher->getPublicationName();
    data["publisherLicenseNumber"] = publisher->getPublisherLicenseNumber();
    data["registerDate"] = publisher->getRegisterDate();
    return Response(ResponseStatus::Success, "پروفایل ناشر بازیابی شد", data);
}
Response PublisherManager::updateProfile(int publisherUserId, const QString &firstName, const QString &lastName, const QString &email, const QString &shortDescription, const QString &publicationName) {
    if (firstName.trimmed().isEmpty() || firstName.length() > 30) {
        return Response(ResponseStatus::ValidationFailed, "نام نامعتبر است (حداکثر ۳۰ کاراکتر)");
    }
    if (lastName.trimmed().isEmpty() || lastName.length() > 30) {
        return Response(ResponseStatus::ValidationFailed, "نام خانوادگی نامعتبر است (حداکثر ۳۰ کاراکتر)");
    }
    if (email.trimmed().isEmpty() || email.length() > 120) {
        return Response(ResponseStatus::ValidationFailed, "ایمیل نامعتبر است");
    }
    if (publicationName.trimmed().isEmpty() || publicationName.length() > 50) {
        return Response(ResponseStatus::ValidationFailed, "نام انتشارات نامعتبر است (حداکثر ۵۰ کاراکتر)");
    }
    if (shortDescription.length() > 500) {
        return Response(ResponseStatus::ValidationFailed, "توضیحات کوتاه بیش از حد مجاز است (حداکثر ۵۰۰ کاراکتر)");
    }
    PublisherRepository publisherRepo;
    std::unique_ptr<Publisher> current(publisherRepo.loadPublisherById(publisherUserId));
    if (!current) {
        return Response(ResponseStatus::NotFound, "ناشر یافت نشد");
    }
    if (current->getEmail() != email.trimmed() && publisherRepo.isEmailTaken(email.trimmed())) {
        return Response(ResponseStatus::Error, "این ایمیل قبلاً توسط ناشر دیگری ثبت شده است");
    }
    if (!publisherRepo.updateProfile(publisherUserId, firstName.trimmed(), lastName.trimmed(), email.trimmed(), shortDescription.trimmed(), publicationName.trimmed())) {
        return Response(ResponseStatus::Error, "خطا در به روزرسانی پروفایل");
    }
    return Response(ResponseStatus::Success, "پروفایل با موفقیت به روزرسانی شد");
}
Response PublisherManager::getMyBooks(int publisherUserId) {
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getBooksByPublisher(publisherUserId);
    QVariantList bookList;
    for (int bookId : bookIds) {
        std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
        if (!book) continue;
        QVariantMap bookData;
        bookData["bookId"] = book->getBookId();
        bookData["bookName"] = book->getBookName();
        bookData["price"] = book->getBookPrice();
        bookData["discountPercent"] = book->getDiscountPercent();
        bookData["discountAmount"] = book->getDiscountAmount();
        bookData["isActive"] = book->getIsActive();
        bookData["isDeleted"] = book->getIsDeleted();
        bookList.append(bookData);
    }
    QVariantMap data;
    data["books"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب های ناشر بازیابی شد", data);
}
Response PublisherManager::getFullDashboard(int publisherUserId) {
    BookRepository bookRepo;
    RatingRepository ratingRepo;
    PublisherRepository publisherRepo;
    int totalBooksCount = bookRepo.getTotalBooksCountByPublisher(publisherUserId);
    QVector<int> bookIds = bookRepo.getBooksByPublisher(publisherUserId);
    double totalRevenue = publisherRepo.getTotalRevenue(publisherUserId);
    QVariantList bookStats;
    for (int bookId : bookIds) {
        std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
        if (!book) continue;
        double avgRating = ratingRepo.getAverageRating(bookId);
        int ratingCount = ratingRepo.getRatingCount(bookId);
        int soldCount = bookRepo.getSoldCopiesCount(bookId);
        QVariantMap bookStat;
        bookStat["bookId"] = book->getBookId();
        bookStat["bookName"] = book->getBookName();
        bookStat["averageRating"] = avgRating;
        bookStat["ratingCount"] = ratingCount;
        bookStat["soldCount"] = soldCount;
        bookStat["isActive"] = book->getIsActive();
        bookStats.append(bookStat);
    }
    QVector<int> topSellingIds = bookRepo.getTopSellingBooksByPublisher(publisherUserId, 5);
    QVariantList topSellingList;
    for (int bookId : topSellingIds) {
        std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
        if (!book) continue;
        QVariantMap item;
        item["bookId"] = book->getBookId();
        item["bookName"] = book->getBookName();
        item["soldCount"] = bookRepo.getSoldCopiesCount(bookId);
        topSellingList.append(item);
    }
    QVector<int> leastSellingIds = bookRepo.getLeastSellingBooksByPublisher(publisherUserId, 5);
    QVariantList leastSellingList;
    for (int bookId : leastSellingIds) {
        std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
        if (!book) continue;
        QVariantMap item;
        item["bookId"] = book->getBookId();
        item["bookName"] = book->getBookName();
        item["soldCount"] = bookRepo.getSoldCopiesCount(bookId);
        leastSellingList.append(item);
    }
    QVariantMap data;
    data["totalBooksCount"] = totalBooksCount;
    data["totalRevenue"] = totalRevenue;
    data["books"] = bookStats;
    data["topSellingBooks"] = topSellingList;
    data["leastSellingBooks"] = leastSellingList;
    return Response(ResponseStatus::Success, "داشبورد کامل ناشر بازیابی شد", data);
}