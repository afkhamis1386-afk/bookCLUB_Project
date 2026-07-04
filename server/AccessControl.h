#ifndef ACCESSCONTROL_H
#define ACCESSCONTROL_H
#include "../common/Response.h"
#include "../common/Enums.h"
#include <QVector>
class AccessControl {
public:
    AccessControl();
    bool isUserBlocked(int userId, UserRole role);
    bool hasPermission(UserRole userRole, const QVector<UserRole> &allowedRoles);
    Response checkAccess(int userId, UserRole role, const QVector<UserRole> &allowedRoles);
};

#endif // ACCESSCONTROL_H
