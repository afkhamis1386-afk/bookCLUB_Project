#include "ClientSocket.h"
#include <QDebug>

ClientSocket::ClientSocket(QObject *parent)
    : QObject(parent) , socket(new QTcpSocket(this)) {
    connect(socket, &QTcpSocket::connected, this, &ClientSocket::connected);
    connect(socket, &QTcpSocket::disconnected, this, &ClientSocket::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &ClientSocket::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientSocket::onError);
}
ClientSocket::~ClientSocket() {
    if (socket->isOpen())
        socket->close();
}
void ClientSocket::connectToServer(const QString &host, quint16 port) {
    qDebug() << "Connecting to server : " << host << ":" << port;
    socket->connectToHost(host, port);
}
void ClientSocket::disconnectFromServer() {
    if (socket->state() == QAbstractSocket::ConnectedState)
        socket->disconnectFromHost();
}
void ClientSocket::sendRequest(const Request &req) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Error: Socket is not connected!";
        emit errorOccurred("Socket is not connected to the server");
        return;
    }
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << req;
    QByteArray packet;
    QDataStream packetStream(&packet, QIODevice::WriteOnly);
    packetStream.setVersion(QDataStream::Qt_6_5);
    packetStream << static_cast<quint32>(data.size());
    packet.append(data);
    qint64 written = socket->write(packet);
    if (written == -1) {
        qWarning() << "Error sending data : " << socket->errorString();
        emit errorOccurred("Error sending data");
    }
    else {
        socket->flush();
        qDebug() << "Request sent - Type : "  << static_cast<int>(req.getType());
    }
}
bool ClientSocket::isConnected() const {
    return socket->state() == QAbstractSocket::ConnectedState;
}
void ClientSocket::onReadyRead() {
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
        Response res;
        msgStream >> res;
        qDebug() << "Response received - Status : " << static_cast<int>(res.getStatus());
        emit responseReceived(res);
    }
}
void ClientSocket::onError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    QString errorMsg = socket->errorString();
    qWarning() << "Socket error : " << errorMsg;
    emit errorOccurred(errorMsg);
}
