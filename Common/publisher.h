#ifndef PUBLISHER_H
#define PUBLISHER_H
#include "user.h"
#include <QString>
#include <QVector>
class Publisher : public User {
private :
    QString publisherName;
    QVector<int> publishedBookIds;
    double totalRevenue;
    int totalSoldCopies;
    double averageBooksRating;
public :
    Publisher();
    Publisher(QString username, QString password, QString securityQuestion, QString securityAnswer, QString publisherName);
    ~Publisher();
    QString getRole() const override;
    QString getPublisherName() const;
    void setPublisherName(const QString& name);
    void addPublishedBook(int bookId);
    void removePublishedBook(int bookId);
    bool hasPublishedBook(int bookId) const;
    QVector<int> getPublishedBookIds() const;
    int getPublishedBooksCount() const;
    double getTotalRevenue() const;
    void increaseRevenue(double amount);
    int getTotalSoldCopies() const;
    void increaseSoldCopies(int count);
    double getAverageBooksRating() const;
    void setAverageBooksRating(double rating);
};

#endif // PUBLISHER_H