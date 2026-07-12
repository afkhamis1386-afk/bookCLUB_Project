#ifndef RATINGMANAGER_H
#define RATINGMANAGER_H
#include "../common/Response.h"
class RatingManager {
public:
    RatingManager();
    Response submitRating(int userId, int bookId, int ratingValue);
    Response getBookRatingSummary(int bookId);
};

#endif // RATINGMANAGER_H
