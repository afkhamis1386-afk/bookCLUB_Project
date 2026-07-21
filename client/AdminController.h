#ifndef ADMINCONTROLLER_H
#define ADMINCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "NetworkManager.h"

class AdminController : public QObject {
    Q_OBJECT

public:
    explicit AdminController(NetworkManager *networkManager, QObject *parent = nullptr);
    void loadAllUsers();
    void loadNormalUserDetails(int userId);
    void loadPublisherDetails(int userId);
    void blockUser(int userId);
    void unblockUser(int userId);
    void deleteUser(int userId);
    void setUserActiveStatus(int userId, bool active);
    void loadAllBooks();
    void loadBookDetailsForReview(int bookId);
    void deleteBook(int bookId);
    void updateBook(int bookId, const QString &bookName, const QString &description, double price);
    void loadAllReviews();
    void deleteReview(int reviewId);
    void createAdmin(const QString &username, const QString &password, const QString &securityAnswer, const QString &firstName, const QString &lastName);

signals:
    void usersLoaded(const QVariantList &users);
    void usersLoadFailed(const QString &message);
    void normalUserDetailsLoaded(const QVariantMap &userData);
    void normalUserDetailsLoadFailed(const QString &message);
    void publisherDetailsLoaded(const QVariantMap &publisherData);
    void publisherDetailsLoadFailed(const QString &message);
    void userBlocked(const QString &message);
    void userBlockFailed(const QString &message);
    void userUnblocked(const QString &message);
    void userUnblockFailed(const QString &message);
    void userDeleted(const QString &message);
    void userDeleteFailed(const QString &message);
    void userActiveStatusChanged(const QString &message);
    void userActiveStatusChangeFailed(const QString &message);
    void allBooksLoaded(const QVariantList &books);
    void allBooksLoadFailed(const QString &message);
    void bookDetailsForReviewLoaded(const QVariantMap &bookData);
    void bookDetailsForReviewLoadFailed(const QString &message);
    void bookDeleted(const QString &message);
    void bookDeleteFailed(const QString &message);
    void bookUpdated(const QString &message);
    void bookUpdateFailed(const QString &message);
    void allReviewsLoaded(const QVariantList &reviews);
    void allReviewsLoadFailed(const QString &message);
    void reviewDeleted(const QString &message);
    void reviewDeleteFailed(const QString &message);
    void validationError(const QString &message);
    void adminCreated(const QString &message);
    void adminCreateFailed(const QString &message);

private slots:
    void onResponseReceived(RequestType type, const Response &response);

private:
    NetworkManager *networkManager;
    bool ensureConnected(const QString &failMessage);
};

#endif // ADMINCONTROLLER_H
