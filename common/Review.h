#ifndef REVIEW_H
#define REVIEW_H
#include <QString>
#include <QDateTime>
#include <QDataStream>
class Review {
private:
    int reviewId;
    int userId;
    int bookId;
    QString commentText;
    int parentId;
    bool isDeleted;
    QDateTime reviewDate;
public:
    Review();
    Review(int userId, int bookId, const QString &commentText, int parentId = -1);
    Review(int reviewId, int userId, int bookId, const QString &commentText, int parentId, bool isDeleted, const QDateTime &reviewDate);
    int getReviewId() const;
    int getUserId() const;
    int getBookId() const;
    QString getCommentText() const;
    int getParentId() const;
    bool getIsDeleted() const;
    QDateTime getReviewDate() const;
    bool isReply() const;
    void setReviewId(int id);
    bool setCommentText(const QString &text);
    void setIsDeleted(bool deleted);
    friend QDataStream &operator<<(QDataStream &out, const Review &review);
    friend QDataStream &operator>>(QDataStream &in, Review &review);
};
#endif // REVIEW_H
