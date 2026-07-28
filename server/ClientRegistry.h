#ifndef CLIENTREGISTRY_H
#define CLIENTREGISTRY_H
#include <QObject>
#include <QMap>
#include <QMutex>
#include "../common/Notification.h"
class ClientHandler;
class ClientRegistry : public QObject {
    Q_OBJECT

private:
    static ClientRegistry *instance;
    static QMutex instanceMutex;
    QMutex mapMutex;
    QMap<int, ClientHandler*> onlineUsers;
    ClientRegistry();
public:
    static ClientRegistry* getInstance();
    void registerClient(int userId, ClientHandler *handler);
    void unregisterClient(int userId);
    bool isUserOnline(int userId) const;
    void pushToUser(int userId, const Notification &notification);
    void broadcastLiveUpdate(const QString &updateType, const QVariantMap &payload);
};
#endif // CLIENTREGISTRY_H
