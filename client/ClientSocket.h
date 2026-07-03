#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include "Request.h"
#include "Response.h"
class ClientSocket : public QObject {
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);
    ~ClientSocket();
    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    void sendRequest(const Request &req);
    bool isConnected() const;
signals:
    void connected();
    void disconnected();
    void responseReceived(const Response &res);
    void errorOccurred(const QString &errorMsg);
private slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
private:
    QTcpSocket *socket;
    QByteArray buffer;
};

#endif // CLIENTSOCKET_H
