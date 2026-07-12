#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <QObject>
#include "ClientSocket.h"
#include "../common/Response.h"
#include "../common/Enums.h"

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    bool isConnected() const;
    int getCurrentUserId() const;
    UserRole getCurrentUserRole() const;
    bool isLoggedIn() const;
    void logout();
    void registerNormalUser(const QString &username, const QString &password, const QString &securityAnswer);
    void registerPublisher(const QString &username, const QString &password, const QString &securityAnswer,
                           const QString &firstName, const QString &lastName, const QString &email,
                           const QString &publicationName, const QString &licenseNumber,
                           const QString &shortDescription);
    void login(const QString &username, const QString &password);
    void changePassword(const QString &oldPassword, const QString &newPassword);
    void recoverPassword(const QString &username, const QString &securityAnswer, const QString &newPassword);
    void getBooks();
    void searchBooks(const QString &query);
    void getBookDetails(int bookId);
    void saveReadingProgress(int bookId, int lastPage);
    void getBooksByGenre(int genreId);
    void getBooksByCategory(int categoryId);
    void getNewestBooks(int limit = 10);
    void getFreeBooks();
    void getRecommendedBooks();
    void addBook(const QString &bookName, const QString &description, double price,
                 const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
                 const QByteArray &coverImageData, const QString &coverImageExtension,
                 const QByteArray &pdfData);

    void updateBook(int bookId, const QString &bookName, const QString &description, double price);
    void deactivateBook(int bookId);
    void reactivateBook(int bookId);
    void applyDiscount(int bookId, double discountPercent, double discountAmount);
    void getPublisherStats();
    void addToCart(int bookId);
    void removeFromCart(int bookId);
    void getCart();
    void checkout();
    void submitReview(int bookId, const QString &commentText, int parentId = -1);
    void editReview(int reviewId, const QString &newCommentText);
    void deleteReview(int reviewId);
    void getReviewsForBook(int bookId);
    void submitRating(int bookId, int ratingValue);
    void getBookRatingSummary(int bookId);
    void getShelf();
    void createShelf(const QString &shelfName);
    void renameShelf(int shelfId, const QString &newName);
    void deleteShelf(int shelfId);
    void addBookToShelf(int shelfId, int bookId);
    void saveBook(int bookId);
    void unsaveBook(int bookId);
    void getSavedBooks();
    void moveBookBetweenShelves(int sourceShelfId, int destShelfId, int bookId);
    void getNotifications();
    void markNotificationRead(int notificationId);
    void getUnreadNotificationCount();
    void getAllUsers();
    void getNormalUserDetails(int userId);
    void getPublisherDetails(int userId);
    void blockUser(int userId);
    void unblockUser(int userId);
    void setUserActiveStatus(int userId, bool active);
    void deleteUser(int userId);
    void getAllBooksAdmin();
    void getBookDetailsForReview(int bookId);
    void deleteBook(int bookId);
    void getAllReviews();
    void deleteReviewByAdmin(int reviewId);
    void getBookCoverImage(int bookId);
    void getBookFile(int bookId);
signals:
    void connected();
    void disconnected();
    void connectionError(const QString &errorMessage);
    void responseReceived(RequestType requestType, const Response &response);
    void loginSucceeded(int userId, UserRole role);
    void loginFailed(const QString &message);
    void pushNotificationReceived(const QVariantMap &notificationData);
    void bookLiveUpdateReceived(const QString &updateType, const QVariantMap &data);
private slots:
    void onSocketResponseReceived(const Response &response);
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(const QString &errorMessage);
private:
    ClientSocket *socket;
    int currentUserId;
    UserRole currentUserRole;
    bool loggedIn;
    QVector<RequestType> pendingRequestQueue;
    void sendRequest(RequestType type, const QVariantMap &payload = QVariantMap());
};
#endif // NETWORKMANAGER_H
