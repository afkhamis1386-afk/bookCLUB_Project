#ifndef CART_H
#define CART_H
#include <QVector>
#include <QDataStream>
#include <QDateTime>
class CartItem {
private:
    int bookId;
    double price;
    double discountPercent;
    double discountAmount;
public:
    CartItem();
    CartItem(int bookId, double price, double discountPercent = 0, double discountAmount = 0);
    int getBookId() const;
    double getPrice() const;
    double getDiscountPercent() const;
    double getDiscountAmount() const;
    double getFinalPrice() const;
    friend QDataStream& operator<<(QDataStream& out, const CartItem& item);
    friend QDataStream& operator>>(QDataStream& in, CartItem& item);
};
class Cart {
private:
    int cartId;
    int userId;
    QVector<CartItem> items;
    QDateTime lastUpdated;
public:
    Cart();
    Cart(int userId);
    int getUserId() const;
    bool addBook(int bookId, double price, double discountPercent = 0, double discountAmount = 0);
    bool removeBook(int bookId);
    bool containsBook(int bookId) const;
    int getCartId() const;
    void setCartId(int id);
    QVector<CartItem> getItems() const;
    int getItemCount() const;
    double getTotalPrice() const;
    double getTotalDiscount() const;
    double getFinalPrice() const;
    void clearCart();
    QDateTime getLastUpdated() const;
    friend QDataStream& operator<<(QDataStream& out, const Cart& cart);
    friend QDataStream& operator>>(QDataStream& in, Cart& cart);
};
#endif // CART_H
