#include "ServerMonitorManager.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#endif

ServerMonitorManager::ServerMonitorManager(QObject *parent) : QObject(parent), onlineClientCount(0) {}
void ServerMonitorManager::setOnlineClientCount(int count) {
    onlineClientCount = count;
    emit statsUpdated();
}
int ServerMonitorManager::getOnlineClientCount() const {
    return onlineClientCount;
}
void ServerMonitorManager::addRequestLog(const QString &requestType, int statusCode) {
    RequestLogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.requestType = requestType;
    entry.statusCode = statusCode;
    requestLogs.append(entry);
    if (requestLogs.size() > MAX_LOG_ENTRIES) {
        requestLogs.removeFirst();
    }
    emit statsUpdated();
}
QVector<RequestLogEntry> ServerMonitorManager::getRecentLogs(int maxCount) const {
    if (requestLogs.size() <= maxCount)
        return requestLogs;
    return requestLogs.mid(requestLogs.size() - maxCount, maxCount);
}
ServerHealthStatus ServerMonitorManager::getHealthStatus() const {
    if (onlineClientCount >= OVERLOADED_THRESHOLD)
        return ServerHealthStatus::Overloaded;
    if (onlineClientCount >= BUSY_THRESHOLD)
        return ServerHealthStatus::Busy;
    return ServerHealthStatus::Healthy;
}
QString ServerMonitorManager::getHealthStatusText() const {
    switch (getHealthStatus()) {
    case ServerHealthStatus::Healthy: return "Healthy";
    case ServerHealthStatus::Busy: return "Busy";
    case ServerHealthStatus::Overloaded: return "Overloaded";
    default: return "Unknown";
    }
}
double ServerMonitorManager::getEstimatedCpuUsage() const {
    double ratio = static_cast<double>(onlineClientCount) / OVERLOADED_THRESHOLD;
    if (ratio > 1) ratio = 1;
    return ratio * 100;
}
double ServerMonitorManager::getEstimatedRamUsageMB() const {
#ifdef Q_OS_WIN
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0);
    }
#endif
    return 50.0 + (onlineClientCount * 2.0);
}