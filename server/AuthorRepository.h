#ifndef AUTHORREPOSITORY_H
#define AUTHORREPOSITORY_H
#include "../common/Author.h"
#include <QString>
#include <QVector>
class AuthorRepository {
public:
    AuthorRepository();
    int insertAuthor(const Author &author);
    Author* loadAuthorById(int authorId);
    bool updateAuthorName(int authorId, const QString &newName);
    bool deleteAuthor(int authorId);
    QVector<Author> getAllAuthors();
    bool authorExists(const QString &name);
    int getOrCreateAuthor(const QString &authorName);
};

#endif // AUTHORREPOSITORY_H
