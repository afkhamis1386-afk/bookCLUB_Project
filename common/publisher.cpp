#include "publisher.h"
bool Publisher::isValidName(const QString &name) const {
    static const QRegularExpression nameRegex(R"(^[a-zA-Z\s\x{0600}-\x{06FF}]+$)");
    return !name.trimmed().isEmpty() && nameRegex.match(name).hasMatch();
}
Publisher::Publisher() : User() {}
Publisher::Publisher(const QString &username, const QString &plainPassword, const QString &plainAnswer,
                     const QString &firstName, const QString &lastName, const QString &email,
                     const QString &publicationName, const QString &publisherLicenseNumber,
                     const QString &shortDescription)
    : User(username, plainPassword, plainAnswer),
    firstName(firstName.trimmed()),
    lastName(lastName.trimmed()),
    email(email.trimmed()),
    shortDescription(shortDescription),
    publicationName(publicationName.trimmed()),
    publisherLicenseNumber(publisherLicenseNumber.trimmed()) {}
Publisher::Publisher(int userId, const QString &encryptedUsername, const QString &passwordHash,
                     const QString &answerHash, bool isBlocked, bool isDeleted, bool isActive,
                     const QDateTime &registerDate, const QString &firstName, const QString &lastName,
                     const QString &email, const QString &publicationName,
                     const QString &publisherLicenseNumber, const QString &shortDescription)
    : User(userId, encryptedUsername, passwordHash, answerHash, isBlocked, isDeleted, isActive, registerDate),
    firstName(firstName),
    lastName(lastName),
    email(email),
    shortDescription(shortDescription),
    publicationName(publicationName),
    publisherLicenseNumber(publisherLicenseNumber) {}
Publisher::~Publisher() {}
QString Publisher::getRole() const { return "Publisher"; }
QString Publisher::getFirstName() const { return firstName; }
QString Publisher::getLastName() const { return lastName; }
QString Publisher::getFullName() const { return firstName + " " + lastName; }
QString Publisher::getEmail() const { return email; }
QString Publisher::getShortDescription() const { return shortDescription; }
QString Publisher::getPublicationName() const { return publicationName; }
QString Publisher::getPublisherLicenseNumber() const { return publisherLicenseNumber; }
bool Publisher::setFirstName(const QString &name) {
    if (!isValidName(name) || name.trimmed().length() > 30)
        return false;
    firstName = name.trimmed();
    return true;
}
bool Publisher::setLastName(const QString &name) {
    if (!isValidName(name) || name.trimmed().length() > 30)
        return false;
    lastName = name.trimmed();
    return true;
}
bool Publisher::setEmail(const QString &emailValue) {
    static const QRegularExpression emailRegex(R"(^[\w\.-]+@[\w\.-]+\.\w{2,}$)");
    if (emailValue.trimmed().isEmpty() || emailValue.length() > 120)
        return false;
    if (!emailRegex.match(emailValue).hasMatch())
        return false;
    email = emailValue.trimmed();
    return true;
}
bool Publisher::setShortDescription(const QString &desc) {
    if (desc.length() > 500)
        return false;
    shortDescription = desc;
    return true;
}
bool Publisher::setPublicationName(const QString &name) {
    if (!isValidName(name) || name.trimmed().length() > 50)
        return false;
    publicationName = name.trimmed();
    return true;
}
bool Publisher::setPublisherLicenseNumber(const QString &license) {
    QString clean = license.trimmed();
    if (clean.isEmpty() || clean.length() > 13)
        return false;
    publisherLicenseNumber = clean;
    return true;
}
void Publisher::setPublishedBooks(const QVector<int> &bookIds) {
    QVector<int> unique;
    for (int id : bookIds) {
        if (!unique.contains(id))
            unique.append(id);
    }
    publishedBookIds = unique;
}
void Publisher::addPublishedBook(int bookId) {
    if (!publishedBookIds.contains(bookId))
        publishedBookIds.append(bookId);
}
void Publisher::removePublishedBook(int bookId) {
    publishedBookIds.removeAll(bookId);
}
bool Publisher::hasPublishedBook(int bookId) const { return publishedBookIds.contains(bookId); }
int Publisher::getPublishedBooksCount() const { return publishedBookIds.size(); }
QVector<int> Publisher::getPublishedBookIds() const { return publishedBookIds; }
QDataStream &operator<<(QDataStream &out, const Publisher &publisher) {
    publisher.serializeBase(out);
    out << publisher.firstName << publisher.lastName << publisher.email
        << publisher.shortDescription << publisher.publicationName
        << publisher.publisherLicenseNumber << publisher.publishedBookIds;
    return out;
}
QDataStream &operator>>(QDataStream &in, Publisher &publisher) {
    publisher.deserializeBase(in);
    in >> publisher.firstName >> publisher.lastName >> publisher.email
        >> publisher.shortDescription >> publisher.publicationName
        >> publisher.publisherLicenseNumber >> publisher.publishedBookIds;
    return in;
}