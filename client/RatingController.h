#ifndef RATINGCONTROLLER_H
#define RATINGCONTROLLER_H
#include <QObject>
#include "NetworkManager.h"
class RatingController : public QObject {
    Q_OBJECT

public:
    explicit RatingController(NetworkManager *networkManager, QObject *parent = nullptr);
    void submitRating(int bookId, int ratingValue);
    void loadRatingSummary(int bookId);

signals:
    void ratingSubmitted(double newAverage, int ratingCount, const QString &message);
    void ratingSubmitFailed(const QString &message);
    void ratingSummaryLoaded(double averageRating, int ratingCount);
    void ratingSummaryLoadFailed(const QString &message);
    void validationError(const QString &message);

private slots:
    void onResponseReceived(RequestType type, const Response &response);

private:
    NetworkManager *networkManager;
};

#endif // RATINGCONTROLLER_H
