#include "AdminController.h"
AdminController::AdminController(NetworkManager *networkManager, QObject *parent):QObject(parent), networkManager(networkManager){
    connect(networkManager, &NetworkManager::responseReceived, this, &AdminController::onResponseReceived);
}
bool AdminController::ensureConnected(const QString &failMessage){
    if(!networkManager->isConnected()){
        emit validationError(failMessage);
        return false;
    }
    return true;
}
void AdminController::loadAllUsers(){
    if(!networkManager->isConnected()) { emit usersLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getAllUsers();
}
void AdminController::loadNormalUserDetails(int userId){
    if(userId <= 0) { emit validationError("شناسه کاربر نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit normalUserDetailsLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getNormalUserDetails(userId);
}
void AdminController::loadPublisherDetails(int userId){
    if(userId <= 0) { emit validationError("شناسه کاربر نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit publisherDetailsLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getPublisherDetails(userId);
}
void AdminController::blockUser(int userId){
    if(userId <= 0) { emit validationError("شناسه کاربر نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit userBlockFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->blockUser(userId);
}
void AdminController::unblockUser(int userId){
    if(userId <= 0) { emit validationError("شناسه کاربر نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit userUnblockFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->unblockUser(userId);
}
void AdminController::deleteUser(int userId){
    if(userId <= 0) { emit validationError("شناسه کاربر نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit userDeleteFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->deleteUser(userId);
}
void AdminController::setUserActiveStatus(int userId, bool active){
    if(userId <= 0) { emit validationError("شناسه کاربر نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit userActiveStatusChangeFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->setUserActiveStatus(userId, active);
}
void AdminController::loadAllBooks(){
    if(!networkManager->isConnected()) { emit allBooksLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getAllBooksAdmin();
}
void AdminController::loadBookDetailsForReview(int bookId){
    if(bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit bookDetailsForReviewLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getBookDetailsForReview(bookId);
}
void AdminController::deleteBook(int bookId){
    if(bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit bookDeleteFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->deleteBook(bookId);
}
void AdminController::loadAllReviews(){
    if(!networkManager->isConnected()) { emit allReviewsLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getAllReviews();
}
void AdminController::deleteReview(int reviewId){
    if(reviewId <= 0) { emit validationError("شناسه نظر نامعتبر است"); return; }
    if(!networkManager->isConnected()) { emit reviewDeleteFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->deleteReviewByAdmin(reviewId);
}
void AdminController::createAdmin(const QString &username, const QString &password, const QString &securityAnswer, const QString &firstName, const QString &lastName) {
    if(username.trimmed().isEmpty() || password.isEmpty() || firstName.trimmed().isEmpty() || lastName.trimmed().isEmpty()){
        emit validationError("تمامی فیلدها الزامی هستند");
        return;
    }
    if(!networkManager->isConnected()) { emit adminCreateFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->createAdditionalAdmin(username, password, securityAnswer, firstName, lastName);
}
void AdminController::onResponseReceived(RequestType type, const Response &response){
    switch(type){
    case RequestType::GetAllUsers:
        if(response.isSuccess()) emit usersLoaded(response.getData().value("users").toList());
        else emit usersLoadFailed(response.getMessage());
        break;
    case RequestType::GetNormalUserDetails:
        if(response.isSuccess()) emit normalUserDetailsLoaded(response.getData());
        else emit normalUserDetailsLoadFailed(response.getMessage());
        break;
    case RequestType::GetPublisherDetails:
        if(response.isSuccess()) emit publisherDetailsLoaded(response.getData());
        else emit publisherDetailsLoadFailed(response.getMessage());
        break;
    case RequestType::BlockUser:
        if(response.isSuccess()) emit userBlocked(response.getMessage());
        else emit userBlockFailed(response.getMessage());
        break;
    case RequestType::UnblockUser:
        if(response.isSuccess()) emit userUnblocked(response.getMessage());
        else emit userUnblockFailed(response.getMessage());
        break;
    case RequestType::DeleteUser:
        if(response.isSuccess()) emit userDeleted(response.getMessage());
        else emit userDeleteFailed(response.getMessage());
        break;
    case RequestType::SetUserActiveStatus:
        if(response.isSuccess()) emit userActiveStatusChanged(response.getMessage());
        else emit userActiveStatusChangeFailed(response.getMessage());
        break;
    case RequestType::GetAllBooksAdmin:
        if(response.isSuccess()) emit allBooksLoaded(response.getData().value("books").toList());
        else emit allBooksLoadFailed(response.getMessage());
        break;
    case RequestType::GetBookDetailsForReview:
        if(response.isSuccess()) emit bookDetailsForReviewLoaded(response.getData());
        else emit bookDetailsForReviewLoadFailed(response.getMessage());
        break;
    case RequestType::DeleteBook:
        if(response.isSuccess()) emit bookDeleted(response.getMessage());
        else emit bookDeleteFailed(response.getMessage());
        break;
    case RequestType::GetAllReviews:
        if(response.isSuccess()) emit allReviewsLoaded(response.getData().value("reviews").toList());
        else emit allReviewsLoadFailed(response.getMessage());
        break;
    case RequestType::DeleteReviewByAdmin:
        if(response.isSuccess()) emit reviewDeleted(response.getMessage());
        else emit reviewDeleteFailed(response.getMessage());
        break;
    case RequestType::CreateAdditionalAdmin:
        if(response.isSuccess()) emit adminCreated(response.getMessage());
        else emit adminCreateFailed(response.getMessage());
        break;
    default:
        break;
    }
}