#include "AdminManager.h"
#include "UserRepository.h"
#include "PublisherRepository.h"
#include "BookRepository.h"
#include "AuthorRepository.h"
#include "ReviewRepository.h"
#include "GenreRepository.h"
#include "../common/normaluser.h"
#include "../common/publisher.h"
#include "../common/Admin.h"
#include "AdminRepository.h"
#include "../common/Book.h"
#include "../common/Review.h"
#include <memory>
#include <QHash>
AdminManager::AdminManager(){}
Response AdminManager::getAllUsers(){
    UserRepository userRepo;
    QVector<int> normalUserIds = userRepo.getAllNormalUserIds();
    PublisherRepository publisherRepo;
    QVector<int> publisherIds = publisherRepo.getAllPublisherIds();
    QVariantList userList;
    GenreRepository genreRepo;
    QHash<int, QString> genreTitlesById;
    const QVector<Genre> allGenres = genreRepo.getAllGenres();
    for (const Genre &genre : allGenres)
        genreTitlesById.insert(genre.getGenreId(), genre.getGenreTitle());

    for(int userId : qAsConst(normalUserIds)){
        std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
        if (!user) continue;
        QVariantMap userData;
        userData["userId"] = user->getUserId();
        userData["username"] = user->getUsername();
        userData["role"] = "NormalUser";
        userData["purchasedCount"] = user->getPurchasedCount();
        QVariantList favoriteGenreTitles;
        for (int genreId : user->getFavoriteGenres()) {
            const auto title = genreTitlesById.constFind(genreId);
            if (title != genreTitlesById.constEnd())
                favoriteGenreTitles.append(title.value());
        }
        userData["favoriteGenreTitles"] = favoriteGenreTitles;
        userData["isBlocked"] = user->getIsBlocked();
        userData["isDeleted"] = user->getIsDeleted();
        userData["isActive"] = user->getIsActive();
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
        userData["isActive"] = publisher->getIsActive();
        userData["registerDate"] = publisher->getRegisterDate();
        userData["firstName"] = publisher->getFirstName();
        userData["lastName"] = publisher->getLastName();
        userData["publicationName"] = publisher->getPublicationName();
        userData["publisherLicenseNumber"] = publisher->getPublisherLicenseNumber();
        userData["email"] = publisher->getEmail();
        userList.append(userData);
    }
    AdminRepository adminRepoForList;
    QVector<int> adminIds = adminRepoForList.getAllAdminIds();
    for(int userId : qAsConst(adminIds)){
        std::unique_ptr<Admin> admin(adminRepoForList.loadAdminById(userId));
        if (!admin) continue;
        QVariantMap userData;
        userData["userId"] = admin->getUserId();
        userData["username"] = admin->getUsername();
        userData["role"] = "Admin";
        userData["isBlocked"] = admin->getIsBlocked();
        userData["isDeleted"] = admin->getIsDeleted();
        userData["isActive"] = admin->getIsActive();
        userData["registerDate"] = admin->getRegisterDate();
        userData["firstName"] = admin->getFirstName();
        userData["lastName"] = admin->getLastName();
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
    data["isActive"] = user->getIsActive();
    data["registerDate"] = user->getRegisterDate();
    data["purchasedCount"] = user->getPurchasedCount();
    QVariantList genreList;
    for(int genreId : user->getFavoriteGenres())
        genreList.append(genreId);
    data["favoriteGenres"] = genreList;
    QVariantList genreTitles;
    GenreRepository genreRepo;
    for (int genreId : user->getFavoriteGenres()) {
        std::unique_ptr<Genre> genre(genreRepo.loadGenreById(genreId));
        if (genre)
            genreTitles.append(genre->getGenreTitle());
    }
    data["favoriteGenreTitles"] = genreTitles;
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
    data["publisherLicenseNumber"] = publisher->getPublisherLicenseNumber();
    data["isBlocked"] = publisher->getIsBlocked();
    data["isDeleted"] = publisher->getIsDeleted();
    data["isActive"] = publisher->getIsActive();
    data["registerDate"] = publisher->getRegisterDate();
    data["publishedBooksCount"] = publisher->getPublishedBooksCount();
    return Response(ResponseStatus::Success, "اطلاعات ناشر بازیابی شد", data);
}
Response AdminManager::blockUser(int actingAdminUserId, int userId){
    if(userId == actingAdminUserId){
        return Response(ResponseStatus::ValidationFailed, "نمی توانید حساب کاربری خودتان را مسدود کنید");
    }
    UserRepository userRepo;
    UserRole targetRole;
    if(!userRepo.findRoleById(userId, targetRole)){
        return Response(ResponseStatus::NotFound, "کاربر مورد نظر یافت نشد");
    }
    if(targetRole == UserRole::Admin){
        return Response(ResponseStatus::ValidationFailed, "امکان مسدود کردن حساب مدیر سیستم وجود ندارد");
    }
    if(!userRepo.updateBlockedStatus(userId, true)){
        return Response(ResponseStatus::Error, "خطا در مسدودسازی کاربر");
    }
    return Response(ResponseStatus::Success, "کاربر با موفقیت مسدود شد");
}

Response AdminManager::unblockUser(int actingAdminUserId, int userId){
    Q_UNUSED(actingAdminUserId)
    UserRepository userRepo;
    if(!userRepo.updateBlockedStatus(userId, false)){
        return Response(ResponseStatus::Error, "خطا در رفع مسدودیت کاربر");
    }
    return Response(ResponseStatus::Success, "مسدودیت کاربر با موفقیت رفع شد");
}

Response AdminManager::deleteUser(int actingAdminUserId, int userId){
    if(userId == actingAdminUserId){
        return Response(ResponseStatus::ValidationFailed, "نمی توانید حساب کاربری خودتان را حذف کنید");
    }
    UserRepository userRepo;
    UserRole targetRole;
    if(!userRepo.findRoleById(userId, targetRole)){
        return Response(ResponseStatus::NotFound, "کاربر مورد نظر یافت نشد");
    }
    if(targetRole == UserRole::Admin){
        return Response(ResponseStatus::ValidationFailed, "امکان حذف حساب مدیر سیستم وجود ندارد");
    }
    if(!userRepo.updateDeletedStatus(userId, true)){
        return Response(ResponseStatus::Error, "خطا در حذف کاربر");
    }
    return Response(ResponseStatus::Success, "حساب کاربری با موفقیت حذف شد");
}

Response AdminManager::setUserActiveStatus(int actingAdminUserId, int targetUserId, bool active){
    if(!active && targetUserId == actingAdminUserId){
        return Response(ResponseStatus::ValidationFailed, "نمی توانید حساب کاربری خودتان را غیرفعال کنید");
    }
    UserRepository userRepo;
    UserRole targetRole;
    if(!userRepo.findRoleById(targetUserId, targetRole)){
        return Response(ResponseStatus::NotFound, "کاربر مورد نظر یافت نشد");
    }
    if(!active && targetRole == UserRole::Admin){
        return Response(ResponseStatus::ValidationFailed, "امکان غیرفعال کردن حساب مدیر سیستم وجود ندارد");
    }
    if(!userRepo.updateActiveStatus(targetUserId, active)){
        return Response(ResponseStatus::Error, "خطا در تغییر وضعیت فعال سازی کاربر");
    }
    return Response(ResponseStatus::Success, active ? "کاربر فعال شد" : "کاربر غیرفعال شد");
}
Response AdminManager::getAllBooks(){
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getAllBookIdsForAdmin();
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

    AuthorRepository authorRepo;
    std::unique_ptr<Author> author(authorRepo.loadAuthorById(book->getAuthorId()));
    if(!author){
        return Response(ResponseStatus::Error, "اطلاعات نویسنده کتاب یافت نشد");
    }

    GenreRepository genreRepo;
    std::unique_ptr<Genre> genre(genreRepo.loadGenreById(book->getGenreId()));
    if(!genre){
        return Response(ResponseStatus::Error, "اطلاعات ژانر کتاب یافت نشد");
    }

    QVariantMap data;
    data["bookId"] = book->getBookId();
    data["bookName"] = book->getBookName();
    data["authorName"] = author->getAuthorName();
    data["price"] = book->getBookPrice();
    data["genreTitle"] = genre->getGenreTitle();
    data["description"] = book->getBookDescription();
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
Response AdminManager::updateBook(int bookId, const QString &bookName, const QString &description, double price){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if(!book->setBookName(bookName)){
        return Response(ResponseStatus::ValidationFailed, "نام کتاب نامعتبر است");
    }
    if(!book->setBookDescription(description)){
        return Response(ResponseStatus::ValidationFailed, "توضیحات کتاب نامعتبر است");
    }
    if(!book->setBookPrice(price)){
        return Response(ResponseStatus::ValidationFailed, "قیمت کتاب نامعتبر است");
    }
    if(!bookRepo.updateBook(*book)){
        return Response(ResponseStatus::Error, "خطا در ویرایش کتاب توسط مدیر");
    }
    return Response(ResponseStatus::Success, "کتاب با موفقیت توسط مدیر ویرایش شد");
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
Response AdminManager::createAdmin(const QString &username, const QString &plainPassword, const QString &firstName, const QString &lastName, bool requireNoExistingAdmin) {
    AdminRepository adminRepo;
    if(requireNoExistingAdmin && adminRepo.anyAdminExists()){
        return Response(ResponseStatus::Error, "این مسیر فقط برای راه اندازی اولیه سیستم است و دیگر در دسترس نیست");
    }
    if(!User::isValidUsername(username)){
        return Response(ResponseStatus::ValidationFailed, "نام کاربری نامعتبر است");
    }
    if(!User::isStrongPassword(plainPassword)){
        return Response(ResponseStatus::ValidationFailed, "رمز عبور ضعیف است");
    }
    if(firstName.trimmed().isEmpty() || lastName.trimmed().isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "نام و نام خانوادگی الزامی است");
    }
    Admin newAdmin(username, plainPassword, QString(), firstName, lastName);
    int newAdminId = adminRepo.insertAdmin(newAdmin);
    if(newAdminId == -1){
        return Response(ResponseStatus::Error, "خطا در ثبت ادمین (شاید نام کاربری تکراری است)");
    }
    QVariantMap data;
    data["userId"] = newAdminId;
    return Response(ResponseStatus::Success, "ادمین با موفقیت ساخته شد", data);
}