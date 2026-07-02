#include "Author.h"
Author::Author():authorId(-1){}
Author::Author(int authorId, const QString &authorName):authorId(authorId), authorName(authorName){}
int Author::getAuthorId() const { return authorId; }
QString Author::getAuthorName() const { return authorName; }
void Author::setAuthorId(int id) { authorId = id; }
bool Author::setAuthorName(const QString &name){
    if(name.trimmed().isEmpty() || name.length() > 60)
        return false;
    authorName = name.trimmed();
    return true;
}
QDataStream &operator<<(QDataStream &out, const Author &author){
    out << author.authorId << author.authorName;
    return out;
}
QDataStream &operator>>(QDataStream &in, Author &author){
    in >> author.authorId >> author.authorName;
    return in;
}