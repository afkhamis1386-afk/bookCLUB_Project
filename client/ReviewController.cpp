#include "ReviewController.h"
ReviewController::ReviewController(NetworkManager *networkManager, QObject *parent):QObject(parent), networkManager(networkManager){
    connect(networkManager, &NetworkManager::responseReceived, this, &ReviewController::onResponseReceived);
}
void ReviewController::submitReview(int bookId, const QString &commentText, int parentId){
    if(bookId <= 0){
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if(commentText.trimmed().isEmpty() || commentText.length() > 1000){
        emit validationError("متن نظر نمی تواند خالی باشد و باید حداکثر ۱۰۰۰ کاراکتر باشد");
        return;
    }
    if(!networkManager->isConnected()){
        emit reviewSubmitFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->submitReview(bookId, commentText.trimmed(), parentId);
}
void ReviewController::editReview(int reviewId, const QString &newCommentText){
    if(reviewId <= 0){
        emit validationError("شناسه نظر نامعتبر است");
        return;
    }
    if(newCommentText.trimmed().isEmpty() || newCommentText.length() > 1000){
        emit validationError("متن نظر نمی تواند خالی باشد و باید حداکثر ۱۰۰۰ کاراکتر باشد");
        return;
    }
    if(!networkManager->isConnected()){
        emit reviewEditFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->editReview(reviewId, newCommentText.trimmed());
}
void ReviewController::deleteReview(int reviewId){
    if(reviewId <= 0){
        emit validationError("شناسه نظر نامعتبر است");
        return;
    }
    if(!networkManager->isConnected()){
        emit reviewDeleteFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->deleteReview(reviewId);
}
void ReviewController::loadReviewsForBook(int bookId){
    if(bookId <= 0){
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if(!networkManager->isConnected()){
        emit reviewsLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getReviewsForBook(bookId);
}
void ReviewController::onResponseReceived(RequestType type, const Response &response){
    switch(type){
    case RequestType::SubmitReview:
        if(response.isSuccess())
            emit reviewSubmitted(response.getData().value("reviewId").toInt(), response.getMessage());
        else
            emit reviewSubmitFailed(response.getMessage());
        break;
    case RequestType::EditReview:
        if(response.isSuccess())
            emit reviewEdited(response.getMessage());
        else
            emit reviewEditFailed(response.getMessage());
        break;
    case RequestType::DeleteReview:
        if(response.isSuccess())
            emit reviewDeleted(response.getMessage());
        else
            emit reviewDeleteFailed(response.getMessage());
        break;
    case RequestType::GetReviewsForBook:
        if(response.isSuccess())
            emit reviewsLoaded(response.getData().value("reviews").toList());
        else
            emit reviewsLoadFailed(response.getMessage());
        break;
    default:
        break;
    }
}