#include "CartRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
CartRepository::CartRepository() {}
int CartRepository::getOrCreateCartId(int userId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery selectQuery(db);
    selectQuery.prepare("SELECT CartID FROM ShoppingCarts WHERE UserID = :userId");
    selectQuery.bindValue(":userId", userId);
    if (selectQuery.exec() && selectQuery.next()) {
        return selectQuery.value(0).toInt();
    }
    QSqlQuery insertQuery(db);
    insertQuery.prepare(
        "INSERT INTO ShoppingCarts (UserID) OUTPUT INSERTED.CartID VALUES (:userId)" );
    insertQuery.bindValue(":userId", userId);
    if (!insertQuery.exec() || !insertQuery.next()) {
        qWarning() << "خطا در ساخت سبد خرید جدید:" << insertQuery.lastError().text();
        return -1;
    }
    return insertQuery.value(0).toInt();
}
Cart* CartRepository::loadCartByUserId(int userId) {
    int cartId = getOrCreateCartId(userId);
    if (cartId == -1)
        return nullptr;
    Cart *cart = new Cart(userId);
    cart->setCartId(cartId);
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT b.BookID, b.BookPrice, b.DiscountPercent, b.DiscountAmount "
        "FROM CartItems ci "
        "JOIN Books b ON ci.BookID = b.BookID "
        "WHERE ci.CartID = :cartId" );
    query.bindValue(":cartId", cartId);
    if (query.exec()) {
        while (query.next()) {
            cart->addBook(
                query.value(0).toInt(), query.value(1).toDouble(),
                query.value(2).toDouble(), query.value(3).toDouble() );
        }
    }
    return cart;
}
bool CartRepository::addItemToCart(int cartId, int bookId) {
    if (isBookInCart(cartId, bookId))
        return false;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("INSERT INTO CartItems (CartID, BookID) VALUES (:cartId, :bookId)");
    query.bindValue(":cartId", cartId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در افزودن کتاب به سبد خرید:" << query.lastError().text();
        return false;
    }
    return true;
}
bool CartRepository::removeItemFromCart(int cartId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM CartItems WHERE CartID = :cartId AND BookID = :bookId");
    query.bindValue(":cartId", cartId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در حذف کتاب از سبد خرید:" << query.lastError().text();
        return false;
    }
    return true;
}
bool CartRepository::isBookInCart(int cartId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM CartItems WHERE CartID = :cartId AND BookID = :bookId");
    query.bindValue(":cartId", cartId);
    query.bindValue(":bookId", bookId);
    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}
bool CartRepository::clearCart(int cartId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM CartItems WHERE CartID = :cartId");
    query.bindValue(":cartId", cartId);
    if (!query.exec()) {
        qWarning() << "خطا در خالی کردن سبد خرید:" << query.lastError().text();
        return false;
    }
    return true;
}
int CartRepository::getItemCount(int cartId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM CartItems WHERE CartID = :cartId");
    query.bindValue(":cartId", cartId);
    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}