#include "ClientRegistry.h"
#include "ClientHandler.h"
#include <QMetaObject>
ClientRegistry* ClientRegistry::instance = nullptr;
QMutex ClientRegistry::instanceMutex;
ClientRegistry::ClientRegistry(){}
ClientRegistry* ClientRegistry::getInstance(){
    QMutexLocker locker(&instanceMutex);
    if(instance == nullptr){
        instance = new ClientRegistry();
    }
    return instance;
}
void ClientRegistry::registerClient(int userId, ClientHandler *handler){
    QMutexLocker locker(&mapMutex);
    onlineUsers.insert(userId, handler);
}
void ClientRegistry::unregisterClient(int userId){
    QMutexLocker locker(&mapMutex);
    onlineUsers.remove(userId);
}
bool ClientRegistry::isUserOnline(int userId) const {
    QMutexLocker locker(const_cast<QMutex*>(&mapMutex));
    return onlineUsers.contains(userId);
}
void ClientRegistry::pushToUser(int userId, const Notification &notification){
    ClientHandler *handler = nullptr; {
        QMutexLocker locker(&mapMutex);
        if(!onlineUsers.contains(userId))
            return;
        handler = onlineUsers.value(userId);
    }
    QMetaObject::invokeMethod(handler, "pushNotificationToClient", Qt::QueuedConnection, Q_ARG(Notification, notification));
}
void ClientRegistry::broadcastLiveUpdate(const QString &updateType, const QVariantMap &payload){
    QVariantMap data = payload;
    data["liveUpdateType"] = updateType;
    Response liveUpdate(ResponseStatus::PushNotification, "به روزرسانی لحظه ای", data);
    QMap<int, ClientHandler*> snapshot;
    {
        QMutexLocker locker(&mapMutex);
        snapshot = onlineUsers;
    }
    for(auto it = snapshot.begin(); it != snapshot.end(); ++it){
     QMetaObject::invokeMethod(it.value(), "sendLiveUpdateToClient",
     Qt::QueuedConnection, Q_ARG(Response, liveUpdate));
    }
}