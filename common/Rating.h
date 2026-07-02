#ifndef RATING_H
#define RATING_H
#include <QDataStream>
class Rating {
private:
    int ratingId;
    int userId;
    int bookId;
    int ratingValue;
public:
    Rating();
    Rating(int userId, int bookId, int ratingValue);
    Rating(int ratingId, int userId, int bookId, int ratingValue);
    int getRatingId() const;
    int getUserId() const;
    int getBookId() const;
    int getRatingValue() const;
    void setRatingId(int id);
    bool setRatingValue(int value);
    friend QDataStream &operator<<(QDataStream &out, const Rating &rating);
    friend QDataStream &operator>>(QDataStream &in, Rating &rating);
};

#endif // RATING_H
