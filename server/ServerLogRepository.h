#ifndef SERVERLOGREPOSITORY_H
#define SERVERLOGREPOSITORY_H
#include <QString>
class ServerLogRepository {
public:
    ServerLogRepository();
    bool insertLog(int userId, const QString &requestType, int statusCode, const QString &details = QString());
};
#endif // SERVERLOGREPOSITORY_H