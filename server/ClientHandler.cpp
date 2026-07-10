#include "ClientHandler.h"
#include "ClientRegistry.h"
#include"NotificationManager.h"
#include "AuthManager.h"
#include "AccessControl.h"
#include "BookManager.h"
#include "CartManager.h"
#include "OrderManager.h"
#include "ReviewManager.h"
#include "RatingManager.h"
#include "ShelfManager.h"
#include "AdminManager.h"
#include "PublisherManager.h"
#include "DatabaseManager.h"
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
    qDebug() << "کلاینت قطع شد. Socket ID:" << socketDescriptor;
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
    if (type == RequestType::Register || type == RequestType::Login || type == RequestType::RecoverPassword) {
        response = handleAuthRequest(req);
        if (type == RequestType::Login && response.isSuccess()) {
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
            response = handleAuthRequest(req);
            break;
        case RequestType::GetBooks:
        case RequestType::SearchBooks:
        case RequestType::GetBookDetails:
            response = handleBookRequest(req);
            break;
        case RequestType::AddBook:
        case RequestType::UpdateBook:
        case RequestType::DeactivateBook:
            if (checkRole({UserRole::Publisher}, accessError))
                response = handleBookRequest(req);
            else
                response = accessError;
            break;
        case RequestType::AddToCart:
        case RequestType::RemoveFromCart:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleCartRequest(req);
            else
                response = accessError;
            break;
        case RequestType::Checkout:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleOrderRequest(req);
            else
                response = accessError;
            break;
        case RequestType::SubmitReview:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleReviewRequest(req);
            else
                response = accessError;
            break;
        case RequestType::SubmitRating:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleRatingRequest(req);
            else
                response = accessError;
            break;
        case RequestType::SaveBook:
        case RequestType::GetShelf:
            if (checkRole({UserRole::NormalUser}, accessError))
                response = handleShelfRequest(req);
            else
                response = accessError;
            break;
        case RequestType::GetNotifications:
        case RequestType::MarkNotificationRead:
            response = handleNotificationRequest(req);
            break;
        case RequestType::GetAllUsers:
        case RequestType::BlockUser:
        case RequestType::DeleteUser:
        case RequestType::DeleteBook:
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
    emit requestLogReceived(QString::number(static_cast<int>(type)), static_cast<int>(response.getStatus()));
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
            p.value("securityAnswer").toString());
    case RequestType::Login:
        return authManager.login(p.value("username").toString(), p.value("password").toString());
    case RequestType::ChangePassword:
        return authManager.changePassword(authenticatedUserId, authenticatedRole, p.value("oldPassword").toString(), p.value("newPassword").toString());
    case RequestType::RecoverPassword:
        return authManager.recoverPassword(p.value("username").toString(), p.value("securityAnswer").toString(), p.value("newPassword").toString());
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
    case RequestType::AddBook:
        return bookManager.addBook(authenticatedUserId, p.value("bookName").toString(),
                                   p.value("description").toString(), p.value("price").toDouble(),
                                   p.value("genreTitle").toString(), p.value("categoryTitle").toString(),
                                   p.value("authorName").toString(), p.value("coverImagePath").toString(),
                                   p.value("pdfFilePath").toString());
    case RequestType::UpdateBook:
        return bookManager.updateBook(authenticatedUserId, p.value("bookId").toInt(), p.value("bookName").toString(), p.value("description").toString(), p.value("price").toDouble());
    case RequestType::DeactivateBook:
        return bookManager.deactivateBook(authenticatedUserId, p.value("bookId").toInt());
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
    Q_UNUSED(req)
    OrderManager orderManager;
    return orderManager.checkout(authenticatedUserId);
}
Response ClientHandler::handleReviewRequest(const Request &req) {
    ReviewManager reviewManager;
    QVariantMap p = req.getPayload();
    return reviewManager.submitReview(authenticatedUserId, p.value("bookId").toInt(), p.value("commentText").toString(), p.value("parentId").toInt());
}
Response ClientHandler::handleRatingRequest(const Request &req) {
    RatingManager ratingManager;
    QVariantMap p = req.getPayload();
    return ratingManager.submitRating(authenticatedUserId, p.value("bookId").toInt(), p.value("ratingValue").toInt());
}
Response ClientHandler::handleShelfRequest(const Request &req) {
    ShelfManager shelfManager;
    if (req.getType() == RequestType::GetShelf)
        return shelfManager.getUserShelves(authenticatedUserId);
    return Response(ResponseStatus::Error, "درخواست قفسه نامعتبر");
}
Response ClientHandler::handleNotificationRequest(const Request &req) {
    NotificationManager notifManager;
    QVariantMap p = req.getPayload();
    if (req.getType() == RequestType::GetNotifications)
        return notifManager.getUserNotifications(authenticatedUserId);
    if (req.getType() == RequestType::MarkNotificationRead)
        return notifManager.markAsRead(authenticatedUserId, p.value("notificationId").toInt());
    return Response(ResponseStatus::Error, "درخواست اعلان نامعتبر");
}
Response ClientHandler::handleAdminRequest(const Request &req) {
    AdminManager adminManager;
    QVariantMap p = req.getPayload();
    switch (req.getType()) {
    case RequestType::GetAllUsers:
        return adminManager.getAllUsers();
    case RequestType::BlockUser:
        return adminManager.blockUser(p.value("userId").toInt());
    case RequestType::DeleteUser:
        return adminManager.deleteUser(p.value("userId").toInt());
    case RequestType::DeleteBook:
        return adminManager.removeInvalidBook(p.value("bookId").toInt());
    default:
        return Response(ResponseStatus::Error, "درخواست ادمین نامعتبر");
    }
}
Response ClientHandler::handlePublisherRequest(const Request &req) {
    Q_UNUSED(req)
    PublisherManager publisherManager;
    return publisherManager.getFullDashboard(authenticatedUserId);
}
