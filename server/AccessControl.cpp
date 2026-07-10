#include "AccessControl.h"
#include "UserRepository.h"
#include "PublisherRepository.h"
#include "AdminRepository.h"
#include <memory>
AccessControl::AccessControl(){}
bool AccessControl::isUserBlocked(int userId, UserRole role){
    if(role == UserRole::NormalUser){
        UserRepository userRepo;
        NormalUser *user = userRepo.loadNormalUserById(userId);
        if(!user) return true;
        bool blocked = user->getIsBlocked();
        delete user;
        return blocked;
    }
    else if(role == UserRole::Publisher){
        PublisherRepository publisherRepo;
        Publisher *publisher = publisherRepo.loadPublisherById(userId);
        if (!publisher) return true;
        bool blocked = publisher->getIsBlocked();
        delete publisher;
        return blocked;
    }
    else {
        AdminRepository adminRepo;
        Admin *admin = adminRepo.loadAdminById(userId);
        if (!admin) return true;
        bool blocked = admin->getIsBlocked();
        delete admin;
        return blocked;
    }
}
bool AccessControl::hasPermission(UserRole userRole, const QVector<UserRole> &allowedRoles){
    return allowedRoles.contains(userRole);
}
bool AccessControl::isUserInactive(int userId, UserRole role){
    if(role == UserRole::NormalUser){
        UserRepository userRepo;
        std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
        return !user || !user->getIsActive();
    }
    else if(role == UserRole::Publisher){
        PublisherRepository publisherRepo;
        std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(userId));
        return !publisher || !publisher->getIsActive();
    }
    return false;
}
Response AccessControl::checkAccess(int userId, UserRole role, const QVector<UserRole> &allowedRoles){
    if(userId <= 0){
        return Response(ResponseStatus::Unauthorized, "برای انجام این عملیات باید وارد سیستم شوید");
    }
    if(isUserBlocked(userId, role)){
        return Response(ResponseStatus::Unauthorized, "حساب کاربری شما مسدود شده است");
    }
    if(isUserInactive(userId, role)){
        return Response(ResponseStatus::Unauthorized, "حساب کاربری شما غیرفعال شده است");
    }
    if(!hasPermission(role, allowedRoles)){
        return Response(ResponseStatus::Unauthorized, "شما اجازه ی انجام این عملیات را ندارید");
    }
    return Response(ResponseStatus::Success, "دسترسی مجاز است");
}