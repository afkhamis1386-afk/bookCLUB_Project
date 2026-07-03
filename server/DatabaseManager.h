#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QSqlDatabase>
#include <QString>
class DatabaseManager {
private:
    QSqlDatabase db;
    static DatabaseManager *instance;
    DatabaseManager();
public:
    static DatabaseManager* getInstance();
    bool connect(const QString &server, const QString &databaseName, const QString &username = QString(), const QString &password = QString());
    bool isConnected() const;
    QSqlDatabase& getConnection();
    void closeConnection();
    ~DatabaseManager();
};
#endif // DATABASEMANAGER_H
