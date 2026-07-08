#ifndef SERVERMONITORMANAGER_H
#define SERVERMONITORMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>

enum class ServerHealthStatus {
    Healthy,
    Busy,
    Overloaded
};
struct RequestLogEntry {
    QDateTime timestamp;
    QString requestType;
    int statusCode;
};
class ServerMonitorManager : public QObject {
    Q_OBJECT

public:
    explicit ServerMonitorManager(QObject *parent = nullptr);
    void setOnlineClientCount(int count);
    int getOnlineClientCount() const;
    void addRequestLog(const QString &requestType, int statusCode);
    QVector<RequestLogEntry> getRecentLogs(int maxCount = 50) const;
    ServerHealthStatus getHealthStatus() const;
    QString getHealthStatusText() const;
    double getEstimatedCpuUsage() const;
    double getEstimatedRamUsageMB() const;
signals:
    void statsUpdated();
private:
    int onlineClientCount;
    QVector<RequestLogEntry> requestLogs;
    static const int MAX_LOG_ENTRIES = 200;
    static const int BUSY_THRESHOLD = 10;
    static const int OVERLOADED_THRESHOLD = 25;
};

#endif // SERVERMONITORMANAGER_H