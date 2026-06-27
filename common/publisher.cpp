#ifndef PUBLISHER_CPP
#include "publisher.h"
#define PUBLISHER_CPP
Publisher::Publisher(): totalRevenue(0), totalSoldCopies(0), averageBooksRating(0) {}
Publisher::Publisher( QString username, QString password, QString securityQuestion, QString securityAnswer, QString name) :
    User(username, password,securityQuestion, securityAnswer), publisherName(name), totalRevenue(0), totalSoldCopies(0), averageBooksRating(0) {}
Publisher::~Publisher() {}
QString Publisher::getRole() const { return "Publisher"; }
QString Publisher::getPublisherName() const { return publisherName; }
void Publisher::setPublisherName(const QString& name) { publisherName = name; }
void Publisher::addPublishedBook(int bookId) {
    if(!publishedBookIds.contains(bookId))
        publishedBookIds.push_back(bookId);
}
void Publisher::removePublishedBook(int bookId) { publishedBookIds.removeAll(bookId); }
bool Publisher::hasPublishedBook(int bookId) const { return publishedBookIds.contains(bookId); }
QVector<int> Publisher::getPublishedBookIds() const { return publishedBookIds; }
int Publisher::getPublishedBooksCount() const { return publishedBookIds.size(); }
double Publisher::getTotalRevenue() const { return totalRevenue; }
void Publisher::increaseRevenue(double amount) { totalRevenue += amount; }
int Publisher::getTotalSoldCopies() const { return totalSoldCopies; }
void Publisher::increaseSoldCopies(int count) { totalSoldCopies += count; }
double Publisher::getAverageBooksRating() const { return averageBooksRating; }
void Publisher::setAverageBooksRating(double rating) { averageBooksRating = rating;}
#endif // PUBLISHER_CPP
