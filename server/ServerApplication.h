#ifndef SERVERAPPLICATION_H
#define SERVERAPPLICATION_H
#include <QObject>
#include <QTcpServer>
#include <QThread>
#include <QMap>
#include <QMutex>
class ServerApplication : public QTcpServer {
    Q_OBJECT

public:
    explicit ServerApplication(QObject *parent = nullptr);
    ~ServerApplication();
    bool startListening(quint16 port);
    void stopListening();
    int getOnlineClientCount() const;
signals:
    void requestProcessed(const QString &requestType, int statusCode);
    void clientConnected(qintptr socketDescriptor);
    void clientDisconnected(qintptr socketDescriptor);
    void logMessage(const QString &message);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private slots:
    void onClientThreadFinished();
private:
    mutable QMutex activeThreadsMutex;
    QMap<qintptr, QThread*> activeThreads;
};
#endif // SERVERAPPLICATION_H
