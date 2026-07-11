#include "ReviewManager.h"
#include "ReviewRepository.h"
#include "BookRepository.h"
#include "NotificationManager.h"
#include "ClientRegistry.h"
#include "../common/Review.h"
#include "../common/Book.h"
#include <memory>
ReviewManager::ReviewManager(){}
Response ReviewManager::submitReview(int userId, int bookId, const QString &commentText, int parentId){
    if(commentText.trimmed().isEmpty() || commentText.length() > 1000){
        return Response(ResponseStatus::ValidationFailed, "متن نظر نمی تواند خالی باشد و باید حداکثر ۱۰۰۰ کاراکتر باشد");
    }
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    ReviewRepository reviewRepo;
    if(parentId != -1){
        std::unique_ptr<Review> parentReview(reviewRepo.loadReviewById(parentId));
        if(!parentReview){
            return Response(ResponseStatus::NotFound, "نظر والد یافت نشد");
        }
        if(parentReview->getBookId() != bookId){
            return Response(ResponseStatus::ValidationFailed, "نظر والد متعلق به این کتاب نیست");
        }
    }
    Review newReview(userId, bookId, commentText.trimmed(), parentId);
    int newReviewId = reviewRepo.insertReview(newReview);
    if(newReviewId == -1){
        return Response(ResponseStatus::Error, "خطا در ثبت نظر");
    }
    if(parentId == -1){
        NotificationManager notifManager;
        notifManager.sendNotification(
            book->getPublisherUserId(),
            NotificationType::NewReviewForPublisher, "نظر جدید", QString("یک نظر جدید برای کتاب «%1» ثبت شد").arg(book->getBookName()), bookId, userId );
    }
    QVariantMap data;
    data["reviewId"] = newReviewId;
    return Response(ResponseStatus::Success, "نظر با موفقیت ثبت شد", data);
}
    QVariantMap liveData;
    liveData["bookId"] = bookId;
    liveData["reviewId"] = newReviewId;
    ClientRegistry::getInstance()->broadcastLiveUpdate("newReview", liveData);
    Response ReviewManager::editReview(int userId, int reviewId, const QString &newCommentText){
    if(newCommentText.trimmed().isEmpty() || newCommentText.length() > 1000) {
        return Response(ResponseStatus::ValidationFailed, "متن نظر نمی تواند خالی باشد و باید حداکثر ۱۰۰۰ کاراکتر باشد");
    }
    ReviewRepository reviewRepo;
    std::unique_ptr<Review> review(reviewRepo.loadReviewById(reviewId));
    if(!review){
        return Response(ResponseStatus::NotFound, "نظر یافت نشد");
    }
    if(review->getUserId() != userId){
        return Response(ResponseStatus::Unauthorized, "شما اجازه ویرایش این نظر را ندارید");
    }
    if(review->getIsDeleted()){
        return Response(ResponseStatus::Error, "نمی توانید نظر حذف شده را ویرایش کنید");
    }
    if(!reviewRepo.updateCommentText(reviewId, newCommentText.trimmed())){
        return Response(ResponseStatus::Error, "خطا در ویرایش نظر");
    }
    return Response(ResponseStatus::Success, "نظر با موفقیت ویرایش شد");
}
Response ReviewManager::deleteReview(int userId, int reviewId){
    ReviewRepository reviewRepo;
    std::unique_ptr<Review> review(reviewRepo.loadReviewById(reviewId));
    if(!review){
        return Response(ResponseStatus::NotFound, "نظر یافت نشد");
    }
    if(review->getUserId() != userId){
        return Response(ResponseStatus::Unauthorized, "شما اجازه حذف این نظر را ندارید");
    }
    if(!reviewRepo.setDeletedStatus(reviewId, true)){
        return Response(ResponseStatus::Error, "خطا در حذف نظر");
    }
    return Response(ResponseStatus::Success, "نظر با موفقیت حذف شد");
}
Response ReviewManager::deleteReviewByAdmin(int reviewId){
    ReviewRepository reviewRepo;
    std::unique_ptr<Review> review(reviewRepo.loadReviewById(reviewId));
    if(!review){
        return Response(ResponseStatus::NotFound, "نظر یافت نشد");
    }
    if(!reviewRepo.setDeletedStatus(reviewId, true)){
        return Response(ResponseStatus::Error, "خطا در حذف نظر");
    }
    return Response(ResponseStatus::Success, "نظر با موفقیت حذف شد");
}
Response ReviewManager::getReviewsForBook(int bookId){
    ReviewRepository reviewRepo;
    QVector<int> reviewIds = reviewRepo.getReviewIdsByBook(bookId);
    QVariantList reviewList;
    for(int reviewId : qAsConst(reviewIds)){
        std::unique_ptr<Review> review(reviewRepo.loadReviewById(reviewId));
        if(!review || review->isReply()) continue;
        QVariantMap reviewData;
        reviewData["reviewId"] = review->getReviewId();
        reviewData["userId"] = review->getUserId();
        reviewData["commentText"] = review->getCommentText();
        reviewData["reviewDate"] = review->getReviewDate();
        QVector<int> replyIds = reviewRepo.getRepliesOf(reviewId);
        QVariantList replyList;
        for(int replyId : qAsConst(replyIds)){
            std::unique_ptr<Review> reply(reviewRepo.loadReviewById(replyId));
            if (!reply) continue;
            QVariantMap replyData;
            replyData["reviewId"] = reply->getReviewId();
            replyData["userId"] = reply->getUserId();
            replyData["commentText"] = reply->getCommentText();
            replyData["reviewDate"] = reply->getReviewDate();
            replyList.append(replyData);
        }
        reviewData["replies"] = replyList;
        reviewList.append(reviewData);
    }
    QVariantMap data;
    data["reviews"] = reviewList;
    return Response(ResponseStatus::Success, "نظرات بازیابی شد", data);
}

