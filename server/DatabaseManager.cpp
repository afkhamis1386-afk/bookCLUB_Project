#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlError>
DatabaseManager* DatabaseManager::instance = nullptr;
QMutex DatabaseManager::instanceMutex;
DatabaseManager::DatabaseManager(){}
DatabaseManager* DatabaseManager::getInstance(){
    QMutexLocker locker(&instanceMutex);
    if(instance == nullptr){
        instance = new DatabaseManager();
    }
    return instance;
}
void DatabaseManager::configure(const QString &server, const QString &databaseName, const QString &username, const QString &password){
    this->serverName = server;
    this->databaseName = databaseName;
    this->username = username;
    this->password = password;
}
QString DatabaseManager::connectionNameForCurrentThread(){
    return QString("conn_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
}
QSqlDatabase DatabaseManager::getConnection(){
    QMutexLocker locker(&connectionsMutex);
    QString connName = connectionNameForCurrentThread();
    if(QSqlDatabase::contains(connName)){
        QSqlDatabase db = QSqlDatabase::database(connName);
        if(db.isOpen())
            return db;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", connName);
    QString connectionString = QString(
     "DRIVER={ODBC Driver 17 for SQL Server};"
     "SERVER=%1;"
     "DATABASE=%2;"
     "Trusted_Connection=Yes;"
     ).arg(serverName, databaseName);
    db.setDatabaseName(connectionString);
    if(!username.isEmpty()){
        db.setUserName(username);
        db.setPassword(password);
    }
    if(!db.open()){
        qWarning() << "خطا در اتصال به دیتابیس (Thread:" << connName << "):" << db.lastError().text();
    }
    return db;
}
void DatabaseManager::closeConnectionForCurrentThread(){
    QMutexLocker locker(&connectionsMutex);
    QString connName = connectionNameForCurrentThread();
    if(QSqlDatabase::contains(connName)){
        {
        QSqlDatabase db = QSqlDatabase::database(connName);
        if(db.isOpen())
        db.close();
        }
        QSqlDatabase::removeDatabase(connName);
    }
}
DatabaseManager::~DatabaseManager(){}