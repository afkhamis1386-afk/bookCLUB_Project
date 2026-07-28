#include "ServerApplication.h"
#include "ClientHandler.h"
#include <QDebug>
ServerApplication::ServerApplication(QObject *parent):QTcpServer(parent){}
ServerApplication::~ServerApplication(){
    stopListening();
}
bool ServerApplication::startListening(quint16 port){
    if(!this->listen(QHostAddress::Any, port)){
        qWarning() << "خطا در شروع گوش دادن سرور:" << this->errorString();
        return false;
    }
    qDebug() << "سرور در حال گوش دادن روی پورت:" << port;
    return true;
}
void ServerApplication::stopListening(){
    if(this->isListening()){
        emit logMessage("سرور در حال توقف...");
        this->close();
    }
    QMutexLocker locker(&activeThreadsMutex);
    for(auto it = activeThreads.begin(); it != activeThreads.end(); ++it) {
        QThread *thread = it.value();
        thread->quit();
        if(!thread->wait(3000)){
            qWarning() << "ترد به زور بسته شد";
            thread->terminate();
            thread->wait();
        }
    }
    activeThreads.clear();
}
int ServerApplication::getOnlineClientCount() const {
    QMutexLocker locker(&activeThreadsMutex);
    return activeThreads.size();
}
void ServerApplication::incomingConnection(qintptr socketDescriptor){
    QThread *thread = new QThread();
    ClientHandler *handler = new ClientHandler(socketDescriptor);
    handler->moveToThread(thread);
    connect(thread, &QThread::started, handler, &ClientHandler::run);
    connect(handler, &ClientHandler::clientDisconnected, this, [this](qintptr sd){
    emit clientDisconnected(sd);
    });
    connect(handler, &ClientHandler::requestLogReceived, this, &ServerApplication::requestProcessed);
    connect(handler, &ClientHandler::clientDisconnected, thread, &QThread::quit);
    connect(thread, &QThread::finished, handler, &QObject::deleteLater);
    connect(thread, &QThread::finished, this, &ServerApplication::onClientThreadFinished);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    {
        QMutexLocker locker(&activeThreadsMutex);
        activeThreads.insert(socketDescriptor, thread);
    }
    thread->start();
    emit clientConnected(socketDescriptor);
    emit logMessage(QString("کلاینت جدید متصل شد. تعداد کل: %1").arg(getOnlineClientCount()));
}
void ServerApplication::onClientThreadFinished() {
    QThread *finishedThread = qobject_cast<QThread*>(sender());
    if(!finishedThread) return;
    QMutexLocker locker(&activeThreadsMutex);
    qintptr keyToRemove = -1;
    for(auto it = activeThreads.begin(); it != activeThreads.end(); ++it){
        if(it.value() == finishedThread){
            keyToRemove = it.key();
            break;
        }
    }
    if(keyToRemove != -1){
        activeThreads.remove(keyToRemove);
        emit logMessage(QString("کلاینت قطع شد. تعداد باقی مانده: %1").arg(activeThreads.size()));
    }
}