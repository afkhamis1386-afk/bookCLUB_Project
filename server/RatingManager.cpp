#include "RatingManager.h"
#include "RatingRepository.h"
#include "BookRepository.h"
#include "NotificationManager.h"
#include "../common/Rating.h"
#include "../common/Book.h"
#include <memory>
RatingManager::RatingManager(){}
Response RatingManager::submitRating(int userId, int bookId, int ratingValue){
    if(ratingValue < 1 || ratingValue > 5){
        return Response(ResponseStatus::ValidationFailed, "امتیاز باید بین ۱ تا ۵ باشد");
    }
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    Rating newRating(userId, bookId, ratingValue);
    RatingRepository ratingRepo;
    if(!ratingRepo.upsertRating(newRating)){
        return Response(ResponseStatus::Error, "خطا در ثبت امتیاز");
    }
    NotificationManager notifManager;
    notifManager.sendNotification(
        book->getPublisherUserId(),
        NotificationType::NewReviewForPublisher, "امتیاز جدید", QString("کتاب «%1» یک امتیاز جدید دریافت کرد").arg(book->getBookName()), bookId, userId);
    QVariantMap data;
    data["newAverage"] = ratingRepo.getAverageRating(bookId);
    data["ratingCount"] = ratingRepo.getRatingCount(bookId);
    return Response(ResponseStatus::Success, "امتیاز با موفقیت ثبت شد", data);
}
Response RatingManager::getBookRatingSummary(int bookId){
    RatingRepository ratingRepo;
    QVariantMap data;
    data["averageRating"] = ratingRepo.getAverageRating(bookId);
    data["ratingCount"] = ratingRepo.getRatingCount(bookId);
    return Response(ResponseStatus::Success, "خلاصه امتیازات بازیابی شد", data);
}
