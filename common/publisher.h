#ifndef PUBLISHER_H
#define PUBLISHER_H
#include "user.h"
#include "Book.h"
#include <QString>
#include <memory>
#include <QVector>
#include <QRegularExpression>
#include <QDataStream>
class Publisher: public User {
private :
    QString firstName;
    QString lastName;
    QString publicationName;
    QString email;
    QString description;
    QString licenseNumber;
    QVector<std::shared_ptr<Book>> publishedBooks;
public :
    Publisher();
    Publisher(const QString& username,
              const QString& password,
              const QString& securityAnswer,
              const QString& firstName,
              const QString& lastName,
              const QString& publicationName,
              const QString& email,
              const QString& description,
              const QString& licenseNumber);
    ~Publisher() override;
    QString getRole() const override;
    QString getFirstName() const;
    void setFirstName(const QString& fName);
    QString getLastName() const;
    void setLastName(const QString& lName);
    QString getPublicationName() const;
    void setPublicationName(const QString& name);
    QString getEmail() const;
    void setEmail(const QString& emailValue);
    QString getDescription() const;
    void setDescription(const QString& desc);
    QString getLicenseNumber() const;
    void setLicenseNumber(const QString& license);
    void addBook(const std::shared_ptr<Book>& book);
    void deactivateBook(int bookId);
    void activateBook(int bookId);
    const QVector<std::shared_ptr<Book>>& getPublishedBooks() const;
    int getPublishedBooksCount() const;
    double getTotalRevenue() const;
    int getTotalSoldCopies() const;
    double calculateAverageBooksRating() const;
    QVector<std::shared_ptr<Book>> getTopSellingBooks(int limit = 5) const;
    QVector<std::shared_ptr<Book>> getLeastSellingBooks(int limit = 5) const;
    std::shared_ptr<Book> findBook(int bookId) const;
    void updateBook(int bookId,
                    const QString& title,
                    const QString& author,
                    const QString& genre,
                    const QString& description,
                    double price,
                    double discount);
    friend QDataStream &operator<<(QDataStream &out, const Publisher &p);
    friend QDataStream &operator>>(QDataStream &in, Publisher &p);


private :
    bool isValidName(const QString& name) const;
};
#endif // PUBLISHER_H