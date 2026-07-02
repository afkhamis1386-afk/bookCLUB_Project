#include "Shelf.h"
Shelf::Shelf():shelfId(-1), userId(-1){}
Shelf::Shelf(int userId, const QString &shelfName):shelfId(-1), userId(userId), shelfName(shelfName.trimmed()){}
Shelf::Shelf(int shelfId, int userId, const QString &shelfName):shelfId(shelfId), userId(userId), shelfName(shelfName){}
int Shelf::getShelfId() const { return shelfId; }
int Shelf::getUserId() const { return userId; }
QString Shelf::getShelfName() const { return shelfName; }
QVector<int> Shelf::getBookIds() const { return bookIds; }
int Shelf::getBookCount() const { return bookIds.size(); }
void Shelf::setShelfId(int id){ shelfId = id; }
bool Shelf::setShelfName(const QString &name){
    if(name.trimmed().isEmpty() || name.length() > 100)
        return false;
    shelfName = name.trimmed();
    return true;
}
bool Shelf::addBook(int bookId){
    if(bookIds.contains(bookId))
        return false;
    bookIds.append(bookId);
    return true;
}
bool Shelf::removeBook(int bookId){
    if(!bookIds.contains(bookId))
        return false;
    bookIds.removeAll(bookId);
    return true;
}
bool Shelf::containsBook(int bookId) const {
    return bookIds.contains(bookId);
}
void Shelf::setBookIds(const QVector<int> &ids){
    QVector<int> unique;
    for(int id : ids){
        if(!unique.contains(id))
            unique.append(id);
    }
    bookIds = unique;
}
QDataStream &operator<<(QDataStream &out, const Shelf &shelf){
    out << shelf.shelfId << shelf.userId << shelf.shelfName << shelf.bookIds;
    return out;
}
QDataStream &operator>>(QDataStream &in, Shelf &shelf){
    in >> shelf.shelfId >> shelf.userId >> shelf.shelfName >> shelf.bookIds;
    return in;
}