#ifndef PUBLISHERREPOSITORY_H
#define PUBLISHERREPOSITORY_H
#include "../common/publisher.h"
#include <QString>
#include <QVector>
class PublisherRepository {
public:
    PublisherRepository();
    int insertPublisher(const Publisher &publisher);
    Publisher* loadPublisherById(int userId);
    bool updateProfile(int userId, const QString &firstName, const QString &lastName, const QString &email, const QString &shortDescription, const QString &publicationName);
    bool updateAccount(int userId, const QString &encryptedUsername, const QString &newPasswordHash,
                       const QString &newSecurityAnswerHash, const QString &firstName,
                       const QString &lastName, const QString &email, const QString &shortDescription,
                       const QString &publicationName, const QString &publisherLicenseNumber);
    QVector<int> getAllPublisherIds();
    bool isEmailTaken(const QString &email, int excludedUserId = -1);
    bool isLicenseNumberTaken(const QString &licenseNumber, int excludedUserId = -1);
    double getTotalRevenue(int publisherUserId);
};
#endif // PUBLISHERREPOSITORY_H
