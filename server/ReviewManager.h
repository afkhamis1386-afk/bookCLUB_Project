#ifndef REVIEWMANAGER_H
#define REVIEWMANAGER_H
#include "../common/Response.h"
#include <QString>
class ReviewManager {
public:
    ReviewManager();
    Response submitReview(int userId, int bookId, const QString &commentText, int parentId = -1);
    Response editReview(int userId, int reviewId, const QString &newCommentText);
    Response deleteReview(int userId, int reviewId);
    Response deleteReviewByAdmin(int reviewId);
    Response getReviewsForBook(int bookId);
};
#endif // REVIEWMANAGER_H
