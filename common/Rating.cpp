#include "Rating.h"
Rating::Rating():ratingId(-1), userId(-1), bookId(-1), ratingValue(0){}
Rating::Rating(int userId, int bookId, int ratingValue):ratingId(-1), userId(userId), bookId(bookId), ratingValue(ratingValue >= 1 && ratingValue <= 5 ? ratingValue : 0){}
Rating::Rating(int ratingId, int userId, int bookId, int ratingValue):ratingId(ratingId), userId(userId), bookId(bookId), ratingValue(ratingValue >= 1 && ratingValue <= 5 ? ratingValue : 0){}
int Rating::getRatingId() const { return ratingId; }
int Rating::getUserId() const { return userId; }
int Rating::getBookId() const { return bookId; }
int Rating::getRatingValue() const { return ratingValue; }
void Rating::setRatingId(int id){ ratingId = id; }
bool Rating::setRatingValue(int value){
    if(value < 1 || value > 5)
        return false;
    ratingValue = value;
    return true;
}
QDataStream &operator<<(QDataStream &out, const Rating &rating){
    out << rating.ratingId << rating.userId << rating.bookId << rating.ratingValue;
    return out;
}
QDataStream &operator>>(QDataStream &in, Rating &rating){
    in >> rating.ratingId >> rating.userId >> rating.bookId >> rating.ratingValue;
    return in;
}

