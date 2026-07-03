#ifndef REQUEST_H
#define REQUEST_H
#include "Enums.h"
#include <QVariantMap>
#include <QDataStream>
class Request {
private:
    RequestType type;
    int senderUserId;
    QVariantMap payload;
public:
    Request();
    Request(RequestType type, const QVariantMap &payload, int senderUserId = -1);
    RequestType getType() const;
    int getSenderUserId() const;
    QVariantMap getPayload() const;
    void setSenderUserId(int id);
    QVariant getValue(const QString &key) const;
    friend QDataStream &operator<<(QDataStream &out, const Request &req);
    friend QDataStream &operator>>(QDataStream &in, Request &req);
};

#endif // REQUEST_H
