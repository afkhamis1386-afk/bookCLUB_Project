#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "user.h"
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QRegularExpression>
class Publisher : public User {
private:
    QString firstName;
    QString lastName;
    QString email;
    QString shortDescription;
    QString publicationName;
    QString publisherLicenseNumber;
    QVector<int> publishedBookIds;
    bool isValidName(const QString &name) const;
public:
    Publisher();
    Publisher(const QString &username, const QString &plainPassword, const QString &plainAnswer,
              const QString &firstName, const QString &lastName, const QString &email,
              const QString &publicationName, const QString &publisherLicenseNumber,
              const QString &shortDescription = QString());
    Publisher(int userId, const QString &encryptedUsername, const QString &passwordHash,
              const QString &answerHash, bool isBlocked, bool isDeleted,
              const QDateTime &registerDate, const QString &firstName, const QString &lastName,
              const QString &email, const QString &publicationName,
              const QString &publisherLicenseNumber, const QString &shortDescription);
    ~Publisher() override;
    QString getRole() const override;
    QString getFirstName() const;
    QString getLastName() const;
    QString getFullName() const;
    QString getEmail() const;
    QString getShortDescription() const;
    QString getPublicationName() const;
    QString getPublisherLicenseNumber() const;
    bool setFirstName(const QString &name);
    bool setLastName(const QString &name);
    bool setEmail(const QString &email);
    bool setShortDescription(const QString &desc);
    bool setPublicationName(const QString &name);
    bool setPublisherLicenseNumber(const QString &license);
    void setPublishedBooks(const QVector<int> &bookIds);
    void addPublishedBook(int bookId);
    void removePublishedBook(int bookId);
    bool hasPublishedBook(int bookId) const;
    int getPublishedBooksCount() const;
    QVector<int> getPublishedBookIds() const;
    friend QDataStream &operator<<(QDataStream &out, const Publisher &publisher);
    friend QDataStream &operator>>(QDataStream &in, Publisher &publisher);
};
#endif // PUBLISHER_H