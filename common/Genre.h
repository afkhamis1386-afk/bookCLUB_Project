#ifndef GENRE_H
#define GENRE_H
#include <QString>
#include <QDataStream>
class Genre {
private:
    int genreId;
    QString genreTitle;
public:
    Genre();
    Genre(int genreId, const QString &genreTitle);
    int getGenreId() const;
    QString getGenreTitle() const;
    void setGenreId(int id);
    bool setGenreTitle(const QString &title);
    friend QDataStream &operator<<(QDataStream &out, const Genre &genre);
    friend QDataStream &operator>>(QDataStream &in, Genre &genre);
};
#endif // GENRE_H
