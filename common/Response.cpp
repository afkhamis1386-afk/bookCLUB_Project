#include "Response.h"
Response::Response():status(ResponseStatus::Error){}
Response::Response(ResponseStatus status, const QString &message, const QVariantMap &data) :status(status), message(message), data(data){}
ResponseStatus Response::getStatus() const { return status; }
QString Response::getMessage() const { return message; }
QVariantMap Response::getData() const { return data; }
bool Response::isSuccess() const { return status == ResponseStatus::Success; }
QDataStream &operator<<(QDataStream &out, const Response &res){
    out << res.status << res.message << res.data;
    return out;
}
QDataStream &operator>>(QDataStream &in, Response &res){
    in >> res.status >> res.message >> res.data;
    return in;
}