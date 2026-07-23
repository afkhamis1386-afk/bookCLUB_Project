#include "NetworkManager.h"
#include "../common/Request.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), socket(new ClientSocket(this)),
    currentUserId(-1), currentUserRole(UserRole::NormalUser), loggedIn(false) {
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
    if (!loggedIn) {
        return;
    }
    sendRequest(RequestType::Logout);
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
void NetworkManager::getAccountInfo() {
    sendRequest(RequestType::GetAccountInfo);
}
void NetworkManager::getAllGenres() {
    sendRequest(RequestType::GetAllGenres);
}
void NetworkManager::getAllCategories() {
    sendRequest(RequestType::GetAllCategories);
}
void NetworkManager::setFavoriteGenres(const QVector<int> &genreIds) {
    QVariantMap p;
    QVariantList list;
    list.reserve(genreIds.size());
    for (int id : genreIds) {
        list.append(id);
    }
    p["genreIds"] = list;
    sendRequest(RequestType::SetFavoriteGenres, p);
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
void NetworkManager::getBookCoverImage(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::GetBookCoverImage, p);
}
void NetworkManager::getBookFile(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::GetBookFile, p);

}
void NetworkManager::getPurchasedBooks() {
    sendRequest(RequestType::GetPurchasedBooks);
}
void NetworkManager::saveReadingProgress(int bookId, int lastPage) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["lastPage"] = lastPage;
    sendRequest(RequestType::SaveReadingProgress, p);
}
void NetworkManager::getBooksByGenre(int genreId) {
    QVariantMap p;
    p["genreId"] = genreId;
    sendRequest(RequestType::GetBooksByGenre, p);
}
void NetworkManager::getBooksByCategory(int categoryId) {
    QVariantMap p;
    p["categoryId"] = categoryId;
    sendRequest(RequestType::GetBooksByCategory, p);
}
void NetworkManager::getNewestBooks(int limit) {
    QVariantMap p;
    p["limit"] = limit;
    sendRequest(RequestType::GetNewestBooks, p);
}
void NetworkManager::getBestSellers(int limit) {
    QVariantMap p;
    p["limit"] = limit;
    sendRequest(RequestType::GetBestSellers, p);
}
void NetworkManager::getPopularBooks(int limit) {
    QVariantMap p;
    p["limit"] = limit;
    sendRequest(RequestType::GetPopularBooks, p);
}
void NetworkManager::getFreeBooks() {
    sendRequest(RequestType::GetFreeBooks);
}
void NetworkManager::getRecommendedBooks() {
    sendRequest(RequestType::GetRecommendedBooks);
}
void NetworkManager::addBook(const QString &bookName, const QString &description, double price,
                             const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
                             const QByteArray &coverImageData, const QString &coverImageExtension,
                             const QByteArray &pdfData) {
    QVariantMap p;
    p["bookName"] = bookName;
    p["description"] = description;
    p["price"] = price;
    p["genreTitle"] = genreTitle;
    p["categoryTitle"] = categoryTitle;
    p["authorName"] = authorName;
    p["coverImageData"] = coverImageData;
    p["coverImageExtension"] = coverImageExtension;
    p["pdfData"] = pdfData;
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
void NetworkManager::reactivateBook(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::ReactivateBook, p);
}
void NetworkManager::applyDiscount(int bookId, double discountPercent, double discountAmount) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["discountPercent"] = discountPercent;
    p["discountAmount"] = discountAmount;
    sendRequest(RequestType::ApplyDiscount, p);
}
void NetworkManager::applyTimedDiscount(int bookId, double discountPercent, const QDateTime &startDate, const QDateTime &endDate) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["discountPercent"] = discountPercent;
    p["startDate"] = startDate;
    p["endDate"] = endDate;
    sendRequest(RequestType::ApplyTimedDiscount, p);
}
void NetworkManager::getPublisherStats() {
    sendRequest(RequestType::GetPublisherStats);
}
void NetworkManager::addToCart(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::AddToCart, p);
}
void NetworkManager::claimFreeBook(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::ClaimFreeBook, p);
}
void NetworkManager::removeFromCart(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::RemoveFromCart, p);
}
void NetworkManager::getCart(){
    sendRequest(RequestType::GetCart);
}
void NetworkManager::checkout(const QString &cardNumber) {
    sendRequest(RequestType::Checkout, {{"cardNumber", cardNumber}});
}
void NetworkManager::getOrderHistory() {
    sendRequest(RequestType::GetOrderHistory);
}
void NetworkManager::submitReview(int bookId, const QString &commentText, int parentId) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["commentText"] = commentText;
    p["parentId"] = parentId;
    sendRequest(RequestType::SubmitReview, p);
}
void NetworkManager::editReview(int reviewId, const QString &newCommentText) {
    QVariantMap p;
    p["reviewId"] = reviewId;
    p["commentText"] = newCommentText;
    sendRequest(RequestType::EditReview, p);
}
void NetworkManager::deleteReview(int reviewId) {
    QVariantMap p;
    p["reviewId"] = reviewId;
    sendRequest(RequestType::DeleteReview, p);
}
void NetworkManager::getReviewsForBook(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::GetReviewsForBook, p);
}
void NetworkManager::submitRating(int bookId, int ratingValue) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["ratingValue"] = ratingValue;
    sendRequest(RequestType::SubmitRating, p);
}
void NetworkManager::getBookRatingSummary(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::GetBookRatingSummary, p);
}
void NetworkManager::getShelf() {
    sendRequest(RequestType::GetShelf);
}
void NetworkManager::createShelf(const QString &shelfName) {
    QVariantMap p;
    p["shelfName"] = shelfName;
    sendRequest(RequestType::CreateShelf, p);
}
void NetworkManager::renameShelf(int shelfId, const QString &newName) {
    QVariantMap p;
    p["shelfId"] = shelfId;
    p["newName"] = newName;
    sendRequest(RequestType::RenameShelf, p);
}
void NetworkManager::deleteShelf(int shelfId) {
    QVariantMap p;
    p["shelfId"] = shelfId;
    sendRequest(RequestType::DeleteShelf, p);
}
void NetworkManager::addBookToShelf(int shelfId, int bookId) {
    QVariantMap p;
    p["shelfId"] = shelfId;
    p["bookId"] = bookId;
    sendRequest(RequestType::AddBookToShelf, p);
}
void NetworkManager::saveBook(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::SaveBook, p);
}
void NetworkManager::unsaveBook(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::UnsaveBook, p);
}
void NetworkManager::getSavedBooks() {
    sendRequest(RequestType::GetSavedBooks);
}
void NetworkManager::moveBookBetweenShelves(int sourceShelfId, int destShelfId, int bookId) {
    QVariantMap p;
    p["sourceShelfId"] = sourceShelfId;
    p["destShelfId"] = destShelfId;
    p["bookId"] = bookId;
    sendRequest(RequestType::MoveBookBetweenShelves, p);
}
void NetworkManager::getNotifications() {
    sendRequest(RequestType::GetNotifications);
}
void NetworkManager::markNotificationRead(int notificationId) {
    QVariantMap p;
    p["notificationId"] = notificationId;
    sendRequest(RequestType::MarkNotificationRead, p);
}
void NetworkManager::getUnreadNotificationCount() {
    sendRequest(RequestType::GetUnreadNotificationCount);
}
void NetworkManager::getAllUsers() {
    sendRequest(RequestType::GetAllUsers);
}
void NetworkManager::getNormalUserDetails(int userId) {
    QVariantMap p;
    p["userId"] = userId;
    sendRequest(RequestType::GetNormalUserDetails, p);
}
void NetworkManager::getPublisherDetails(int userId) {
    QVariantMap p;
    p["userId"] = userId;
    sendRequest(RequestType::GetPublisherDetails, p);
}
void NetworkManager::unblockUser(int userId) {
    QVariantMap p;
    p["userId"] = userId;
    sendRequest(RequestType::UnblockUser, p);
}
void NetworkManager::getAllBooksAdmin() {
    sendRequest(RequestType::GetAllBooksAdmin);
}
void NetworkManager::getBookDetailsForReview(int bookId) {
    QVariantMap p;
    p["bookId"] = bookId;
    sendRequest(RequestType::GetBookDetailsForReview, p);
}
void NetworkManager::getAllReviews() {
    sendRequest(RequestType::GetAllReviews);
}
void NetworkManager::blockUser(int userId) {
    QVariantMap p;
    p["userId"] = userId;
    sendRequest(RequestType::BlockUser, p);
}
void NetworkManager::setUserActiveStatus(int userId, bool active) {
    QVariantMap p;
    p["userId"] = userId;
    p["active"] = active;
    sendRequest(RequestType::SetUserActiveStatus, p);
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
void NetworkManager::updateBookByAdmin(int bookId, const QString &bookName, const QString &description, double price) {
    QVariantMap p;
    p["bookId"] = bookId;
    p["bookName"] = bookName;
    p["description"] = description;
    p["price"] = price;
    sendRequest(RequestType::UpdateBookByAdmin, p);
}
void NetworkManager::deleteReviewByAdmin(int reviewId) {
    QVariantMap p;
    p["reviewId"] = reviewId;
    sendRequest(RequestType::DeleteReviewByAdmin, p);
}
void NetworkManager::onSocketConnected() {
    emit connected();
}
void NetworkManager::onSocketDisconnected() {
    loggedIn = false;
    currentUserId = -1;
    currentUserRole = UserRole::NormalUser;
    emit disconnected();
}
void NetworkManager::onSocketError(const QString &errorMessage) {
    emit connectionError(errorMessage);
}
void NetworkManager::onSocketResponseReceived(const Response &response) {
    if (response.getStatus() == ResponseStatus::PushNotification) {
        QVariantMap data = response.getData();
        if (data.contains("liveUpdateType")) {
            emit bookLiveUpdateReceived(data.value("liveUpdateType").toString(), data);
        } else {
            emit pushNotificationReceived(data);
        }
        return;
    }
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
    if (matchedType == RequestType::Logout) {
        if (response.isSuccess()) {
            currentUserId = -1;
            currentUserRole = UserRole::NormalUser;
            loggedIn = false;
        }
    }
    emit responseReceived(matchedType, response);
}
void NetworkManager::bootstrapFirstAdmin(const QString &username, const QString &password, const QString &securityAnswer,
                                         const QString &firstName, const QString &lastName) {
    QVariantMap p;
    p["username"] = username;
    p["password"] = password;
    p["securityAnswer"] = securityAnswer;
    p["firstName"] = firstName;
    p["lastName"] = lastName;
    sendRequest(RequestType::BootstrapFirstAdmin, p);
}
void NetworkManager::createAdditionalAdmin(const QString &username, const QString &password, const QString &securityAnswer,
                                           const QString &firstName, const QString &lastName) {
    QVariantMap p;
    p["username"] = username;
    p["password"] = password;
    p["securityAnswer"] = securityAnswer;
    p["firstName"] = firstName;
    p["lastName"] = lastName;
    sendRequest(RequestType::CreateAdditionalAdmin, p);
}