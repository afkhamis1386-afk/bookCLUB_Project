#include "BookRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
BookRepository::BookRepository(){}
int BookRepository::insertBook(const Book &book){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Books "
        "(BookName, BookDescription, BookPrice, DiscountPercent, DiscountAmount, "
        "CoverImagePath, PDFfilePath, RegisteredIn, IsActive, IsDeleted, "
        "GenreID, CategoryID, AuthorID, PublisherUserID) "
        "OUTPUT INSERTED.BookID "
        "VALUES (:name, :desc, :price, :discPercent, :discAmount, :cover, :pdf, "
        ":registeredIn, :isActive, :isDeleted, :genreId, :categoryId, :authorId, :publisherId)");
    query.bindValue(":name", book.getBookName());
    query.bindValue(":desc", book.getBookDescription());
    query.bindValue(":price", book.getBookPrice());
    query.bindValue(":discPercent", book.getDiscountPercent());
    query.bindValue(":discAmount", book.getDiscountAmount());
    query.bindValue(":cover", book.getCoverImagePath());
    query.bindValue(":pdf", book.getPdfFilePath());
    query.bindValue(":registeredIn", book.getRegisteredIn());
    query.bindValue(":isActive", book.getIsActive());
    query.bindValue(":isDeleted", book.getIsDeleted());
    query.bindValue(":genreId", book.getGenreId());
    query.bindValue(":categoryId", book.getCategoryId());
    query.bindValue(":authorId", book.getAuthorId());
    query.bindValue(":publisherId", book.getPublisherUserId());
    if(!query.exec() || !query.next()){
        qWarning() << "خطا در ثبت کتاب جدید:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Book* BookRepository::loadBookById(int bookId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID, BookName, BookDescription, BookPrice, DiscountPercent, DiscountAmount, "
        "CoverImagePath, PDFfilePath, RegisteredIn, IsActive, IsDeleted, "
        "GenreID, CategoryID, AuthorID, PublisherUserID "
        "FROM Books WHERE BookID = :bookId");
    query.bindValue(":bookId", bookId);
    if(!query.exec() || !query.next()){
        qWarning() << "کتاب یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    Book *book = new Book(
        query.value(0).toInt(), query.value(1).toString(), query.value(2).toString(),
        query.value(3).toDouble(), query.value(4).toDouble(), query.value(5).toDouble(),
        query.value(6).toString(), query.value(7).toString(), query.value(8).toDateTime(),
        query.value(9).toBool(), query.value(10).toBool(), query.value(11).toInt(),
        query.value(12).toInt(), query.value(13).toInt(),  query.value(14).toInt());
    return book;
}
bool BookRepository::updateBook(const Book &book){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "UPDATE Books SET "
        "BookName = :name, BookDescription = :desc, BookPrice = :price, "
        "CoverImagePath = :cover, PDFfilePath = :pdf, "
        "GenreID = :genreId, CategoryID = :categoryId, AuthorID = :authorId "
        "WHERE BookID = :bookId");
    query.bindValue(":name", book.getBookName());
    query.bindValue(":desc", book.getBookDescription());
    query.bindValue(":price", book.getBookPrice());
    query.bindValue(":cover", book.getCoverImagePath());
    query.bindValue(":pdf", book.getPdfFilePath());
    query.bindValue(":genreId", book.getGenreId());
    query.bindValue(":categoryId", book.getCategoryId());
    query.bindValue(":authorId", book.getAuthorId());
    query.bindValue(":bookId", book.getBookId());
    if(!query.exec()){
        qWarning() << "خطا در ویرایش کتاب:" << query.lastError().text();
        return false;
    }
    return true;
}
bool BookRepository::updateDiscount(int bookId, double discountPercent, double discountAmount){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "UPDATE Books SET DiscountPercent = :percent, DiscountAmount = :amount "
        "WHERE BookID = :bookId");
    query.bindValue(":percent", discountPercent);
    query.bindValue(":amount", discountAmount);
    query.bindValue(":bookId", bookId);
    if (!query.exec()){
        qWarning() << "خطا در اعمال تخفیف:" << query.lastError().text();
        return false;
    }
    return true;
}
bool BookRepository::setActiveStatus(int bookId, bool isActive) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Books SET IsActive = :isActive WHERE BookID = :bookId");
    query.bindValue(":isActive", isActive);
    query.bindValue(":bookId", bookId);
    if(!query.exec()){
        qWarning() << "خطا در تغییر وضعیت فعال بودن کتاب:" << query.lastError().text();
        return false;
    }
    return true;
}
bool BookRepository::setDeletedStatus(int bookId, bool isDeleted){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Books SET IsDeleted = :isDeleted WHERE BookID = :bookId");
    query.bindValue(":isDeleted", isDeleted);
    query.bindValue(":bookId", bookId);
    if(!query.exec()){
        qWarning() << "خطا در حذف کتاب:" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<int> BookRepository::searchByName(const QString &nameQuery){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM Books WHERE BookName LIKE :pattern "
        "AND IsActive = 1 AND IsDeleted = 0");
    query.bindValue(":pattern", "%" + nameQuery + "%");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::searchByAuthorName(const QString &authorNameQuery){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT b.BookID FROM Books b JOIN Authors a ON b.AuthorID = a.AuthorID "
        "WHERE a.AuthorName LIKE :pattern AND b.IsActive = 1 AND b.IsDeleted = 0");
    query.bindValue(":pattern", "%" + authorNameQuery + "%");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::searchByPublisherName(const QString &publisherNameQuery){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT b.BookID FROM Books b JOIN Publishers p ON b.PublisherUserID = p.UserID "
        "WHERE p.PublicationName LIKE :pattern AND b.IsActive = 1 AND b.IsDeleted = 0");
    query.bindValue(":pattern", "%" + publisherNameQuery + "%");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getAllActiveBookIds(){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT b.BookID FROM Books b "
        "JOIN Genres g ON b.GenreID = g.GenreID "
        "WHERE b.IsActive = 1 AND b.IsDeleted = 0 "
        "ORDER BY g.GenreTitle ASC, b.BookName ASC");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getBooksByGenre(int genreId){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM Books WHERE GenreID = :genreId "
        "AND IsActive = 1 AND IsDeleted = 0 "
        "ORDER BY BookName ASC");
    query.bindValue(":genreId", genreId);
    if(query.exec()){
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getBooksByCategory(int categoryId){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM Books WHERE CategoryID = :categoryId "
        "AND IsActive = 1 AND IsDeleted = 0");
    query.bindValue(":categoryId", categoryId);
    if(query.exec()){
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getNewestBooks(int limit){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT TOP (:limit) BookID FROM Books "
        "WHERE IsActive = 1 AND IsDeleted = 0 "
        "ORDER BY RegisteredIn DESC");
    query.bindValue(":limit", limit);
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getFreeBooks(){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM Books WHERE BookPrice = 0 "
        "AND IsActive = 1 AND IsDeleted = 0");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getBooksByPublisher(int publisherUserId){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT BookID FROM Books WHERE PublisherUserID = :publisherId "
        "AND IsDeleted = 0");
    query.bindValue(":publisherId", publisherUserId);
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getAllBookIdsForAdmin(){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT BookID FROM Books WHERE IsDeleted = 0");
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
int BookRepository::getTotalBooksCountByPublisher(int publisherUserId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT COUNT(*) FROM Books WHERE PublisherUserID = :publisherId "
        "AND IsDeleted = 0");
    query.bindValue(":publisherId", publisherUserId);
    if(query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}
int BookRepository::getSoldCopiesCount(int bookId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT COUNT(*) FROM OrderItems oi "
        "JOIN Orders o ON oi.OrderID = o.OrderID "
        "JOIN Statuses s ON o.StatusID = s.StatusID "
        "WHERE oi.BookID = :bookId AND s.StatusTitle IN ('Paid', 'Completed')" );
    query.bindValue(":bookId", bookId);
    if(query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}
QVector<int> BookRepository::getTopSellingBooksByPublisher(int publisherUserId, int limit){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT TOP (:limit) b.BookID, "
        "COUNT(CASE WHEN s.StatusTitle IN ('Paid', 'Completed') THEN oi.OrderItemID END) AS SoldCount "
        "FROM Books b "
        "LEFT JOIN OrderItems oi ON b.BookID = oi.BookID "
        "LEFT JOIN Orders o ON oi.OrderID = o.OrderID "
        "LEFT JOIN Statuses s ON o.StatusID = s.StatusID "
        "WHERE b.PublisherUserID = :publisherId AND b.IsDeleted = 0 "
        "GROUP BY b.BookID "
        "ORDER BY SoldCount DESC" );
    query.bindValue(":limit", limit);
    query.bindValue(":publisherId", publisherUserId);
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getBestSellingBookIds(int limit){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT TOP (:limit) b.BookID, "
        "COUNT(CASE WHEN s.StatusTitle IN ('Paid', 'Completed') THEN oi.OrderItemID END) AS SoldCount, "
        "MAX(b.RegisteredIn) AS RegisteredIn "
        "FROM Books b "
        "LEFT JOIN OrderItems oi ON b.BookID = oi.BookID "
        "LEFT JOIN Orders o ON oi.OrderID = o.OrderID "
        "LEFT JOIN Statuses s ON o.StatusID = s.StatusID "
        "WHERE b.IsActive = 1 AND b.IsDeleted = 0 "
        "GROUP BY b.BookID "
        "ORDER BY SoldCount DESC, RegisteredIn DESC" );
    query.bindValue(":limit", limit);
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getMostPopularBookIds(int limit){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT TOP (:limit) b.BookID, "
        "ISNULL(AVG(CAST(r.Rating AS FLOAT)), -1) AS AvgRating, "
        "COUNT(r.RatingID) AS RatingCount, "
        "MAX(b.RegisteredIn) AS RegisteredIn "
        "FROM Books b "
        "LEFT JOIN Ratings r ON b.BookID = r.BookID "
        "WHERE b.IsActive = 1 AND b.IsDeleted = 0 "
        "GROUP BY b.BookID "
        "ORDER BY AvgRating DESC, RatingCount DESC, RegisteredIn DESC" );
    query.bindValue(":limit", limit);
    if(query.exec()){
        while(query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> BookRepository::getLeastSellingBooksByPublisher(int publisherUserId, int limit){
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT TOP (:limit) b.BookID, "
        "COUNT(CASE WHEN s.StatusTitle IN ('Paid', 'Completed') THEN oi.OrderItemID END) AS SoldCount "
        "FROM Books b "
        "LEFT JOIN OrderItems oi ON b.BookID = oi.BookID "
        "LEFT JOIN Orders o ON oi.OrderID = o.OrderID "
        "LEFT JOIN Statuses s ON o.StatusID = s.StatusID "
        "WHERE b.PublisherUserID = :publisherId AND b.IsDeleted = 0 "
        "GROUP BY b.BookID "
        "ORDER BY SoldCount ASC" );
    query.bindValue(":limit", limit);
    query.bindValue(":publisherId", publisherUserId);
    if(query.exec()){
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}