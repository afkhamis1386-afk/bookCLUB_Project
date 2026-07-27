#include "ClientHandler.h"
#include "ClientRegistry.h"
#include"NotificationManager.h"
#include "AuthManager.h"
#include "AccessControl.h"
#include "BookManager.h"
#include "CartManager.h"
#include "OrderManager.h"
#include "ReviewManager.h"
#include "SavedBookManager.h"
#include "RatingManager.h"
#include "ShelfManager.h"
#include "AdminManager.h"
#include "PublisherManager.h"
#include "DatabaseManager.h"
#include "ServerLogRepository.h"
#include <QDataStream>
#include <QDebug>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QObject(parent), socketDescriptor(socketDescriptor), socket(nullptr),
    authenticatedUserId(-1), authenticatedRole(UserRole::NormalUser), isAuthenticated(false) {}
void ClientHandler::pushNotificationToClient(const Notification &notification) {
    QVariantMap data;
    data["notificationId"] = notification.getNotificationId();
    data["type"] = static_cast<int>(notification.getNotificationType());
    data["title"] = notification.getTitle();
    data["message"] = notification.getMessage();
    data["targetId"] = notification.getTargetId();
    Response pushMsg(ResponseStatus::PushNotification, "اعلان جدید", data);
    sendResponse(pushMsg);
}
void ClientHandler::sendLiveUpdateToClient(const Response &liveUpdate) {
    sendResponse(liveUpdate);
}
void ClientHandler::run() {
    socket = new QTcpSocket();
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qWarning() << "خطا در تنظیم سوکت کلاینت";
        delete socket;
        return;
    }
    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
    qDebug() << "کلاینت جدید متصل شد Socket ID:" << socketDescriptor;
}
void ClientHandler::onReadyRead() {
    buffer.append(socket->readAll());
    while (true) {
        if (buffer.size() < 4)
            return;
        QDataStream sizeStream(buffer);
        sizeStream.setVersion(QDataStream::Qt_6_5);
        quint32 msgSize;
        sizeStream >> msgSize;
        if (msgSize > MAX_FRAME_SIZE) {
            qWarning() << "پیام دریافتی از حد مجاز بزرگتر است (msgSize:" << msgSize
                       << ")اتصال کلاینت قطع می شود Socket ID:" << socketDescriptor;
            buffer.clear();
            socket->abort();
            return;
        }
        if (buffer.size() < static_cast<int>(4 + msgSize))
            return;
        QByteArray msgData = buffer.mid(4, msgSize);
        buffer.remove(0, 4 + msgSize);
        QDataStream msgStream(msgData);
        msgStream.setVersion(QDataStream::Qt_6_5);
        Request req;
        msgStream >> req;
        processRequest(req);
    }
}
void ClientHandler::onDisconnected() {
    qDebug() << "کلاینت قطع شد Socket ID:" << socketDescriptor;
    if (isAuthenticated) {
        ClientRegistry::getInstance()->unregisterClient(authenticatedUserId);
    }
    DatabaseManager::getInstance()->closeConnectionForCurrentThread();
    emit clientDisconnected(socketDescriptor);
    socket->deleteLater();
}
bool ClientHandler::checkRole(const QVector<UserRole> &allowedRoles, Response &outErrorResponse) {
    if (!isAuthenticated) {
        outErrorResponse = Response(ResponseStatus::Unauthorized, "لطفاً ابتدا وارد شوید");
        return false;
    }
    AccessControl access;
    Response check = access.checkAccess(authenticatedUserId, authenticatedRole, allowedRoles);
    if (!check.isSuccess()) {
        outErrorResponse = check;
        return false;
    }
    return true;
}
void ClientHandler::processRequest(const Request &req) {
    Response response;
    RequestType type = req.getType();
    if (type == RequestType::Register || type == RequestType::Login ||
        type == RequestType::RecoverPassword || type == RequestType::BootstrapFirstAdmin ||
        type == RequestType::GetAllGenres || type == RequestType::GetAllCategories) {
        response = handleAuthRequest(req);
        if (type == RequestType::Login && response.isSuccess()) {
            if (isAuthenticated) {
                ClientRegistry::getInstance()->unregisterClient(authenticatedUserId);
            }
            authenticatedUserId = response.getData().value("userId").toInt();
            authenticatedRole = static_cast<UserRole>(response.getData().value("role").toInt());
            isAuthenticated = true;
            ClientRegistry::getInstance()->registerClient(authenticatedUserId, this);
        }
    }
    else if (!isAuthenticated) {
        response = Response(ResponseStatus::Unauthorized, "لطفاً ابتدا وارد شوید");
    }
    else {
        Response accessError;
        switch (type) {
        case RequestType::ChangePassword:
        case RequestType::GetAccountInfo:
            response = handleAuthRequest(req);
            break;
        case RequestType::UpdateAccount:
            if (checkRole({UserRole::NormalUser, UserRole::Publisher}, accessError))
                response = handleAuthRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetAllGenres:
        case RequestType::GetAllCategories:
            response = handleAuthRequest(req);
            break;
        case RequestType::SetFavoriteGenres:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleAuthRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetBooks:
        case RequestType::SearchBooks:
        case RequestType::GetBookDetails:
        case RequestType::GetBookCoverImage:
            response = handleBookRequest(req);
            break;
        case RequestType::GetBooksByGenre:
        case RequestType::GetBooksByCategory:
        case RequestType::GetNewestBooks:
        case RequestType::GetFreeBooks:
        case RequestType::GetRecommendedBooks:
        case RequestType::GetBestSellers:
        case RequestType::GetPopularBooks:
            response = handleBookRequest(req);
            break;
        case RequestType::AddBook:
        case RequestType::UpdateBook:
        case RequestType::DeactivateBook:
        case RequestType::ReactivateBook:
        case RequestType::ApplyDiscount:
        case RequestType::ApplyTimedDiscount:
        case RequestType::CancelTimedDiscount:
            if (checkRole({UserRole::Publisher}, accessError))
                response = handleBookRequest(req);
            else
                response = accessError;
            break;
        case RequestType::AddToCart:
        case RequestType::RemoveFromCart:
        case RequestType::GetCart:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleCartRequest(req);
            else
                response = accessError;
            break;
        case RequestType::ClaimFreeBook:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleBookRequest(req);
            else
                response = accessError;
            break;
        case RequestType::Checkout:
        case RequestType::GetOrderHistory:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleOrderRequest(req);
            else
                response = accessError;
            break;
        case RequestType::SubmitReview:
        case RequestType::EditReview:
        case RequestType::DeleteReview:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleReviewRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetReviewsForBook:
            response = handleReviewRequest(req);
            break;
        case RequestType::SubmitRating:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleRatingRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetBookRatingSummary:
            response = handleRatingRequest(req);
            break;
        case RequestType::GetShelf:
        case RequestType::CreateShelf:
        case RequestType::RenameShelf:
        case RequestType::DeleteShelf:
        case RequestType::AddBookToShelf:
        case RequestType::RemoveBookFromShelf:
        case RequestType::MoveBookBetweenShelves:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleShelfRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetBookFile:
        case RequestType::SaveReadingProgress:
        case RequestType::GetPurchasedBooks:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleBookRequest(req);
            else
                response = accessError;
            break;
        case RequestType::SaveBook:
        case RequestType::UnsaveBook:
        case RequestType::GetSavedBooks:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleSavedBookRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetNotifications:
        case RequestType::MarkNotificationRead:
        case RequestType::GetUnreadNotificationCount:
            if (checkRole({UserRole::NormalUser, UserRole::Publisher, UserRole::Admin}, accessError))
                response = handleNotificationRequest(req);
            else
                response = accessError;
            break;
        case RequestType::Logout:
            response = handleLogoutRequest();
            break;
        case RequestType::GetAllUsers:
        case RequestType::CreateAdditionalAdmin:
        case RequestType::GetNormalUserDetails:
        case RequestType::GetPublisherDetails:
        case RequestType::BlockUser:
        case RequestType::UnblockUser:
        case RequestType::DeleteUser:
        case RequestType::SetUserActiveStatus:
        case RequestType::GetAllBooksAdmin:
        case RequestType::GetBookDetailsForReview:
        case RequestType::DeleteBook:
        case RequestType::UpdateBookByAdmin:
        case RequestType::GetAllReviews:
        case RequestType::DeleteReviewByAdmin:
            if (checkRole({UserRole::Admin}, accessError))
                response = handleAdminRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetPublisherStats:
            if (checkRole({UserRole::Publisher}, accessError))
                response = handlePublisherRequest(req);
            else
                response = accessError;
            break;
        default:
            response = Response(ResponseStatus::Error, "نوع درخواست نامعتبر است");
        }
    }
    sendResponse(response);
    const QString requestName = requestTypeToString(type);
    const int statusCode = static_cast<int>(response.getStatus());
    emit requestLogReceived(requestName, statusCode);
    ServerLogRepository logRepo;
    logRepo.insertLog(isAuthenticated ? authenticatedUserId : -1, requestName, statusCode, response.getMessage());
}
void ClientHandler::sendResponse(const Response &res) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << res;
    QByteArray packet;
    QDataStream packetStream(&packet, QIODevice::WriteOnly);
    packetStream.setVersion(QDataStream::Qt_6_5);
    packetStream << static_cast<quint32>(data.size());
    packet.append(data);
    socket->write(packet);
    socket->flush();
}
Response ClientHandler::handleAuthRequest(const Request &req) {
    AuthManager authManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::Register:
        if (p.value("role").toInt() == static_cast<int>(UserRole::Publisher)) {
            return authManager.registerPublisher(
                p.value("username").toString(), p.value("password").toString(),
                p.value("securityAnswer").toString(), p.value("firstName").toString(),
                p.value("lastName").toString(), p.value("email").toString(),
                p.value("publicationName").toString(), p.value("licenseNumber").toString(),
                p.value("shortDescription").toString());
        }
        return authManager.registerNormalUser(
            p.value("username").toString(), p.value("password").toString(),
            p.value("securityAnswer").toString(), p.value("firstName").toString(),
            p.value("lastName").toString());
    case RequestType::Login:
        return authManager.login(p.value("username").toString(), p.value("password").toString());
    case RequestType::ChangePassword:
        return authManager.changePassword(authenticatedUserId, authenticatedRole, p.value("oldPassword").toString(), p.value("newPassword").toString());
    case RequestType::GetAccountInfo:
        return authManager.getAccountInfo(authenticatedUserId, authenticatedRole);
    case RequestType::UpdateAccount:
        return authManager.updateAccount(authenticatedUserId, authenticatedRole, p);
    case RequestType::RecoverPassword:
        return authManager.recoverPassword(p.value("username").toString(), p.value("securityAnswer").toString(), p.value("newPassword").toString());
    case RequestType::GetAllGenres:
        return authManager.getAllGenres();
    case RequestType::GetAllCategories:
        return authManager.getAllCategories();
    case RequestType::SetFavoriteGenres: {
        QVariantList genreList = p.value("genreIds").toList();
        QVector<int> genreIds;
        genreIds.reserve(genreList.size());
        for (const QVariant &v : qAsConst(genreList)) {
            genreIds.append(v.toInt());
        }
        return authManager.setFavoriteGenres(authenticatedUserId, genreIds);
    }
    case RequestType::BootstrapFirstAdmin: {
        AdminManager adminManager;
        return adminManager.createAdmin(
            p.value("username").toString(),
            p.value("password").toString(),
            p.value("firstName").toString(),
            p.value("lastName").toString(),
            true);
    }
    default:
        return Response(ResponseStatus::Error, "درخواست احراز هویت نامعتبر");
    }
}
Response ClientHandler::handleBookRequest(const Request &req) {
    BookManager bookManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::GetBooks:
        return bookManager.getStorefrontBooks();
    case RequestType::SearchBooks:
        return bookManager.searchBooks(p.value("query").toString());
    case RequestType::GetBookDetails:
        return bookManager.getBookDetails(p.value("bookId").toInt());
    case RequestType::GetPurchasedBooks:
        return bookManager.getPurchasedBooks(authenticatedUserId);
    case RequestType::ClaimFreeBook:
        return bookManager.claimFreeBook(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::GetBookCoverImage:
        return bookManager.getCoverImageData(p.value("bookId").toInt());
    case RequestType::GetBookFile:
        return bookManager.getBookFileData(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::SaveReadingProgress:
        return bookManager.saveReadingProgress(
            authenticatedUserId,
            p.value("bookId").toInt(),
            p.value("lastPage").toInt() );
    case RequestType::AddBook:
        return bookManager.addBook(
            authenticatedUserId,
            p.value("bookName").toString(),
            p.value("description").toString(),
            p.value("price").toDouble(),
            p.value("genreTitle").toString(),
            p.value("categoryTitle").toString(),
            p.value("authorName").toString(),
            p.value("coverImageData").toByteArray(),
            p.value("coverImageExtension").toString(),
            p.value("pdfData").toByteArray(),
            p.value("discountPercent").toDouble());
    case RequestType::GetBooksByGenre:
        return bookManager.getBooksByGenre(p.value("genreId").toInt());
    case RequestType::GetBooksByCategory:
        return bookManager.getBooksByCategory(p.value("categoryId").toInt());
    case RequestType::GetNewestBooks:
        return bookManager.getNewestBooks(p.value("limit").toInt() > 0 ? p.value("limit").toInt() : 10);
    case RequestType::GetFreeBooks:
        return bookManager.getFreeBooks();
    case RequestType::GetBestSellers:
        return bookManager.getBestSellers(p.value("limit").toInt() > 0 ? p.value("limit").toInt() : 10);
    case RequestType::GetPopularBooks:
        return bookManager.getPopularBooks(p.value("limit").toInt() > 0 ? p.value("limit").toInt() : 10);
    case RequestType::GetRecommendedBooks:
        return bookManager.getRecommendedBooks(authenticatedUserId);
    case RequestType::UpdateBook:
        return bookManager.updateBook(
            authenticatedUserId,
            p.value("bookId").toInt(),
            p.value("bookName").toString(),
            p.value("description").toString(),
            p.value("price").toDouble(),
            p.value("genreTitle").toString(),
            p.value("categoryTitle").toString(),
            p.value("authorName").toString(),
            p.value("coverImageData").toByteArray(),
            p.value("coverImageExtension").toString(),
            p.value("pdfData").toByteArray());
    case RequestType::DeactivateBook:
        return bookManager.deactivateBook(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::ReactivateBook:
        return bookManager.reactivateBook(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::ApplyDiscount:
        return bookManager.applyDiscount(
            authenticatedUserId,
            p.value("bookId").toInt(),
            p.value("discountPercent").toDouble(),
            p.value("discountAmount").toDouble());
    case RequestType::ApplyTimedDiscount:
        return bookManager.applyTimedDiscount(
            authenticatedUserId,
            p.value("bookId").toInt(),
            p.value("discountPercent").toDouble(),
            p.value("startDate").toDateTime(),
            p.value("endDate").toDateTime());
    case RequestType::CancelTimedDiscount:
        return bookManager.cancelTimedDiscount(authenticatedUserId, p.value("bookId").toInt());
    default:
        return Response(ResponseStatus::Error, "درخواست کتاب نامعتبر");
    }
}
Response ClientHandler::handleCartRequest(const Request &req) {
    CartManager cartManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::AddToCart:
        return cartManager.addBookToCart(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::RemoveFromCart:
        return cartManager.removeBookFromCart(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::GetCart:
        return cartManager.getCart(authenticatedUserId);
    default:
        return Response(ResponseStatus::Error, "درخواست سبد خرید نامعتبر");
    }
}
Response ClientHandler::handleOrderRequest(const Request &req) {
    OrderManager orderManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::Checkout:
        return orderManager.checkout(authenticatedUserId, p.value("cardNumber").toString());
    case RequestType::GetOrderHistory:
        return orderManager.getOrderHistory(authenticatedUserId);
    default:
        return Response(ResponseStatus::Error, "درخواست سفارش نامعتبر");
    }
}
Response ClientHandler::handleReviewRequest(const Request &req) {
    ReviewManager reviewManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::SubmitReview:
        return reviewManager.submitReview(authenticatedUserId, p.value("bookId").toInt(), p.value("commentText").toString(), p.value("parentId").toInt());
    case RequestType::EditReview:
        return reviewManager.editReview(authenticatedUserId, p.value("reviewId").toInt(), p.value("commentText").toString());
    case RequestType::DeleteReview:
        return reviewManager.deleteReview(authenticatedUserId, p.value("reviewId").toInt());
    case RequestType::GetReviewsForBook:
        return reviewManager.getReviewsForBook(p.value("bookId").toInt());
    default:
        return Response(ResponseStatus::Error, "درخواست نظر نامعتبر");
    }
}
Response ClientHandler::handleRatingRequest(const Request &req) {
    RatingManager ratingManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::SubmitRating:
        return ratingManager.submitRating(authenticatedUserId, p.value("bookId").toInt(), p.value("ratingValue").toInt());
    case RequestType::GetBookRatingSummary:
        return ratingManager.getBookRatingSummary(p.value("bookId").toInt());
    default:
        return Response(ResponseStatus::Error, "درخواست امتیاز نامعتبر");
    }
}
Response ClientHandler::handleShelfRequest(const Request &req) {
    ShelfManager shelfManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::GetShelf:
        return shelfManager.getUserShelves(authenticatedUserId);
    case RequestType::CreateShelf:
        return shelfManager.createShelf(authenticatedUserId, p.value("shelfName").toString());
    case RequestType::RenameShelf:
        return shelfManager.renameShelf(authenticatedUserId, p.value("shelfId").toInt(), p.value("newName").toString());
    case RequestType::DeleteShelf:
        return shelfManager.deleteShelf(authenticatedUserId, p.value("shelfId").toInt());
    case RequestType::AddBookToShelf:
        return shelfManager.addBookToShelf(authenticatedUserId, p.value("shelfId").toInt(), p.value("bookId").toInt());
    case RequestType::RemoveBookFromShelf:
        return shelfManager.removeBookFromShelf(authenticatedUserId, p.value("shelfId").toInt(), p.value("bookId").toInt());
    case RequestType::MoveBookBetweenShelves:
        return shelfManager.moveBookBetweenShelves(authenticatedUserId,p.value("sourceShelfId").toInt(), p.value("destShelfId").toInt(), p.value("bookId").toInt());
    default:
        return Response(ResponseStatus::Error, "درخواست قفسه نامعتبر");
    }
}
Response ClientHandler::handleSavedBookRequest(const Request &req) {
    SavedBookManager savedBookManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::SaveBook:
        return savedBookManager.saveBook(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::UnsaveBook:
        return savedBookManager.unsaveBook(authenticatedUserId, p.value("bookId").toInt());
    case RequestType::GetSavedBooks:
        return savedBookManager.getSavedBooks(authenticatedUserId);
    default:
        return Response(ResponseStatus::Error, "درخواست کتاب ذخیره شده نامعتبر");
    }
}
Response ClientHandler::handleNotificationRequest(const Request &req) {
    NotificationManager notifManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::GetNotifications:
        return notifManager.getUserNotifications(authenticatedUserId);
    case RequestType::MarkNotificationRead:
        return notifManager.markAsRead(authenticatedUserId, p.value("notificationId").toInt());
    case RequestType::GetUnreadNotificationCount:
        return notifManager.getUnreadCount(authenticatedUserId);
    default:
        return Response(ResponseStatus::Error, "درخواست اعلان نامعتبر");
    }
}
Response ClientHandler::handleAdminRequest(const Request &req) {
    AdminManager adminManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::GetAllUsers:
        return adminManager.getAllUsers();
    case RequestType::GetNormalUserDetails:
        return adminManager.getNormalUserDetails(p.value("userId").toInt());
    case RequestType::GetPublisherDetails:
        return adminManager.getPublisherDetails(p.value("userId").toInt());
    case RequestType::BlockUser:
        return adminManager.blockUser(authenticatedUserId, p.value("userId").toInt());
    case RequestType::UnblockUser:
        return adminManager.unblockUser(authenticatedUserId, p.value("userId").toInt());
    case RequestType::DeleteUser:
        return adminManager.deleteUser(authenticatedUserId, p.value("userId").toInt());
    case RequestType::SetUserActiveStatus:
        return adminManager.setUserActiveStatus(authenticatedUserId, p.value("userId").toInt(), p.value("active").toBool());
    case RequestType::GetAllBooksAdmin:
        return adminManager.getAllBooks();
    case RequestType::GetBookDetailsForReview:
        return adminManager.getBookDetailsForReview(p.value("bookId").toInt());
    case RequestType::DeleteBook:
        return adminManager.removeInvalidBook(p.value("bookId").toInt());
    case RequestType::UpdateBookByAdmin:
        return adminManager.updateBook(
            p.value("bookId").toInt(),
            p.value("bookName").toString(),
            p.value("description").toString(),
            p.value("price").toDouble());
    case RequestType::GetAllReviews:
        return adminManager.getAllReviews();
    case RequestType::DeleteReviewByAdmin: {
        ReviewManager reviewManager;
        return reviewManager.deleteReviewByAdmin(p.value("reviewId").toInt());
    }
    case RequestType::CreateAdditionalAdmin:
        return adminManager.createAdmin(
            p.value("username").toString(),
            p.value("password").toString(),
            p.value("firstName").toString(),
            p.value("lastName").toString(),
            false);
    default:
        return Response(ResponseStatus::Error, "درخواست ادمین نامعتبر");
    }
}
Response ClientHandler::handlePublisherRequest(const Request &req) {
    Q_UNUSED(req)
    PublisherManager publisherManager;
    return publisherManager.getFullDashboard(authenticatedUserId);
}
Response ClientHandler::handleLogoutRequest() {
    if (!isAuthenticated) {
        return Response(ResponseStatus::Unauthorized, "کاربری وارد نشده است");
    }
    ClientRegistry::getInstance()->unregisterClient(authenticatedUserId);
    authenticatedUserId = -1;
    authenticatedRole = UserRole::NormalUser;
    isAuthenticated = false;
    return Response(ResponseStatus::Success, "خروج با موفقیت انجام شد");
}