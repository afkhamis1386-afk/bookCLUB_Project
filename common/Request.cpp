#include "Request.h"
Request::Request():type(RequestType::Login), senderUserId(-1){}
Request::Request(RequestType type, const QVariantMap &payload, int senderUserId):type(type), senderUserId(senderUserId), payload(payload){}
RequestType Request::getType() const { return type; }
int Request::getSenderUserId() const { return senderUserId; }
QVariantMap Request::getPayload() const { return payload; }
void Request::setSenderUserId(int id){ senderUserId = id; }
QVariant Request::getValue(const QString &key) const {
    return payload.value(key);
}
QDataStream &operator<<(QDataStream &out, const Request &req){
    out << req.type << req.senderUserId << req.payload;
    return out;
}
QDataStream &operator>>(QDataStream &in, Request &req){
    in >> req.type >> req.senderUserId >> req.payload;
    return in;
}