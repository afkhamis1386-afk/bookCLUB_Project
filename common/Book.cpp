#include "Book.h"
#include <stdexcept>
Book::Book():bookId(-1), title(""), authorId(-1), genreId(-1), publisherId(-1), basePrice(0.0), discountPrc(0.0), discountStartTime(QDateTime()), discountEndTime(QDateTime()), description(""), coverImagePath(""), pdfFilePath(""), reviews(), salesCount(0)
{}
Book::Book(int id, const QString& ti, int auId, int geId, int pubId, double pr, const QString& des, const QString& coverPath, const QString& pdfPath)
    :bookId(id), title(ti), authorId(auId), genreId(geId), publisherId(pubId), basePrice(pr), discountPrc(0.0), discountStartTime(QDateTime()), discountEndTime(QDateTime()), description(des), coverImagePath(coverPath), pdfFilePath(pdfPath), reviews(), salesCount(0)
{
    if(title.trimmed().isEmpty())
        throw std::invalid_argument("Book constructor: Title cannot be empty.");
    if(basePrice < 0.0)
        throw std::invalid_argument("Book constructor: Base price cannot be negative.");
}
Book::~Book(){}
void Book::set_Title(const QString& title){
    if(title.trimmed().isEmpty()) throw std::invalid_argument("Title cannot be empty.");
    this->title = title;
}
void Book::set_AuthorId(int auId) { this->authorId = auId; }
void Book::set_GenreId(int geId) { this->genreId = geId; }
void Book::set_PublisherId(int pubId) { this->publisherId = pubId; }
void Book::set_DiscountPrc(double discountPrc){
    if(discountPrc < 0.0 || discountPrc > 100.0)
        throw std::invalid_argument("Book::set_DiscountPrc: Discount percentage must be between 0 and 100.");
    this->discountPrc = discountPrc;
}
void Book::set_DiscountStartTime(const QDateTime& startTime){ this->discountStartTime = startTime; }
void Book::set_DiscountEndTime(const QDateTime& endTime){ this->discountEndTime = endTime; }
void Book::set_Description(const QString& description){ this->description = description; }
void Book::set_CoverImagePath(const QString& coverPath){ this->coverImagePath = coverPath; }
void Book::set_PdfFilePath(const QString& pdfPath){ this->pdfFilePath = pdfPath; }
void Book::set_SalesCount(int salesCount){
    if(salesCount < 0)
        throw std::invalid_argument("Book::set_SalesCount: Sales count cannot be negative.");
    this->salesCount = salesCount;
}
void Book::addReview(int reviewId){ this->reviews.append(reviewId); }
double Book::get_FinalPrice() const {
    if(discountStartTime.isValid() && discountEndTime.isValid()){
        QDateTime current = QDateTime::currentDateTime();
        if(current >= discountStartTime && current <= discountEndTime){
            return basePrice - (basePrice * discountPrc / 100.0);
        }
    }
    return basePrice;
}
void Book::updateDetails(const QString& title, double price){
    set_Title(title);
    if(price < 0.0)
        throw std::invalid_argument("Book::updateDetails: Price cannot be negative.");
    this->basePrice = price;
}
QDataStream& operator<<(QDataStream& out, const Book& book){
    out << book.bookId << book.title << book.authorId << book.genreId << book.publisherId << book.basePrice
        << book.discountPrc << book.discountStartTime << book.discountEndTime << book.description << book.coverImagePath
        << book.pdfFilePath << book.reviews << book.salesCount;
    return out;
}
QDataStream& operator>>(QDataStream& in, Book& book){
    in >> book.bookId >> book.title >> book.authorId >> book.genreId >> book.publisherId >> book.basePrice
       >> book.discountPrc >> book.discountStartTime >> book.discountEndTime >> book.description >> book.coverImagePath
       >> book.pdfFilePath >> book.reviews >> book.salesCount;
    return in;
}