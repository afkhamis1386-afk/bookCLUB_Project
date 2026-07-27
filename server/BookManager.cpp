#include "BookManager.h"
#include "BookRepository.h"
#include "OrderRepository.h"
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
Response BookManager::addBook(int publisherUserId, const QString &bookName, const QString &description, double price, const QString &genreTitle, const QString &categoryTitle, const QString &authorName,  const QByteArray &coverImageData, const QString &coverImageExtension, const QByteArray &pdfData, double discountPercent)
{
    if(bookName.trimmed().isEmpty() || bookName.length() > 60){
        return Response(ResponseStatus::ValidationFailed, "نام کتاب نامعتبر است (حداکثر ۶۰ کاراکتر)");
    }
    if(description.trimmed().isEmpty()){
        return Response(ResponseStatus::ValidationFailed, "توضیحات کتاب نمی تواند خالی باشد");
    }
    if(price < 0 || price > 99999999.99){
        return Response(ResponseStatus::ValidationFailed, "قیمت کتاب باید بین صفر و ۹۹٬۹۹۹٬۹۹۹٫۹۹ باشد");
    }
    if(discountPercent < 0 || discountPercent > 100){
        return Response(ResponseStatus::ValidationFailed, "درصد تخفیف باید بین ۰ تا ۱۰۰ باشد");
    }
    if(genreTitle.trimmed().isEmpty() || genreTitle.length() > 30
        || categoryTitle.trimmed().isEmpty() || categoryTitle.length() > 50
        || authorName.trimmed().isEmpty() || authorName.length() > 60){
        return Response(ResponseStatus::ValidationFailed, "ژانر، دسته بندی و نام نویسنده با محدودیت های پایگاه داده سازگار نیستند");
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
    int categoryId = categoryRepo.getOrCreateCategory(categoryTitle.trimmed());
    if(categoryId == -1){
        return Response(ResponseStatus::ValidationFailed, "دسته بندی انتخاب شده معتبر نیست (حداکثر ۵۰ کاراکتر)");
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
    newBook.setDiscountPercent(discountPercent);
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
        notifManager.broadcastNotification( interestedUserIds, NotificationType::NewBookInFavouriteGenre, "کتاب جدید در ژانر مورد علاقه شما", QString("کتاب «%1» در ژانر مورد علاقه ی شما منتشر شد").arg(bookName.trimmed()), newBookId, publisherUserId );
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
Response BookManager::updateBook(int actingUserId, UserRole actingRole, int bookId, const QString &bookName, const QString &description, double price, const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
    const QByteArray &coverImageData, const QString &coverImageExtension, const QByteArray &pdfData){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    const bool isAdmin = actingRole == UserRole::Admin;
    const bool isOwnerPublisher = actingRole == UserRole::Publisher && book->getPublisherUserId() == actingUserId;
    if(!isAdmin && !isOwnerPublisher){
        return Response(ResponseStatus::Unauthorized, "شما اجازه ویرایش این کتاب را ندارید");
    }
    if(!book->setBookName(bookName)){
        return Response(ResponseStatus::ValidationFailed, "نام کتاب نامعتبر است");
    }
    if(!book->setBookDescription(description)){
        return Response(ResponseStatus::ValidationFailed, "توضیحات کتاب نامعتبر است");
    }
    if(!book->setBookPrice(price)){
        return Response(ResponseStatus::ValidationFailed, "قیمت کتاب باید بین صفر و ۹٬۹۹۹٬۹۹۹ باشد");
    }
    if(genreTitle.trimmed().isEmpty() || genreTitle.length() > 30
        || categoryTitle.trimmed().isEmpty() || categoryTitle.length() > 50
        || authorName.trimmed().isEmpty() || authorName.length() > 60){
        return Response(ResponseStatus::ValidationFailed, "ژانر، دسته بندی و نام نویسنده با محدودیت های پایگاه داده سازگار نیستند");
    }
    static const qint64 MAX_PDF_SIZE = 100 * 1024 * 1024;
    static const qint64 MAX_COVER_SIZE = 10 * 1024 * 1024;
    if(pdfData.size() > MAX_PDF_SIZE){
        return Response(ResponseStatus::ValidationFailed, "حجم فایل PDF بیش از حد مجاز است");
    }
    if(coverImageData.size() > MAX_COVER_SIZE){
        return Response(ResponseStatus::ValidationFailed, "حجم عکس جلد بیش از حد مجاز است");
    }
    if(!genreTitle.trimmed().isEmpty()){
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
        book->setGenreId(genreId);
    }
    if(!categoryTitle.trimmed().isEmpty()){
        CategoryRepository categoryRepo;
        int categoryId = categoryRepo.getOrCreateCategory(categoryTitle.trimmed());
        if(categoryId == -1){
            return Response(ResponseStatus::ValidationFailed, "دسته بندی انتخاب شده معتبر نیست (حداکثر ۵۰ کاراکتر)");
        }
        book->setCategoryId(categoryId);
    }
    if(!authorName.trimmed().isEmpty()){
        AuthorRepository authorRepo;
        int authorId = authorRepo.getOrCreateAuthor(authorName.trimmed());
        if(authorId == -1){
            return Response(ResponseStatus::Error, "خطا در ثبت نویسنده");
        }
        book->setAuthorId(authorId);
    }
    QString root = storageRootPath();
    QString newCoverRelativePath, newPdfRelativePath;
    QString uniqueId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    if(!coverImageData.isEmpty()){
        QString ext = coverImageExtension.trimmed().isEmpty() ? "jpg" : coverImageExtension.trimmed();
        newCoverRelativePath = QString("covers/%1.%2").arg(uniqueId, ext);
        QFile coverFile(root + "/" + newCoverRelativePath);
        if(!coverFile.open(QIODevice::WriteOnly)){
            return Response(ResponseStatus::Error, "خطا در ذخیره عکس جلد روی سرور");
        }
        coverFile.write(coverImageData);
        coverFile.close();
    }
    if(!pdfData.isEmpty()){
        newPdfRelativePath = QString("pdfs/%1.pdf").arg(uniqueId);
        QFile pdfFile(root + "/" + newPdfRelativePath);
        if(!pdfFile.open(QIODevice::WriteOnly)){
            if(!newCoverRelativePath.isEmpty()) QFile::remove(root + "/" + newCoverRelativePath);
            return Response(ResponseStatus::Error, "خطا در ذخیره فایل PDF روی سرور");
        }
        pdfFile.write(pdfData);
        pdfFile.close();
    }
    QString oldCoverPath = book->getCoverImagePath();
    QString oldPdfPath = book->getPdfFilePath();
    if(!newCoverRelativePath.isEmpty()) book->setCoverImagePath(newCoverRelativePath);
    if(!newPdfRelativePath.isEmpty()) book->setPdfFilePath(newPdfRelativePath);
    if(!bookRepo.updateBook(*book)){
        if(!newCoverRelativePath.isEmpty()) QFile::remove(root + "/" + newCoverRelativePath);
        if(!newPdfRelativePath.isEmpty()) QFile::remove(root + "/" + newPdfRelativePath);
        return Response(ResponseStatus::Error, "خطا در به روزرسانی کتاب");
    }
    if(!newCoverRelativePath.isEmpty() && !oldCoverPath.isEmpty() && oldCoverPath != newCoverRelativePath)
        QFile::remove(root + "/" + oldCoverPath);
    if(!newPdfRelativePath.isEmpty() && !oldPdfPath.isEmpty() && oldPdfPath != newPdfRelativePath)
        QFile::remove(root + "/" + oldPdfPath);
    return Response(ResponseStatus::Success, "کتاب با موفقیت ویرایش شد");
}
Response BookManager::applyDiscount(int actingUserId, UserRole actingRole, int bookId, double discountPercent, double discountAmount){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    const bool isAdmin = actingRole == UserRole::Admin;
    const bool isOwnerPublisher = actingRole == UserRole::Publisher
                                  && book->getPublisherUserId() == actingUserId;
    if(!isAdmin && !isOwnerPublisher){
        return Response(ResponseStatus::Unauthorized, "شما اجازه اعمال تخفیف روی این کتاب را ندارید");
    }
    if(discountPercent > 0 && discountAmount > 0){
        return Response(ResponseStatus::ValidationFailed, "فقط یکی از تخفیف درصدی یا مبلغی را می توانید تعیین کنید");
    }
    if(!book->setDiscountPercent(discountPercent)){
        return Response(ResponseStatus::ValidationFailed, "درصد تخفیف باید بین ۰ تا ۱۰۰ باشد");
    }
    if(!book->setDiscountAmount(discountAmount)){
        return Response(ResponseStatus::ValidationFailed, "مبلغ تخفیف باید بین صفر و ۹۹٬۹۹۹٬۹۹۹٫۹۹ باشد");
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
                NotificationType::DiscountOnSavedBook, "تخفیف روی کتاب ذخیره شده", QString("کتاب «%1» که ذخیره کرده اید تخفیف خورد").arg(book->getBookName()), bookId, book->getPublisherUserId() );
        }
    }
    return Response(ResponseStatus::Success, "تخفیف با موفقیت اعمال شد");
}
Response BookManager::applyTimedDiscount(int actingUserId, UserRole actingRole, int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    const bool isAdmin = actingRole == UserRole::Admin;
    const bool isOwnerPublisher = actingRole == UserRole::Publisher
                                  && book->getPublisherUserId() == actingUserId;
    if(!isAdmin && !isOwnerPublisher){
        return Response(ResponseStatus::Unauthorized, "شما اجازه اعمال تخفیف روی این کتاب را ندارید");
    }
    if(discountPercent <= 0 || discountPercent > 100){
        return Response(ResponseStatus::ValidationFailed, "درصد تخفیف زمان دار باید بین ۰ تا ۱۰۰ باشد");
    }
    if(!startDate.isValid() || !endDate.isValid()){
        return Response(ResponseStatus::ValidationFailed, "تاریخ شروع یا پایان تخفیف نامعتبر است");
    }
    if(startDate >= endDate){
        return Response(ResponseStatus::ValidationFailed, "تاریخ شروع باید قبل از تاریخ پایان باشد");
    }
    if(endDate <= QDateTime::currentDateTime()){
        return Response(ResponseStatus::ValidationFailed, "تاریخ پایان تخفیف باید در آینده باشد");
    }
    TimedDiscount timedDiscount(bookId, discountPercent, startDate, endDate);
    TimedDiscountRepository timedDiscountRepo;
    int newDiscountId = timedDiscountRepo.insertTimedDiscount(timedDiscount);
    if(newDiscountId == -1){
        return Response(ResponseStatus::Error, "خطا در ثبت تخفیف زمان دار");
    }
    SavedBookRepository savedRepo;
    QVector<int> interestedUserIds = savedRepo.getUserIdsWhoSavedBook(bookId);
    if(!interestedUserIds.isEmpty()){
        NotificationManager notifManager;
        notifManager.broadcastNotification(
            interestedUserIds,
            NotificationType::DiscountOnSavedBook, "تخفیف زمان دار روی کتاب ذخیره شده",
            QString("کتاب «%1» که ذخیره کرده اید تخفیف زمان دار خورد").arg(book->getBookName()),
            bookId, book->getPublisherUserId() );
    }
    QVariantMap data;
    data["discountId"] = newDiscountId;
    return Response(ResponseStatus::Success, "تخفیف زمان دار با موفقیت اعمال شد", data);
}
Response BookManager::cancelTimedDiscount(int actingUserId, UserRole actingRole, int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    const bool isAdmin = actingRole == UserRole::Admin;
    const bool isOwnerPublisher = actingRole == UserRole::Publisher
                                  && book->getPublisherUserId() == actingUserId;
    if(!isAdmin && !isOwnerPublisher){
        return Response(ResponseStatus::Unauthorized, "شما اجازه لغو تخفیف این کتاب را ندارید");
    }
    TimedDiscountRepository timedDiscountRepo;
    std::unique_ptr<TimedDiscount> current(timedDiscountRepo.getCurrentOrUpcomingDiscountForBook(bookId));
    if(!current){
        return Response(ResponseStatus::Success, "تخفیف زمان دار فعال یا آینده ای برای این کتاب وجود ندارد");
    }
    if(!timedDiscountRepo.deleteDiscount(current->getDiscountId())){
        return Response(ResponseStatus::Error, "خطا در لغو تخفیف زمان دار");
    }
    return Response(ResponseStatus::Success, "تخفیف زمان دار با موفقیت لغو شد");
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
Response BookManager::getBestSellers(int limit){
    if(limit <= 0) limit = 10;
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getBestSellingBookIds(limit);
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(bookIds))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب های پرفروش بازیابی شد", data);
}
Response BookManager::getPopularBooks(int limit){
    if(limit <= 0) limit = 10;
    BookRepository bookRepo;
    QVector<int> bookIds = bookRepo.getMostPopularBookIds(limit);
    QVariantMap data;
    QVariantList bookList;
    for(int id : qAsConst(bookIds))
        bookList.append(id);
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "لیست کتاب های محبوب بازیابی شد", data);
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
    GenreRepository genreRepo;
    std::unique_ptr<Genre> genre(genreRepo.loadGenreById(book->getGenreId()));
    CategoryRepository categoryRepo;
    std::unique_ptr<Category> category(categoryRepo.loadCategoryById(book->getCategoryId()));
    AuthorRepository authorRepo;
    std::unique_ptr<Author> author(authorRepo.loadAuthorById(book->getAuthorId()));
    QVariantMap data;
    data["bookId"] = book->getBookId();
    data["bookName"] = book->getBookName();
    data["description"] = book->getBookDescription();
    data["price"] = book->getBookPrice();
    data["finalPrice"] = finalPrice;
    data["coverImagePath"] = book->getCoverImagePath();
    data["hasCoverImage"] = !book->getCoverImagePath().trimmed().isEmpty();
    data["hasPdfFile"] = !book->getPdfFilePath().trimmed().isEmpty();
    data["genreId"] = book->getGenreId();
    data["categoryId"] = book->getCategoryId();
    data["authorId"] = book->getAuthorId();
    data["genreTitle"] = genre ? genre->getGenreTitle() : QString();
    data["categoryTitle"] = category ? category->getCategoryTitle() : QString();
    data["authorName"] = author ? author->getAuthorName() : QString();
    data["publisherUserId"] = book->getPublisherUserId();
    data["averageRating"] = avgRating;
    data["ratingCount"] = ratingCount;
    data["isActive"] = book->getIsActive();
    data["discountPercent"] = book->getDiscountPercent();
    data["discountAmount"] = book->getDiscountAmount();
    if(activeDiscount){
        data["timedDiscountId"] = activeDiscount->getDiscountId();
        data["timedDiscountPercent"] = activeDiscount->getDiscountPercent();
        data["timedDiscountStart"] = activeDiscount->getStartDate();
        data["timedDiscountEnd"] = activeDiscount->getEndDate();
    } else {
        std::unique_ptr<TimedDiscount> upcoming(timedDiscountRepo.getCurrentOrUpcomingDiscountForBook(bookId));
        if(upcoming){
            data["timedDiscountId"] = upcoming->getDiscountId();
            data["timedDiscountPercent"] = upcoming->getDiscountPercent();
            data["timedDiscountStart"] = upcoming->getStartDate();
            data["timedDiscountEnd"] = upcoming->getEndDate();
        }
    }
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
Response BookManager::getPurchasedBooks(int userId){
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if(!user){
        return Response(ResponseStatus::NotFound, "کاربر یافت نشد");
    }
    QVariantList bookList;
    BookRepository bookRepo;
    for(int bookId : user->getPurchaseHistory()){
        QVariantMap bookData;
        bookData["bookId"] = bookId;
        std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
        bookData["bookName"] = book ? book->getBookName() : QString("کتاب #%1").arg(bookId);
        bookList.append(bookData);
    }
    QVariantMap data;
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "کتاب های خریداری شده بازیابی شدند", data);
}
Response BookManager::claimFreeBook(int userId, int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if(book->getBookPrice() != 0){
        return Response(ResponseStatus::ValidationFailed, "این کتاب رایگان نیست");
    }
    if(!book->isAvailableForPurchase()){
        return Response(ResponseStatus::Error, "این کتاب در حال حاضر در دسترس نیست");
    }
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if(user && user->hasPurchased(bookId)){
        return Response(ResponseStatus::ValidationFailed, "شما قبلاً این کتاب را دریافت کرده اید");
    }
    OrderRepository orderRepo;
    if(!orderRepo.addBookToLibrary(userId, bookId)){
        return Response(ResponseStatus::Error, "خطا در افزودن کتاب به کتابخانه شما");
    }
    return Response(ResponseStatus::Success, "کتاب رایگان به کتابخانه شما اضافه شد");
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