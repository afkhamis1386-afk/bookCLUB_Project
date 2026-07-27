#include "Book.h"
Book::Book():bookId(-1), bookPrice(0.0), discountPercent(0.0), discountAmount(0.0), registeredIn(QDateTime::currentDateTime()), isActive(true), isDeleted(false), genreId(-1), categoryId(-1), authorId(-1), publisherUserId(-1){}
Book::Book(const QString &bN, const QString &bD, double bP, int gI, int cI, int auI, int pUI, const QString &cIP, const QString &pFP)
    :bookId(-1), bookName(bN), bookDescription(bD), bookPrice(bP >= 0 && bP <= 99999999.99 ? bP : 0.0), discountPercent(0.0), discountAmount(0.0), coverImagePath(cIP), pdfFilePath(pFP), registeredIn(QDateTime::currentDateTime()), isActive(true), isDeleted(false),
    genreId(gI), categoryId(cI), authorId(auI), publisherUserId(pUI){}
Book::Book(int bI, const QString &bN, const QString &bD, double bP, double dP, double dA, const QString &cIP, const QString &pFP, const QDateTime &rIn, bool iA, bool iD, int gI, int cI, int auI, int pUI)
    :bookId(bI), bookName(bN), bookDescription(bD), bookPrice(bP >= 0 && bP <= 99999999.99 ? bP : 0.0), discountPercent(dP >= 0 && dP <= 100 ? dP : 0.0), discountAmount(dA >= 0 && dA <= 99999999.99 ? dA : 0.0), coverImagePath(cIP), pdfFilePath(pFP), registeredIn(rIn), isActive(iA), isDeleted(iD),
    genreId(gI), categoryId(cI), authorId(auI), publisherUserId(pUI){}
Book::~Book(){}
int Book::getBookId() const { return bookId; }
QString Book::getBookName() const { return bookName; }
QString Book::getBookDescription() const { return bookDescription; }
double Book::getBookPrice() const { return bookPrice; }
double Book::getDiscountPercent() const { return discountPercent; }
double Book::getDiscountAmount() const { return discountAmount; }
QString Book::getCoverImagePath() const { return coverImagePath; }
QString Book::getPdfFilePath() const { return pdfFilePath; }
QDateTime Book::getRegisteredIn() const { return registeredIn; }
bool Book::getIsActive() const { return isActive; }
bool Book::getIsDeleted() const { return isDeleted; }
int Book::getGenreId() const { return genreId; }
int Book::getCategoryId() const { return categoryId; }
int Book::getAuthorId() const { return authorId; }
int Book::getPublisherUserId() const { return publisherUserId; }
void Book::setBookId(int id){
    bookId = id;
}
bool Book::setBookName(const QString &name){
    if(name.trimmed().isEmpty() || name.length() > 60)
        return false;
    bookName = name;
    return true;
}
bool Book::setBookDescription(const QString &description){
    if (description.trimmed().isEmpty())
        return false;
    bookDescription = description;
    return true;
}
bool Book::setBookPrice(double price){
    if(price < 0 || price > 99999999.99)
        return false;
    bookPrice = price;
    return true;
}
bool Book::setDiscountPercent(double percent){
    if(percent < 0 || percent > 100)
        return false;
    discountPercent = percent;
    return true;
}
bool Book::setDiscountAmount(double amount){
    if(amount < 0 || amount > 99999999.99)
        return false;
    discountAmount = amount;
    return true;
}
bool Book::setCoverImagePath(const QString &path){
    if(path.length() > 600)
        return false;
    coverImagePath = path;
    return true;
}
bool Book::setPdfFilePath(const QString &path){
    if(path.length() > 600)
        return false;
    pdfFilePath = path;
    return true;
}
void Book::setGenreId(int genreId){
    this->genreId = genreId;
}
void Book::setCategoryId(int categoryId){
    this->categoryId = categoryId;
}
void Book::setAuthorId(int authorId){
    this->authorId = authorId;
}
void Book::deactivate(){
    isActive = false;
}
void Book::reactivate(){
    isActive = true;
}
void Book::markAsDeleted(){
    isDeleted = true;
    isActive = false;
}
bool Book::isAvailableForPurchase() const{
    return isActive && !isDeleted;
}
double Book::getFinalPrice() const {
    double priceAfterPercent = bookPrice - (bookPrice * (discountPercent / 100.0));
    double finalPrice = priceAfterPercent - discountAmount;
    if(finalPrice < 0)
    finalPrice = 0;
    return finalPrice;
}
QDataStream &operator<<(QDataStream &out, const Book &book) {
    out << book.bookId << book.bookName << book.bookDescription << book.bookPrice << book.discountPercent << book.discountAmount << book.coverImagePath << book.pdfFilePath << book.registeredIn << book.isActive << book.isDeleted
    << book.genreId << book.categoryId << book.authorId << book.publisherUserId;
    return out;
}
QDataStream &operator>>(QDataStream &in, Book &book) {
    in >> book.bookId >> book.bookName >> book.bookDescription >> book.bookPrice >> book.discountPercent >> book.discountAmount >> book.coverImagePath >> book.pdfFilePath >> book.registeredIn >> book.isActive >> book.isDeleted
    >> book.genreId >> book.categoryId >> book.authorId >> book.publisherUserId;
    return in;
}