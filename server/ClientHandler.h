#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H
#include "../common/Notification.h"
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include "../common/Request.h"
#include "../common/Response.h"
#include "../common/Enums.h"
class ClientHandler : public QObject {
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);
    Q_INVOKABLE void pushNotificationToClient(const Notification &notification);
    int getAuthenticatedUserId() const { return authenticatedUserId; }
    static constexpr quint32 MAX_FRAME_SIZE = 150u * 1024u * 1024u;
public slots:
    void run();
    void sendLiveUpdateToClient(const Response &liveUpdate);
signals:
    void clientDisconnected(qintptr socketDescriptor);
    void requestLogReceived(const QString &requestType, int statusCode);
private slots:
    void onReadyRead();
    void onDisconnected();
private:
    qintptr socketDescriptor;
    QTcpSocket *socket;
    QByteArray buffer;
    int authenticatedUserId;
    UserRole authenticatedRole;
    bool isAuthenticated;
    void processRequest(const Request &req);
    void sendResponse(const Response &res);
    bool checkRole(const QVector<UserRole> &allowedRoles, Response &outErrorResponse);
    Response handleLogoutRequest();
    Response handleAuthRequest(const Request &req);
    Response handleBookRequest(const Request &req);
    Response handleCartRequest(const Request &req);
    Response handleOrderRequest(const Request &req);
    Response handleReviewRequest(const Request &req);
    Response handleRatingRequest(const Request &req);
    Response handleShelfRequest(const Request &req);
    Response handleSavedBookRequest(const Request &req);
    Response handleNotificationRequest(const Request &req);
    Response handleAdminRequest(const Request &req);
    Response handlePublisherRequest(const Request &req);
};

#endif // CLIENTHANDLER_H