#ifndef SHELF_H
#define SHELF_H
#include <QString>
#include <QVector>
#include <QDataStream>
class Shelf {
private:
    int shelfId;
    int userId;
    QString shelfName;
    QVector<int> bookIds;
public:
    Shelf();
    Shelf(int userId, const QString &shelfName);
    Shelf(int shelfId, int userId, const QString &shelfName);
    int getShelfId() const;
    int getUserId() const;
    QString getShelfName() const;
    QVector<int> getBookIds() const;
    int getBookCount() const;
    void setShelfId(int id);
    bool setShelfName(const QString &name);
    bool addBook(int bookId);
    bool removeBook(int bookId);
    bool containsBook(int bookId) const;
    void setBookIds(const QVector<int> &ids);
    friend QDataStream &operator<<(QDataStream &out, const Shelf &shelf);
    friend QDataStream &operator>>(QDataStream &in, Shelf &shelf);
};

#endif // SHELF_H
