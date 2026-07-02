#include "Category.h"
Category::Category():categoryId(-1){}
Category::Category(int categoryId, const QString &categoryTitle):categoryId(categoryId), categoryTitle(categoryTitle){}
int Category::getCategoryId() const { return categoryId; }
QString Category::getCategoryTitle() const { return categoryTitle; }
void Category::setCategoryId(int id) { categoryId = id; }
bool Category::setCategoryTitle(const QString &title){
    if(title.trimmed().isEmpty() || title.length() > 50)
        return false;
    categoryTitle = title.trimmed();
    return true;
}
QDataStream &operator<<(QDataStream &out, const Category &category){
    out << category.categoryId << category.categoryTitle;
    return out;
}
QDataStream &operator>>(QDataStream &in, Category &category){
    in >> category.categoryId >> category.categoryTitle;
    return in;
}
