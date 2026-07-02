#ifndef TIMEDDISCOUNT_H
#define TIMEDDISCOUNT_H
#include <QDateTime>
#include <QDataStream>
class TimedDiscount {
private:
    int discountId;
    int bookId;
    double discountPercent;
    QDateTime startDate;
    QDateTime endDate;
public:
    TimedDiscount();
    TimedDiscount(int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate);
    TimedDiscount(int discountId, int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate);
    int getDiscountId() const;
    int getBookId() const;
    double getDiscountPercent() const;
    QDateTime getStartDate() const;
    QDateTime getEndDate() const;
    void setDiscountId(int id);
    bool setDiscountPercent(double percent);
    bool setDates(const QDateTime &startDate, const QDateTime &endDate);
    bool isActive() const;
    bool isExpired() const;
    friend QDataStream &operator<<(QDataStream &out, const TimedDiscount &td);
    friend QDataStream &operator>>(QDataStream &in, TimedDiscount &td);
};
#endif // TIMEDDISCOUNT_H
