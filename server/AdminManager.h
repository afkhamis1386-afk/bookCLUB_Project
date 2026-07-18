#ifndef ADMINMANAGER_H
#define ADMINMANAGER_H
#include "../common/Response.h"
#include <QString>
class AdminManager {
public:
    AdminManager();
    Response getAllUsers();
    Response getNormalUserDetails(int userId);
    Response getPublisherDetails(int userId);
    Response blockUser(int userId);
    Response unblockUser(int userId);
    Response deleteUser(int userId);
    Response setUserActiveStatus(int targetUserId, bool active);
    Response getAllBooks();
    Response getBookDetailsForReview(int bookId);
    Response removeInvalidBook(int bookId);
    Response getAllReviews();
    Response createAdmin(const QString &username, const QString &plainPassword, const QString &plainAnswer, const QString &firstName, const QString &lastName, bool requireNoExistingAdmin);
};
#endif // ADMINMANAGER_H
