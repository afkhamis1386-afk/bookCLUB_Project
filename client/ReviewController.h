#ifndef REVIEWCONTROLLER_H
#define REVIEWCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include "NetworkManager.h"
class ReviewController : public QObject {
    Q_OBJECT

public:
    explicit ReviewController(NetworkManager *networkManager, QObject *parent = nullptr);
    void submitReview(int bookId, const QString &commentText, int parentId = -1);
    void editReview(int reviewId, const QString &newCommentText);
    void deleteReview(int reviewId);
    void loadReviewsForBook(int bookId);

signals:
    void reviewSubmitted(int reviewId, const QString &message);
    void reviewSubmitFailed(const QString &message);
    void reviewEdited(const QString &message);
    void reviewEditFailed(const QString &message);
    void reviewDeleted(const QString &message);
    void reviewDeleteFailed(const QString &message);
    void reviewsLoaded(const QVariantList &reviews);
    void reviewsLoadFailed(const QString &message);
    void validationError(const QString &message);

private slots:
    void onResponseReceived(RequestType type, const Response &response);

private:
    NetworkManager *networkManager;
};

#endif // REVIEWCONTROLLER_H
