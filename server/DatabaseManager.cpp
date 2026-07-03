#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlError>
DatabaseManager* DatabaseManager::instance = nullptr;
DatabaseManager::DatabaseManager(){
    db = QSqlDatabase::addDatabase("QODBC");
}
DatabaseManager* DatabaseManager::getInstance(){
    if(instance == nullptr){
        instance = new DatabaseManager();
    }
    return instance;
}
bool DatabaseManager::connect(const QString &server, const QString &databaseName, const QString &username, const QString &password){
    QString connectionString = QString(
       "DRIVER={ODBC Driver 17 for SQL Server};"
       "SERVER=%1;"
       "DATABASE=%2;"
       "Trusted_Connection=Yes;").arg(server, databaseName);
    db.setDatabaseName(connectionString);
    if(!username.isEmpty()){
        db.setUserName(username);
        db.setPassword(password);
    }
    if(!db.open()){
        qDebug() << "خطا در اتصال به دیتابیس:" << db.lastError().text();
        return false;
    }
    qDebug() << "اتصال به دیتابیس با موفقیت برقرار شد.";
    return true;
}
bool DatabaseManager::isConnected() const {
    return db.isOpen();
}
QSqlDatabase& DatabaseManager::getConnection(){
    return db;
}
void DatabaseManager::closeConnection(){
    if(db.isOpen())
        db.close();
}
DatabaseManager::~DatabaseManager(){
    closeConnection();
}