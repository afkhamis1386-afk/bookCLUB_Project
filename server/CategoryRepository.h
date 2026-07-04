#ifndef CATEGORYREPOSITORY_H
#define CATEGORYREPOSITORY_H
#include "../common/Category.h"
#include <QString>
#include <QVector>
class CategoryRepository {
public:
    CategoryRepository();
    int insertCategory(const Category &category);
    Category* loadCategoryById(int categoryId);
    bool updateCategoryTitle(int categoryId, const QString &newTitle);
    bool deleteCategory(int categoryId);
    QVector<Category> getAllCategories();
};

#endif // CATEGORYREPOSITORY_H
