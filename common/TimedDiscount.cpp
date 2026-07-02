#include "TimedDiscount.h"
TimedDiscount::TimedDiscount():discountId(-1), bookId(-1), discountPercent(0.0){}
TimedDiscount::TimedDiscount(int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate):discountId(-1), bookId(bookId), discountPercent(discountPercent >= 0 && discountPercent <= 100 ? discountPercent : 0.0),
startDate(startDate), endDate(endDate){}
TimedDiscount::TimedDiscount(int discountId, int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate):discountId(discountId), bookId(bookId), discountPercent(discountPercent >= 0 && discountPercent <= 100 ? discountPercent : 0.0),
startDate(startDate), endDate(endDate){}
int TimedDiscount::getDiscountId() const { return discountId; }
int TimedDiscount::getBookId() const { return bookId; }
double TimedDiscount::getDiscountPercent() const { return discountPercent; }
QDateTime TimedDiscount::getStartDate() const { return startDate; }
QDateTime TimedDiscount::getEndDate() const { return endDate; }
void TimedDiscount::setDiscountId(int id){ discountId = id; }
bool TimedDiscount::setDiscountPercent(double percent){
    if(percent < 0 || percent > 100)
        return false;
    discountPercent = percent;
    return true;
}
bool TimedDiscount::setDates(const QDateTime &start, const QDateTime &end){
    if(!start.isValid() || !end.isValid() || start >= end)
        return false;
    startDate = start;
    endDate = end;
    return true;
}
bool TimedDiscount::isActive() const {
    QDateTime now = QDateTime::currentDateTime();
    return now >= startDate && now < endDate;
}
bool TimedDiscount::isExpired() const {
    return QDateTime::currentDateTime() >= endDate;
}
QDataStream &operator<<(QDataStream &out, const TimedDiscount &td){
    out << td.discountId << td.bookId << td.discountPercent << td.startDate << td.endDate;
    return out;
}
QDataStream &operator>>(QDataStream &in, TimedDiscount &td){
    in >> td.discountId >> td.bookId >> td.discountPercent >> td.startDate >> td.endDate;
    return in;
}
