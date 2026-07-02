#include "Review.h"
Review::Review():reviewId(-1), userId(-1), bookId(-1), parentId(-1), isDeleted(false), reviewDate(QDateTime::currentDateTime()){}
Review::Review(int userId, int bookId, const QString &commentText, int parentId):reviewId(-1), userId(userId), bookId(bookId), commentText(commentText.trimmed()), parentId(parentId), isDeleted(false), reviewDate(QDateTime::currentDateTime()){}
Review::Review(int reviewId, int userId, int bookId, const QString &commentText, int parentId, bool isDeleted, const QDateTime &reviewDate):reviewId(reviewId), userId(userId), bookId(bookId),commentText(commentText), parentId(parentId),
isDeleted(isDeleted), reviewDate(reviewDate){}
int Review::getReviewId() const { return reviewId; }
int Review::getUserId() const { return userId; }
int Review::getBookId() const { return bookId; }
QString Review::getCommentText() const { return commentText; }
int Review::getParentId() const { return parentId; }
bool Review::getIsDeleted() const { return isDeleted; }
QDateTime Review::getReviewDate() const { return reviewDate; }
bool Review::isReply() const {
    return parentId != -1;
}
void Review::setReviewId(int id) { reviewId = id; }
bool Review::setCommentText(const QString &text){
    if(text.trimmed().isEmpty() || text.length() > 1000)
        return false;
    commentText = text.trimmed();
    return true;
}
void Review::setIsDeleted(bool deleted){ isDeleted = deleted; }
QDataStream &operator<<(QDataStream &out, const Review &review){
    out << review.reviewId << review.userId << review.bookId << review.commentText << review.parentId << review.isDeleted << review.reviewDate;
    return out;
}
QDataStream &operator>>(QDataStream &in, Review &review){
    in >> review.reviewId >> review.userId >> review.bookId >> review.commentText >> review.parentId >> review.isDeleted >> review.reviewDate;
    return in;
}

