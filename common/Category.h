#ifndef CATEGORY_H
#define CATEGORY_H
#include <QString>
#include <QDataStream>
class Category {
private:
    int categoryId;
    QString categoryTitle;
public:
    Category();
    Category(int categoryId, const QString &categoryTitle);
    int getCategoryId() const;
    QString getCategoryTitle() const;
    void setCategoryId(int id);
    bool setCategoryTitle(const QString &title);
    friend QDataStream &operator<<(QDataStream &out, const Category &category);
    friend QDataStream &operator>>(QDataStream &in, Category &category);
};

#endif // CATEGORY_H
