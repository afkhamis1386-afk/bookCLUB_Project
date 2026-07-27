#ifndef NORMALUSER_H
#define NORMALUSER_H
#include "user.h"
#include <QVector>
#include <QMap>
#include <QDateTime>
#include <QString>
class NormalUser : public User {
private:
    QString firstName;
    QString lastName;
    QVector<int> favoriteGenreIds;
    QVector<int> purchasedBookIds;
    QVector<int> savedBookIds;
    QMap<int, int> lastReadPages;
public:
    NormalUser();
    NormalUser(const QString &username, const QString &plainPassword, const QString &plainAnswer, const QString &firstName, const QString &lastName);
    NormalUser(int userId, const QString &encryptedUsername, const QString &passwordHash, const QString &answerHash, bool isBlocked, bool isDeleted, bool isActive,
    const QDateTime &registerDate, const QString &firstName, const QString &lastName);
    ~NormalUser() override;
    QString getRole() const override;
    QString getFirstName() const;
    QString getLastName() const;
    QString getFullName() const;
    bool setFirstName(const QString &name);
    bool setLastName(const QString &name);
    bool setFavoriteGenres(const QVector<int> &genreIds);
    QVector<int> getFavoriteGenres() const;
    void setPurchasedBooks(const QVector<int> &bookIds);
    void addPurchasedBook(int bookId);
    bool hasPurchased(int bookId) const;
    int getPurchasedCount() const;
    QVector<int> getPurchaseHistory() const;
    void setSavedBooks(const QVector<int> &bookIds);
    void saveBook(int bookId);
    void removeSavedBook(int bookId);
    bool isBookSaved(int bookId) const;
    QVector<int> getSavedBooks() const;
    void setReadingProgress(const QMap<int, int> &progress);
    void setLastReadPage(int bookId, int pageNumber);
    int getLastReadPage(int bookId) const;
};
#endif // NORMALUSER_H
