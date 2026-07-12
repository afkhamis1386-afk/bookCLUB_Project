#include "BookManager.h"
#include "BookRepository.h"
#include "GenreRepository.h"
#include "CategoryRepository.h"
#include "AuthorRepository.h"
#include "RatingRepository.h"
#include "UserRepository.h"
#include "ReadingProgressRepository.h"
#include "SavedBookRepository.h"
#include "NotificationManager.h"
#include "TimedDiscountRepository.h"
#include <QFile>
#include <QIODevice>
#include "DatabaseManager.h"
#include "PriceCalculator.h"
#include "../common/Book.h"
#include "../common/Genre.h"
#include "../common/Category.h"
#include "../common/TimedDiscount.h"
#include "../common/normaluser.h"
#include <memory>
#include <QDir>
#include <QUuid>
#include <QCoreApplication>
BookManager::BookManager(){}
static QString storageRootPath(){
    QString path = QCoreApplication::applicationDirPath() + "/BookClubStorage";
    QDir dir(path);
    if(!dir.exists()) dir.mkpath(".");
    QDir covers(path + "/covers");
    if(!covers.exists()) covers.mkpath(".");
    QDir pdfs(path + "/pdfs");
    if(!pdfs.exists()) pdfs.mkpath(".");
    return path;
}
Response BookManager::addBook(int publisherUserId, const QString &bookName, const QString &description, double price, const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
    const QByteArray &coverImageData, const QString &coverImageExtension, const QByteArray &pdfData)
{
    if(bookName.trimmed().isEmpty() || bookName.length() > 60){
        return Response(ResponseStatus::ValidationFailed, "نام کتاب نامعتبر است (حداکثر ۶۰ کاراکتر)");
    }
    if(description.trimmed().isEmpty()){
        return Response(ResponseStatus::ValidationFailed, "توضیحات کتاب نمی تواند خالی باشد");
    }
    if(price < 0){
        return Response(ResponseStatus::ValidationFailed, "قیمت کتاب نمی تواند منفی باشد");
    }
    if(pdfData.isEmpty()){
        return Response(ResponseStatus::ValidationFailed, "فایل PDF کتاب الزامی است");
    }
    static const qint64 MAX_PDF_SIZE = 100 * 1024 * 1024;
    static const qint64 MAX_COVER_SIZE = 10 * 1024 * 1024;
    if(pdfData.size() > MAX_PDF_SIZE){
        return Response(ResponseStatus::ValidationFailed, "حجم فایل PDF بیش از حد مجاز است");
    }
    if(!coverImageData.isEmpty() && coverImageData.size() > MAX_COVER_SIZE){
        return Response(ResponseStatus::ValidationFailed, "حجم عکس جلد بیش از حد مجاز است");
    }
    GenreRepository genreRepo;
    QVector<Genre> allGenres = genreRepo.getAllGenres();
    int genreId = -1;
    for(const Genre &g : qAsConst(allGenres)){
        if(g.getGenreTitle() == genreTitle.trimmed()){
            genreId = g.getGenreId();
            break;
        }
    }
    if(genreId == -1){
        return Response(ResponseStatus::ValidationFailed, "ژانر انتخاب شده معتبر نیست");
    }
    CategoryRepository categoryRepo;
    QVector<Category> allCategories = categoryRepo.getAllCategories();
    int categoryId = -1;
    for(const Category &c : qAsConst(allCategories)){
        if(c.getCategoryTitle() == categoryTitle.trimmed()){
            categoryId = c.getCategoryId();
            break;
        }
    }
    if(categoryId == -1){
        return Response(ResponseStatus::ValidationFailed, "دسته بندی انتخاب شده معتبر نیست");
    }

    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if(!db.transaction()){
        return Response(ResponseStatus::Error, "خطا در شروع تراکنش ثبت کتاب");
    }
    AuthorRepository authorRepo;
    int authorId = authorRepo.getOrCreateAuthor(authorName.trimmed());
    if(authorId == -1){
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در ثبت نویسنده");
    }
    QString uniqueId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString root = storageRootPath();
    QString coverRelativePath;
    if(!coverImageData.isEmpty()){
        QString ext = coverImageExtension.trimmed().isEmpty() ? "jpg" : coverImageExtension.trimmed();
        coverRelativePath = QString("covers/%1.%2").arg(uniqueId, ext);
        QFile coverFile(root + "/" + coverRelativePath);
        if(!coverFile.open(QIODevice::WriteOnly)){
            db.rollback();
            return Response(ResponseStatus::Error, "خطا در ذخیره عکس جلد روی سرور");
        }
        coverFile.write(coverImageData);
        coverFile.close();
    }
    QString pdfRelativePath = QString("pdfs/%1.pdf").arg(uniqueId);
    QFile pdfFile(root + "/" + pdfRelativePath);
    if(!pdfFile.open(QIODevice::WriteOnly)){
        db.rollback();
        if(!coverRelativePath.isEmpty()) QFile::remove(root + "/" + coverRelativePath);
        return Response(ResponseStatus::Error, "خطا در ذخیره فایل PDF روی سرور");
    }
    pdfFile.write(pdfData);
    pdfFile.close();
    Book newBook(bookName.trimmed(), description.trimmed(), price, genreId, categoryId, authorId, publisherUserId, coverRelativePath, pdfRelativePath);
    BookRepository bookRepo;
    int newBookId = bookRepo.insertBook(newBook);
    if(newBookId == -1){
        db.rollback();
        QFile::remove(root + "/" + pdfRelativePath);
        if(!coverRelativePath.isEmpty()) QFile::remove(root + "/" + coverRelativePath);
        return Response(ResponseStatus::Error, "خطا در ثبت کتاب");
    }
    if(!db.commit()){
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در نهایی سازی ثبت کتاب");
    }
    QVariantMap data;
    data["bookId"] = newBookId;
    UserRepository userRepoForNotif;
    QVector<int> interestedUserIds = userRepoForNotif.getUserIdsByFavoriteGenre(genreId);
    if(!interestedUserIds.isEmpty()){
        NotificationManager notifManager;
        notifManager.broadcastNotification( interestedUserIds, NotificationType::NewBookInFavouriteGenre, "کتاب جدید در ژانر مورد علاقه شما",
         QString("کتاب «%1» در ژانر مورد علاقه ی شما منتشر شد").arg(bookName.trimmed()), newBookId, publisherUserId );
    }
    return Response(ResponseStatus::Success, "کتاب با موفقیت ثبت شد", data);
}
Response BookManager::getCoverImageData(int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if(book->getCoverImagePath().isEmpty()){
        return Response(ResponseStatus::NotFound, "این کتاب عکس جلد ندارد");
    }
    QFile file(storageRootPath() + "/" + book->getCoverImagePath());
    if(!file.open(QIODevice::ReadOnly)){
        return Response(ResponseStatus::Error, "خطا در خواندن عکس جلد از سرور");
    }
    QVariantMap data;
    data["bookId"] = bookId;
    data["imageData"] = file.readAll();
    file.close();
    return Response(ResponseStatus::Success, "عکس جلد بازیابی شد", data);
}
Response BookManager::applyDiscount(int publisherUserId, int bookId, double discountPercent, double discountAmount){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if(book->getPublisherUserId() != publisherUserId){
        return Response(ResponseStatus::Unauthorized, "شما اجازه اعمال تخفیف روی این کتاب را ندارید");
    }
    if(discountPercent > 0 && discountAmount > 0){
        return Response(ResponseStatus::ValidationFailed, "فقط یکی از تخفیف درصدی یا مبلغی را می توانید تعیین کنید");
    }
    if(!book->setDiscountPercent(discountPercent)){
        return Response(ResponseStatus::ValidationFailed, "درصد تخفیف باید بین ۰ تا ۱۰۰ باشد");
    }
    if(!book->setDiscountAmount(discountAmount)){
        return Response(ResponseStatus::ValidationFailed, "مبلغ تخفیف نمی تواند منفی باشد");
    }
    if(!bookRepo.updateDiscount(bookId, discountPercent, discountAmount)){
        return Response(ResponseStatus::Error, "خطا در اعمال تخفیف");
    }
    if(discountPercent > 0 || discountAmount > 0){
        SavedBookRepository savedRepo;
        QVector<int> interestedUserIds = savedRepo.getUserIdsWhoSavedBook(bookId);
        if(!interestedUserIds.isEmpty()){
            NotificationManager notifManager;
            notifManager.broadcastNotification(
            interestedUserIds,
            NotificationType::DiscountOnSavedBook, "تخفیف روی کتاب ذخیره شده", QString("کتاب «%1» که ذخیره کرده اید تخفیف خورد").arg(book->getBookName()), bookId, publisherUserId );
        }
    }
    return Response(ResponseStatus::Success, "تخفیف با موفقیت اعمال شد");
}
Response BookManager::deactivateBook(int publisherUserId, int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if(book->getPublisherUserId() != publisherUserId){
        return Response(ResponseStatus::Unauthorized, "شما اجازه غیرفعال سازی این کتاب را ندارید");
    }
    if(!bookRepo.setActiveStatus(bookId, false)){
        return Response(ResponseStatus::Error, "خطا در غیرفعال سازی کتاب");
    }
    return Response(ResponseStatus::Success, "کتاب با موفقیت غیرفعال شد");
}
Response BookManager::reactivateBook(int publisherUserId, int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if(book->getPublisherUserId() != publisherUserId){
        return Response(ResponseStatus::Unauthorized, "شما اجازه فعال سازی این کتاب را ندارید");
    }
    if(!bookRepo.setActiveStatus(bookId, true)){
        return Response(ResponseStatus::Error, "خطا در فعال سازی کتاب");
    }
    return Response(ResponseStatus::Success, "کتاب با موفقیت فعال شد");
}
Response BookManager::getStorefrontBooks(){
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getAllActiveBookIds();
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(bookIds))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب ها بازیابی شد", data);
}
Response BookManager::getBooksByGenre(int genreId){
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getBooksByGenre(genreId);
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(bookIds))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب ها بازیابی شد", data);
}
Response BookManager::getBooksByCategory(int categoryId){
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getBooksByCategory(categoryId);
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(bookIds))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب ها بازیابی شد", data);
}
Response BookManager::getNewestBooks(int limit){
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getNewestBooks(limit);
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(bookIds))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب های جدید بازیابی شد", data);
}
Response BookManager::getFreeBooks(){
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getFreeBooks();
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(bookIds))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب های رایگان بازیابی شد", data);
}
Response BookManager::searchBooks(const QString &query){
    if(query.trimmed().isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "متن جستجو نمی تواند خالی باشد");
    }
    BookRepository bookRepo;
    QVector<int> byName = bookRepo.searchByName(query);
    QVector<int> byAuthor = bookRepo.searchByAuthorName(query);
    QVector<int> byPublisher = bookRepo.searchByPublisherName(query);
    QVector<int> combined;
    for(int id : qAsConst(byName)) if (!combined.contains(id)) combined.append(id);
    for(int id : qAsConst(byAuthor)) if (!combined.contains(id)) combined.append(id);
    for(int id : qAsConst(byPublisher)) if (!combined.contains(id)) combined.append(id);
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(combined))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "نتایج جستجو بازیابی شد", data);
}
Response BookManager::getBookDetails(int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    TimedDiscountRepository timedDiscountRepo;
    std::unique_ptr<TimedDiscount> activeDiscount(timedDiscountRepo.getActiveDiscountForBook(bookId));
    double timedPercent = activeDiscount ? activeDiscount->getDiscountPercent() : 0.0;
    double effectivePercent = PriceCalculator::calculateEffectivePercent(
    book->getDiscountPercent(), timedPercent);
    double finalPrice = PriceCalculator::calculateFinalPrice(
    book->getBookPrice(), effectivePercent, book->getDiscountAmount());
    RatingRepository ratingRepo;
    double avgRating = ratingRepo.getAverageRating(bookId);
    int ratingCount = ratingRepo.getRatingCount(bookId);
    QVariantMap data;
    data["bookId"] = book->getBookId();
    data["bookName"] = book->getBookName();
    data["description"] = book->getBookDescription();
    data["price"] = book->getBookPrice();
    data["finalPrice"] = finalPrice;
    data["coverImagePath"] = book->getCoverImagePath();
    data["genreId"] = book->getGenreId();
    data["categoryId"] = book->getCategoryId();
    data["authorId"] = book->getAuthorId();
    data["publisherUserId"] = book->getPublisherUserId();
    data["averageRating"] = avgRating;
    data["ratingCount"] = ratingCount;
    data["isActive"] = book->getIsActive();
    return Response(ResponseStatus::Success, "جزئیات کتاب بازیابی شد", data);
}
Response BookManager::getBookFileData(int userId, int bookId){
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if(!user || !user->hasPurchased(bookId)){
        return Response(ResponseStatus::Unauthorized, "شما این کتاب را خریداری نکرده اید");
    }
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    QFile file(storageRootPath() + "/" + book->getPdfFilePath());
    if(!file.open(QIODevice::ReadOnly)){
        return Response(ResponseStatus::Error, "خطا در خواندن فایل کتاب از سرور");
    }
    QVariantMap data;
    data["pdfData"] = file.readAll();
    file.close();
    ReadingProgressRepository progressRepo;
    data["lastPage"] = progressRepo.getLastPage(userId, bookId);
    return Response(ResponseStatus::Success, "فایل کتاب بازیابی شد", data);
}
Response BookManager::getRecommendedBooks(int userId){
    UserRepository userRepo;
    QVector<int> favoriteGenreIds = userRepo.getFavoriteGenreIds(userId);
    if(favoriteGenreIds.isEmpty()){
        return Response(ResponseStatus::Success, "ژانر مورد علاقه ثبت نشده", QVariantMap{{"bookIds", QVariantList()}});
    }
    BookRepository bookRepo;
    QVector<int> resultIds;
    for(int genreId : qAsConst(favoriteGenreIds)){
        QVector<int> genreBooks = bookRepo.getBooksByGenre(genreId);
        for(int id : qAsConst(genreBooks)){
            if(!resultIds.contains(id))
                resultIds.append(id);
        }
    }
    QVariantList bookList;
    for(int id : qAsConst(resultIds))
        bookList.append(id);
    QVariantMap data;
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "کتاب های پیشنهادی بازیابی شد", data);
}
Response BookManager::saveReadingProgress(int userId, int bookId, int lastPage){
    if(lastPage < 1){
        return Response(ResponseStatus::ValidationFailed, "شماره صفحه نامعتبر است");
    }
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if(!user || !user->hasPurchased(bookId)){
        return Response(ResponseStatus::Unauthorized, "شما این کتاب را خریداری نکرده اید");
    }
    ReadingProgressRepository progressRepo;
    if(!progressRepo.upsertProgress(userId, bookId, lastPage)){
        return Response(ResponseStatus::Error, "خطا در ذخیره پیشرفت مطالعه");
    }
    return Response(ResponseStatus::Success, "پیشرفت مطالعه ذخیره شد");
}