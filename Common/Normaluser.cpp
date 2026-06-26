#include "normaluser.h"
NormalUser::NormalUser() : User(), walletBalance(0.0) {}
NormalUser::NormalUser(QString us, QString pwh, QString sq, QString hsqa, bool ib, double balance)
    : User(us, pwh, sq, hsqa, ib), walletBalance(balance) {}
NormalUser::~NormalUser() {}
QString NormalUser::getRole() const {
    return "NormalUser";
}
bool NormalUser::setFavoriteGenres(const QVector<QString> &genres){
    if(genres.size() < 1 || genres.size() > 3)
        return false;
    favoriteGenres = genres;
    return true;
}
QVector<QString> NormalUser::getFavoriteGenres() const {
    return favoriteGenres;
}
void NormalUser::addPurchasedBook(int bookId){
    if(!purchasedBooksIds.contains(bookId))
        purchasedBooksIds.append(bookId);
}
int NormalUser::getPurchasedCount() const {
    return purchasedBooksIds.size();
}
QVector<int> NormalUser::getPurchaseHistory() const {
    return purchasedBooksIds;
}
void NormalUser::saveBook(int bookId){
    if(!savedBooksIds.contains(bookId))
        savedBooksIds.append(bookId);
}
void NormalUser::removeSavedBook(int bookId){
    savedBooksIds.removeAll(bookId);
}
QVector<int> NormalUser::getSavedBooks() const {
    return savedBooksIds;
}
double NormalUser::getWalletBalance() const {
    return walletBalance;
}
bool NormalUser::deductFromWallet(double amount){
    if(amount < 0 || amount > walletBalance)
        return false;
    walletBalance -= amount;
    return true;
}
void NormalUser::addToWallet(double amount){
    if (amount > 0)
        walletBalance += amount;
}
bool NormalUser::addReview(int bookId, QString comment, int rating){
    if(rating < 1 || rating > 5)
        return false;
    if(!purchasedBooksIds.contains(bookId))
        return false;
    return true;
}
bool NormalUser::editReview(int reviewId, QString newComment){
    return true;
}
bool NormalUser::deleteReview(int reviewId){
    return true;
}
bool NormalUser::readBook(int bookId){
    if(!purchasedBooksIds.contains(bookId))
        return false;
    return true;
}
void NormalUser::saveLastReadPage(int bookId, int pageNumber){
    lastReadPages[bookId] = pageNumber;
}
int NormalUser::getLastReadPage(int bookId) const {
    return lastReadPages.value(bookId, 0);
}