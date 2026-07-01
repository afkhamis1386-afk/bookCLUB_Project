#include "Genre.h"
Genre::Genre():genreId(-1){}
Genre::Genre(int genreId, const QString &genreTitle):genreId(genreId), genreTitle(genreTitle){}
int Genre::getGenreId() const { return genreId; }
QString Genre::getGenreTitle() const { return genreTitle; }
void Genre::setGenreId(int id){ genreId = id; }
bool Genre::setGenreTitle(const QString &title){
    if(title.trimmed().isEmpty() || title.length() > 30)
    return false;
    genreTitle = title.trimmed();
    return true;
}
QDataStream &operator<<(QDataStream &out, const Genre &genre){
    out << genre.genreId << genre.genreTitle;
    return out;
}
QDataStream &operator>>(QDataStream &in, Genre &genre){
    in >> genre.genreId >> genre.genreTitle;
    return in;
}