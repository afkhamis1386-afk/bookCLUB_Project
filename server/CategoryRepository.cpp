#include "CategoryRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
CategoryRepository::CategoryRepository(){}
int CategoryRepository::insertCategory(const Category &category){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Categories (CategoryTitle) OUTPUT INSERTED.CategoryID VALUES (:title)");
    query.bindValue(":title", category.getCategoryTitle());
    if(!query.exec() || !query.next()){
        qWarning() << "خطا در ثبت دسته بندی جدید:" << query.lastError().text();
        return -1;
    }
    return query.value(0).toInt();
}
Category* CategoryRepository::loadCategoryById(int categoryId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT CategoryID, CategoryTitle FROM Categories WHERE CategoryID = :categoryId");
    query.bindValue(":categoryId", categoryId);
    if(!query.exec() || !query.next()){
        qWarning() << "دسته بندی یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    return new Category(query.value(0).toInt(), query.value(1).toString());
}
bool CategoryRepository::updateCategoryTitle(int categoryId, const QString &newTitle){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Categories SET CategoryTitle = :title WHERE CategoryID = :categoryId");
    query.bindValue(":title", newTitle);
    query.bindValue(":categoryId", categoryId);
    if(!query.exec()){
        qWarning() << "خطا در ویرایش دسته بندی:" << query.lastError().text();
        return false;
    }
    return true;
}
bool CategoryRepository::deleteCategory(int categoryId){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Categories WHERE CategoryID = :categoryId");
    query.bindValue(":categoryId", categoryId);
    if(!query.exec()){
        qWarning() << "خطا در حذف دسته بندی (احتمالاً کتابی به این دسته وابسته است):" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<Category> CategoryRepository::getAllCategories(){
    QVector<Category> categories;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT CategoryID, CategoryTitle FROM Categories ORDER BY CategoryTitle");
    if(query.exec()){
        while(query.next())
            categories.append(Category(query.value(0).toInt(), query.value(1).toString()));
    }
    return categories;
}
int CategoryRepository::getOrCreateCategory(const QString &categoryTitle){
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery selectQuery(db);
    selectQuery.prepare("SELECT CategoryID FROM Categories WHERE CategoryTitle = :title");
    selectQuery.bindValue(":title", categoryTitle);
    if(selectQuery.exec() && selectQuery.next()){
        return selectQuery.value(0).toInt();
    }
    Category newCategory;
    if(!newCategory.setCategoryTitle(categoryTitle))
        return -1;
    return insertCategory(newCategory);
}

