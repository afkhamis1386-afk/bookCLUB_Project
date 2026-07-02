#ifndef AUTHOR_H
#define AUTHOR_H
#include <QString>
#include <QDataStream>
class Author {
private:
    int authorId;
    QString authorName;
public:
    Author();
    Author(int authorId, const QString &authorName);
    int getAuthorId() const;
    QString getAuthorName() const;
    void setAuthorId(int id);
    bool setAuthorName(const QString &name);
    friend QDataStream &operator<<(QDataStream &out, const Author &author);
    friend QDataStream &operator>>(QDataStream &in, Author &author);
};

#endif // AUTHOR_H
