#include "AdminManager.h"
#include "UserRepository.h"
#include "PublisherRepository.h"
#include "BookRepository.h"
#include "ReviewRepository.h"
#include "../common/normaluser.h"
#include "../common/publisher.h"
#include "../common/Book.h"
#include "../common/Review.h"
#include <memory>
AdminManager::AdminManager(){}
Response AdminManager::getAllUsers(){
    UserRepository userRepo;
    QVector<int> normalUserIds = userRepo.getAllNormalUserIds();
    PublisherRepository publisherRepo;
    QVector<int> publisherIds = publisherRepo.getAllPublisherIds();
    QVariantList userList;
    for(int userId : qAsConst(normalUserIds)){
        std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
        if (!user) continue;
        QVariantMap userData;
        userData["userId"] = user->getUserId();
        userData["username"] = user->getUsername();
        userData["role"] = "NormalUser";
        userData["isBlocked"] = user->getIsBlocked();
        userData["isDeleted"] = user->getIsDeleted();
        userData["registerDate"] = user->getRegisterDate();
        userList.append(userData);
    }
    for(int userId : qAsConst(publisherIds)){
        std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(userId));
        if(!publisher) continue;
        QVariantMap userData;
        userData["userId"] = publisher->getUserId();
        userData["username"] = publisher->getUsername();
        userData["role"] = "Publisher";
        userData["isBlocked"] = publisher->getIsBlocked();
        userData["isDeleted"] = publisher->getIsDeleted();
        userData["registerDate"] = publisher->getRegisterDate();
        userData["publicationName"] = publisher->getPublicationName();
        userList.append(userData);
    }
    QVariantMap data;
    data["users"] = userList;
    return Response(ResponseStatus::Success, "لیست کاربران بازیابی شد", data);
}
Response AdminManager::getNormalUserDetails(int userId){
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if(!user){
        return Response(ResponseStatus::NotFound, "کاربر عادی یافت نشد");
    }
    QVariantMap data;
    data["userId"] = user->getUserId();
    data["username"] = user->getUsername();
    data["isBlocked"] = user->getIsBlocked();
    data["isDeleted"] = user->getIsDeleted();
    data["registerDate"] = user->getRegisterDate();
    data["purchasedCount"] = user->getPurchasedCount();
    QVariantList genreList;
    for(int genreId : user->getFavoriteGenres())
        genreList.append(genreId);
    data["favoriteGenres"] = genreList;
    return Response(ResponseStatus::Success, "اطلاعات کاربر عادی بازیابی شد", data);
}
Response AdminManager::getPublisherDetails(int userId){
    PublisherRepository publisherRepo;
    std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(userId));
    if(!publisher){
        return Response(ResponseStatus::NotFound, "ناشر یافت نشد");
    }
    QVariantMap data;
    data["userId"] = publisher->getUserId();
    data["username"] = publisher->getUsername();
    data["firstName"] = publisher->getFirstName();
    data["lastName"] = publisher->getLastName();
    data["email"] = publisher->getEmail();
    data["publicationName"] = publisher->getPublicationName();
    data["isBlocked"] = publisher->getIsBlocked();
    data["isDeleted"] = publisher->getIsDeleted();
    data["registerDate"] = publisher->getRegisterDate();
    data["publishedBooksCount"] = publisher->getPublishedBooksCount();
    return Response(ResponseStatus::Success, "اطلاعات ناشر بازیابی شد", data);
}
Response AdminManager::blockUser(int userId){
    UserRepository userRepo;
    if(!userRepo.updateBlockedStatus(userId, true)){
        return Response(ResponseStatus::Error, "خطا در مسدودسازی کاربر");
    }
    return Response(ResponseStatus::Success, "کاربر با موفقیت مسدود شد");
}
Response AdminManager::unblockUser(int userId){
    UserRepository userRepo;
    if(!userRepo.updateBlockedStatus(userId, false)){
        return Response(ResponseStatus::Error, "خطا در رفع مسدودیت کاربر");
    }
    return Response(ResponseStatus::Success, "مسدودیت کاربر با موفقیت رفع شد");
}
Response AdminManager::deleteUser(int userId){
    UserRepository userRepo;
    if(!userRepo.updateDeletedStatus(userId, true)){
        return Response(ResponseStatus::Error, "خطا در حذف کاربر");
    }
    return Response(ResponseStatus::Success, "حساب کاربری با موفقیت حذف شد");
}
Response AdminManager::setUserActiveStatus(int targetUserId, bool active) {
    UserRepository userRepo;
    if(!userRepo.updateActiveStatus(targetUserId, active)){
        return Response(ResponseStatus::Error, "خطا در تغییر وضعیت فعال سازی کاربر");
    }
    return Response(ResponseStatus::Success, active ? "کاربر فعال شد" : "کاربر غیرفعال شد");
}
Response AdminManager::getAllBooks(){
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getAllActiveBookIds();
    QVariantList bookList;
    for(int bookId : qAsConst(bookIds)){
        std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
        if (!book) continue;
        QVariantMap bookData;
        bookData["bookId"] = book->getBookId();
        bookData["bookName"] = book->getBookName();
        bookData["publisherUserId"] = book->getPublisherUserId();
        bookData["isActive"] = book->getIsActive();
        bookData["isDeleted"] = book->getIsDeleted();
        bookList.append(bookData);
    }
    QVariantMap data;
    data["books"] = bookList;
    return Response(ResponseStatus::Success, "لیست تمامی کتاب ها بازیابی شد", data);
}
Response AdminManager::getBookDetailsForReview(int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    QVariantMap data;
    data["bookId"] = book->getBookId();
    data["bookName"] = book->getBookName();
    data["description"] = book->getBookDescription();
    data["price"] = book->getBookPrice();
    data["publisherUserId"] = book->getPublisherUserId();
    data["isActive"] = book->getIsActive();
    data["isDeleted"] = book->getIsDeleted();
    data["registeredIn"] = book->getRegisteredIn();
    return Response(ResponseStatus::Success, "جزئیات کتاب برای بررسی بازیابی شد", data);
}
Response AdminManager::removeInvalidBook(int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if(!bookRepo.setDeletedStatus(bookId, true)){
        return Response(ResponseStatus::Error, "خطا در حذف کتاب نامعتبر");
    }
    return Response(ResponseStatus::Success, "کتاب نامعتبر با موفقیت حذف شد");
}
Response AdminManager::getAllReviews(){
    ReviewRepository reviewRepo;
    QVector<int> reviewIds = reviewRepo.getAllReviewIds();
    QVariantList reviewList;
    for(int reviewId : qAsConst(reviewIds)){
        std::unique_ptr<Review> review(reviewRepo.loadReviewById(reviewId));
        if (!review) continue;
        QVariantMap reviewData;
        reviewData["reviewId"] = review->getReviewId();
        reviewData["userId"] = review->getUserId();
        reviewData["bookId"] = review->getBookId();
        reviewData["commentText"] = review->getCommentText();
        reviewData["isDeleted"] = review->getIsDeleted();
        reviewData["reviewDate"] = review->getReviewDate();
        reviewList.append(reviewData);
    }
    QVariantMap data;
    data["reviews"] = reviewList;
    return Response(ResponseStatus::Success, "لیست تمامی نظرات بازیابی شد", data);
}
Response AdminManager::removeInappropriateReview(int reviewId){
    ReviewRepository reviewRepo;
    std::unique_ptr<Review> review(reviewRepo.loadReviewById(reviewId));
    if(!review){
        return Response(ResponseStatus::NotFound, "نظر یافت نشد");
    }
    if(!reviewRepo.setDeletedStatus(reviewId, true)){
        return Response(ResponseStatus::Error, "خطا در حذف نظر نامناسب");
    }
    return Response(ResponseStatus::Success, "نظر نامناسب با موفقیت حذف شد");
}