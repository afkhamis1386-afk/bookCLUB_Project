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
    void addBook(const QString &bookName, const QString &description, double price,
                 const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
                 const QString &coverImagePath, const QString &pdfFilePath);
    void updateBook(int bookId, const QString &bookName, const QString &description, double price);
    void deactivateBook(int bookId);
    void getPublisherStats();
    void addToCart(int bookId);
    void removeFromCart(int bookId);
    void getCart();
    void checkout();
    void submitReview(int bookId, const QString &commentText, int parentId = -1);
    void submitRating(int bookId, int ratingValue);
    void getShelf();
    void getNotifications();
    void markNotificationRead(int notificationId);
    void getAllUsers();
    void blockUser(int userId);
    void deleteUser(int userId);
    void deleteBook(int bookId);
signals:
    void connected();
    void disconnected();
    void connectionError(const QString &errorMessage);
    void responseReceived(RequestType requestType, const Response &response);
    void loginSucceeded(int userId, UserRole role);
    void loginFailed(const QString &message);
    void pushNotificationReceived(const QVariantMap &notificationData);
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
