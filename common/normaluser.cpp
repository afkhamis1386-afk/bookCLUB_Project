#include "normaluser.h"
NormalUser::NormalUser():User(){}
NormalUser::NormalUser(const QString &username, const QString &plainPassword, const QString &plainAnswer)
    :User(username, plainPassword, plainAnswer){}
NormalUser::NormalUser(int userId, const QString &encryptedUsername, const QString &passwordHash, const QString &answerHash, bool isBlocked, bool isDeleted, const QDateTime &registerDate)
    :User(userId, encryptedUsername, passwordHash, answerHash, isBlocked, isDeleted, registerDate){}
NormalUser::~NormalUser(){}
QString NormalUser::getRole() const {
    return "NormalUser";
}
bool NormalUser::setFavoriteGenres(const QVector<int> &genreIds){
    QVector<int> unique;
    for(int id : genreIds){
        if(!unique.contains(id))
            unique.append(id);
    }
    if(unique.size() < 1 || unique.size() > 3)
        return false;
    favoriteGenreIds = unique;
    return true;
}
QVector<int> NormalUser::getFavoriteGenres() const {
    return favoriteGenreIds;
}
void NormalUser::setPurchasedBooks(const QVector<int> &bookIds){
    QVector<int> unique;
    for(int id : bookIds){
        if(!unique.contains(id))
            unique.append(id);
    }
    purchasedBookIds = unique;
}
void NormalUser::addPurchasedBook(int bookId){
    if(!purchasedBookIds.contains(bookId))
        purchasedBookIds.append(bookId);
}
bool NormalUser::hasPurchased(int bookId) const {
    return purchasedBookIds.contains(bookId);
}
int NormalUser::getPurchasedCount() const {
    return purchasedBookIds.size();
}
QVector<int> NormalUser::getPurchaseHistory() const {
    return purchasedBookIds;
}
void NormalUser::setSavedBooks(const QVector<int> &bookIds){
    QVector<int> unique;
    for(int id : bookIds){
        if(!unique.contains(id))
            unique.append(id);
    }
    savedBookIds = unique;
}
void NormalUser::saveBook(int bookId){
    if (!savedBookIds.contains(bookId))
        savedBookIds.append(bookId);
}
void NormalUser::removeSavedBook(int bookId){
    savedBookIds.removeAll(bookId);
}
bool NormalUser::isBookSaved(int bookId) const {
    return savedBookIds.contains(bookId);
}
QVector<int> NormalUser::getSavedBooks() const {
    return savedBookIds;
}
void NormalUser::setReadingProgress(const QMap<int, int> &progress){
    lastReadPages.clear();
    for(auto it = progress.constBegin(); it != progress.constEnd(); ++it){
        if(it.value() >= 1)
            lastReadPages[it.key()] = it.value();
    }
}
void NormalUser::setLastReadPage(int bookId, int pageNumber){
    if(pageNumber >= 1)
        lastReadPages[bookId] = pageNumber;
}
int NormalUser::getLastReadPage(int bookId) const {
    return lastReadPages.value(bookId, 1);
}