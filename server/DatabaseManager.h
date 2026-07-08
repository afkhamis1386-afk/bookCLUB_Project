#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QSqlDatabase>
#include <QString>
#include <QMutex>
#include <QThread>
class DatabaseManager {
private:
    static DatabaseManager *instance;
    static QMutex instanceMutex;
    QString serverName;
    QString databaseName;
    QString username;
    QString password;
    QMutex connectionsMutex;
    DatabaseManager();
    QString connectionNameForCurrentThread();
public:
    static DatabaseManager* getInstance();
    void configure(const QString &server, const QString &databaseName, const QString &username = QString(), const QString &password = QString());
    QSqlDatabase getConnection();
    void closeConnectionForCurrentThread();
    ~DatabaseManager();
};
#endif // DATABASEMANAGER_H
