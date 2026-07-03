#ifndef RESPONSE_H
#define RESPONSE_H
#include "Enums.h"
#include <QString>
#include <QVariantMap>
#include <QDataStream>
class Response {
private:
    ResponseStatus status;
    QString message;
    QVariantMap data;
public:
    Response();
    Response(ResponseStatus status, const QString &message, const QVariantMap &data = QVariantMap());
    ResponseStatus getStatus() const;
    QString getMessage() const;
    QVariantMap getData() const;
    bool isSuccess() const;
    friend QDataStream &operator<<(QDataStream &out, const Response &res);
    friend QDataStream &operator>>(QDataStream &in, Response &res);
};
#endif // RESPONSE_H
