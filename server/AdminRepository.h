#ifndef ADMINREPOSITORY_H
#define ADMINREPOSITORY_H
#include "DatabaseManager.h"
#include "../common/Admin.h"
#include <QString>
#include <QVector>
class AdminRepository {
public:
    AdminRepository();
    int insertAdmin(const Admin &admin);
    Admin* loadAdminById(int userId);
    bool updateFirstName(int userId, const QString &firstName);
    bool updateLastName(int userId, const QString &lastName);
    QVector<int> getAllAdminIds();
    bool isAdminExists(int userId);
};
#endif // ADMINREPOSITORY_H
