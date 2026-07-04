#ifndef RATINGREPOSITORY_H
#define RATINGREPOSITORY_H
#include "../common/Rating.h"
#include <QVector>
class RatingRepository {
public:
    RatingRepository();
    bool upsertRating(const Rating &rating);
    Rating* loadRating(int userId, int bookId);
    double getAverageRating(int bookId);
    int getRatingCount(int bookId);
    bool deleteRating(int userId, int bookId);
};
#endif // RATINGREPOSITORY_H
