#ifndef REVIEWREPOSITORY_H
#define REVIEWREPOSITORY_H
#include "../common/Review.h"
#include <QVector>
class ReviewRepository {
public:
    ReviewRepository();
    int insertReview(const Review &review);
    Review* loadReviewById(int reviewId);
    bool updateCommentText(int reviewId, const QString &newComment);
    bool setDeletedStatus(int reviewId, bool isDeleted);
    QVector<int> getReviewIdsByBook(int bookId);
    QVector<int> getRepliesOf(int reviewId);
    QVector<int> getReviewIdsByUser(int userId);
    QVector<int> getAllReviewIds();
    bool reviewBelongsToUser(int reviewId, int userId);
};
#endif // REVIEWREPOSITORY_H
