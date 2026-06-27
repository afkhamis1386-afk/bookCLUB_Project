#ifndef NORMALUSER_H
#define NORMALUSER_H
#include "user.h"
#include <QString>
#include <QVector>
#include <QMap>
class NormalUser : public User {
    Q_OBJECT
private:
    QVector<QString> favoriteGenres;
    QVector<int> purchasedBooksIds;
    QVector<int> savedBooksIds;
    double walletBalance;
    QMap<int, int> lastReadPages;
public:
    NormalUser();
    NormalUser(QString username, QString password, QString securityQuestion, QString securityAnswer, double balance = 0.0);
    ~NormalUser() ;
    QString getRole() const;
    bool setFavoriteGenres(const QVector<QString> &genres);
    QVector<QString> getFavoriteGenres() const;
    void addPurchasedBook(int bookId);
    int getPurchasedCount() const;
    QVector<int> getPurchaseHistory() const;
    void saveBook(int bookId);
    void removeSavedBook(int bookId);
    QVector<int> getSavedBooks() const;
    double getWalletBalance() const;
    bool deductFromWallet(double amount);
    void addToWallet(double amount);
    bool addReview(int bookId, QString comment, int rating);
    bool editReview(int reviewId, QString newComment);
    bool deleteReview(int reviewId);
    bool readBook(int bookId);
    void saveLastReadPage(int bookId, int pageNumber);
    int getLastReadPage(int bookId) const;
};
#endif // NORMALUSER_H
