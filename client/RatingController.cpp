#include "RatingController.h"
RatingController::RatingController(NetworkManager *networkManager, QObject *parent):QObject(parent), networkManager(networkManager){
    connect(networkManager, &NetworkManager::responseReceived, this, &RatingController::onResponseReceived);
}
void RatingController::submitRating(int bookId, int ratingValue){
    if(bookId <= 0){
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if(ratingValue < 1 || ratingValue > 5){
        emit validationError("امتیاز باید بین ۱ تا ۵ باشد");
        return;
    }
    if(!networkManager->isConnected()){
        emit ratingSubmitFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->submitRating(bookId, ratingValue);
}
void RatingController::loadRatingSummary(int bookId){
    if(bookId <= 0){
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if(!networkManager->isConnected()){
        emit ratingSummaryLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getBookRatingSummary(bookId);
}
void RatingController::onResponseReceived(RequestType type, const Response &response){
    switch(type){
    case RequestType::SubmitRating:
        if(response.isSuccess())
            emit ratingSubmitted(response.getData().value("newAverage").toDouble(), response.getData().value("ratingCount").toInt(), response.getMessage());
        else
            emit ratingSubmitFailed(response.getMessage());
        break;
    case RequestType::GetBookRatingSummary:
        if(response.isSuccess())
            emit ratingSummaryLoaded(response.getData().value("averageRating").toDouble(), response.getData().value("ratingCount").toInt());
        else
            emit ratingSummaryLoadFailed(response.getMessage());
        break;
    default:
        break;
    }
}