#ifndef TIMEDDISCOUNTREPOSITORY_H
#define TIMEDDISCOUNTREPOSITORY_H
#include "../common/TimedDiscount.h"
#include <QVector>
class TimedDiscountRepository {
public:
    TimedDiscountRepository();
    int insertTimedDiscount(const TimedDiscount &discount);
    TimedDiscount* loadDiscountById(int discountId);
    TimedDiscount* getActiveDiscountForBook(int bookId);
    TimedDiscount* getCurrentOrUpcomingDiscountForBook(int bookId);
    QVector<int> getAllDiscountIdsForBook(int bookId);
    bool updateDates(int discountId, const QDateTime &startDate, const QDateTime &endDate);
    bool deleteDiscount(int discountId);
    QVector<int> getExpiredDiscountIds();
};
#endif // TIMEDDISCOUNTREPOSITORY_H
