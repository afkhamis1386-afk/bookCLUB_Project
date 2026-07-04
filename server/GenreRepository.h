#ifndef GENREREPOSITORY_H
#define GENREREPOSITORY_H
#include "../common/Genre.h"
#include <QString>
#include <QVector>
class GenreRepository {
public:
    GenreRepository();
    int insertGenre(const Genre &genre);
    Genre* loadGenreById(int genreId);
    bool updateGenreTitle(int genreId, const QString &newTitle);
    bool deleteGenre(int genreId);
    QVector<Genre> getAllGenres();
    bool genreExists(const QString &title);
};

#endif // GENREREPOSITORY_H
