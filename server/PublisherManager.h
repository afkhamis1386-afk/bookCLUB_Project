#ifndef PUBLISHERMANAGER_H
#define PUBLISHERMANAGER_H
#include "../common/Response.h"
#include <QString>
class PublisherManager {
public:
    PublisherManager();
    Response getProfile(int publisherUserId);
    Response updateProfile(int publisherUserId, const QString &firstName, const QString &lastName, const QString &email, const QString &shortDescription, const QString &publicationName);
    Response getMyBooks(int publisherUserId);
    Response getFullDashboard(int publisherUserId);
};
#endif // PUBLISHERMANAGER_H

