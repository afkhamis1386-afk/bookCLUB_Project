#include "NetworkManager.h"
#include "../common/Request.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), socket(new ClientSocket(this)),
    currentUserId(-1), currentUserRole(UserRole::NormalUser), loggedIn(false)
{
    connect(socket, &ClientSocket::connected, this, &NetworkManager::onSocketConnected);
    connect(socket, &ClientSocket::disconnected, this, &NetworkManager::onSocketDisconnected);
    connect(socket, &ClientSocket::responseReceived, this, &NetworkManager::onSocketResponseReceived);
    connect(socket, &ClientSocket::errorOccurred, this, &NetworkManager::onSocketError);
}
void NetworkManager::connectToServer(const QString &host, quint16 port) {
    socket->connectToServer(host, port);
}
bool NetworkManager::isConnected() const {
    return socket->isConnected();
}
int NetworkManager::getCurrentUserId() const { return currentUserId; }
UserRole NetworkManager::getCurrentUserRole() const { return currentUserRole; }
bool NetworkManager::isLoggedIn() const { return loggedIn; }

void NetworkManager::logout() {
    currentUserId = -1;
    loggedIn = false;
}
void NetworkManager::sendRequest(RequestType type, const QVariantMap &payload) {
    Request req(type, payload, currentUserId);
    pendingRequestQueue.append(type);
    socket->sendRequest(req);
}
void NetworkManager::registerNormalUser(const QString &username, const QString &password, const QString &securityAnswer) {
    QVariantMap p;
    p["username"] = username;
    p["password"] = password;
    p["securityAnswer"] = securityAnswer;
    p["role"] = static_cast<int>(UserRole::NormalUser);
    sendRequest(RequestType::Register, p);
}
void NetworkManager::registerPublisher(const QString &username, const QString &password, const QString &securityAnswer,
                                       const QString &firstName, const QString &lastName, const QString &email,
                                       const QString &publicationName, const QString &licenseNumber,
                                       const QString &shortDescription) {
    QVariantMap p;
    p["username"] = username;
    p["password"] = password;
    p["securityAnswer"] = securityAnswer;
    p["role"] = static_cast<int>(UserRole::Publisher);
    p["firstName"] = firstName;
    p["lastName"] = lastName;
    p["email"] = email;
    p["publicationName"] = publicationName;
    p["licenseNumber"] = licenseNumber;
    p["shortDescription"] = shortDescription;
    sendRequest(RequestType::Register, p);
}
void NetworkManager::login(const QString &username, const QString &password) {
    QVariantMap p;
    p["username"] = username;
    p["password"] = password;
    sendRequest(RequestType::Login, p);
}

void NetworkManager::changePassword(const QString &oldPassword, const QString &newPassword) {
    QVariantMap p;
    p["oldPassword"] = oldPassword;
    p["newPassword"] = newPassword;
    p["role"] = static_cast<int>(currentUserRole);
    sendRequest(RequestType::ChangePassword, p);
}
void NetworkManager::recoverPassword(const QString &username, const QString &securityAnswer, const QString &newPassword) {
    QVariantMap p;
    p["username"] = username;
    p["securityAnswer"] = securityAnswer;
    p["newPassword"] = newPassword;
    sendRequest(RequestType::RecoverPassword, p);
}
void NetworkManager::getBooks() {
    sendRequest(RequestType::GetBooks);
}
void NetworkManager::searchBooks(const QString &query) {
    QVariantMap p;
    p["query"] = query;
    sendRequest(RequestType::SearchBooks, p);
}
void NetworkManager::getBookDetails(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::GetBookDetails, p);
}
void NetworkManager::addBook(const QString &bookName, const QString &description, double price,
                             const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
                             const QString &coverImagePath, const QString &pdfFilePath) {
    QVariantMap p;
    p["bookName"] = bookName;
    p["description"] = description;
    p["price"] = price;
    p["genreTitle"] = genreTitle;
    p["categoryTitle"] = categoryTitle;
    p["authorName"] = authorName;
    p["coverImagePath"] = coverImagePath;
    p["pdfFilePath"] = pdfFilePath;
    sendRequest(RequestType::AddBook, p);
}
void NetworkManager::updateBook(int bookId, const QString &bookName, const QString &description, double price) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["bookName"] = bookName;
    p["description"] = description;
    p["price"] = price;
    sendRequest(RequestType::UpdateBook, p);
}
void NetworkManager::deactivateBook(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::DeactivateBook, p);
}
void NetworkManager::getPublisherStats() {
    sendRequest(RequestType::GetPublisherStats);
}
void NetworkManager::addToCart(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::AddToCart, p);
}
void NetworkManager::removeFromCart(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::RemoveFromCart, p);
}
void NetworkManager::checkout() {
    sendRequest(RequestType::Checkout);
}
void NetworkManager::submitReview(int bookId, const QString &commentText, int parentId) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["commentText"] = commentText;
    p["parentId"] = parentId;
    sendRequest(RequestType::SubmitReview, p);
}
void NetworkManager::submitRating(int bookId, int ratingValue) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["ratingValue"] = ratingValue;
    sendRequest(RequestType::SubmitRating, p);
}
void NetworkManager::getShelf() {
    sendRequest(RequestType::GetShelf);
}
void NetworkManager::getNotifications() {
    sendRequest(RequestType::GetNotifications);
}
void NetworkManager::markNotificationRead(int notificationId) {
    QVariantMap p;
    p["notificationId"] = notificationId;
    sendRequest(RequestType::MarkNotificationRead, p);
}
void NetworkManager::getAllUsers() {
    sendRequest(RequestType::GetAllUsers);
}
void NetworkManager::blockUser(int userId) {
    QVariantMap p;
    p["userId"] = userId;
    sendRequest(RequestType::BlockUser, p);
}
void NetworkManager::deleteUser(int userId) {
    QVariantMap p;
    p["userId"] = userId;
    sendRequest(RequestType::DeleteUser, p);
}
void NetworkManager::deleteBook(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::DeleteBook, p);
}
void NetworkManager::onSocketConnected() {
    emit connected();
}
void NetworkManager::onSocketDisconnected() {
    loggedIn = false;
    currentUserId = -1;
    emit disconnected();
}
void NetworkManager::onSocketError(const QString &errorMessage) {
    emit connectionError(errorMessage);
}
void NetworkManager::onSocketResponseReceived(const Response &response) {
    if (pendingRequestQueue.isEmpty()) {
        qWarning() << "پاسخی دریافت شد بدون درخواست معلق متناظر";
        return;
    }
    RequestType matchedType = pendingRequestQueue.takeFirst();
    if (matchedType == RequestType::Login) {
        if (response.isSuccess()) {
            currentUserId = response.getData().value("userId").toInt();
            currentUserRole = static_cast<UserRole>(response.getData().value("role").toInt());
            loggedIn = true;
            emit loginSucceeded(currentUserId, currentUserRole);
        } else {
            emit loginFailed(response.getMessage());
        }
    }
    emit responseReceived(matchedType, response);
}