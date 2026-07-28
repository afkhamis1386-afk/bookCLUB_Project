#include "Cart.h"
CartItem::CartItem() :
    bookId(-1), price(0), discountPercent(0), discountAmount(0) {}
CartItem::CartItem(int bookId, double price, double discountPercent, double discountAmount) : bookId(bookId), price(price), discountPercent(discountPercent), discountAmount(discountAmount) {}
int CartItem::getBookId() const { return bookId; }
double CartItem::getPrice() const { return price; }
double CartItem::getDiscountPercent() const { return discountPercent; }
double CartItem::getDiscountAmount() const { return discountAmount; }
double CartItem::getFinalPrice() const {
    double priceAfterPercent = price - (price * (discountPercent / 100.0));
    double finalPrice = priceAfterPercent - discountAmount;
    if(finalPrice < 0)
        finalPrice = 0;
    return finalPrice;
}
QDataStream& operator<<(QDataStream& out, const CartItem& item) {
    out << item.bookId
        << item.price
        << item.discountPercent
        << item.discountAmount;
    return out;
}
QDataStream& operator>>(QDataStream& in, CartItem& item) {
    in >> item.bookId >> item.price >> item.discountPercent >> item.discountAmount;
    return in;
}
Cart::Cart() : cartId(-1), userId(-1) {}
Cart::Cart(int userId) : cartId(-1), userId(userId) { lastUpdated = QDateTime::currentDateTime(); }
int Cart::getUserId() const { return userId; }
bool Cart::addBook(int bookId, double price, double discountPercent, double discountAmount) {
    for(const CartItem& item : qAsConst(items))
        if(item.getBookId() == bookId)
            return false;
    items.append(CartItem(bookId, price, discountPercent, discountAmount));
    lastUpdated = QDateTime::currentDateTime();
    return true;
}
bool Cart::removeBook(int bookId) {
    for(int i=0;i<items.size();i++) {
        if(items[i].getBookId() == bookId) {
            items.remove(i);
            lastUpdated = QDateTime::currentDateTime();
            return true;
        }
    }
    return false;
}
bool Cart::containsBook(int bookId) const {
    for(const CartItem& item : items)
        if(item.getBookId() == bookId)
            return true;
    return false;
}
int Cart::getCartId() const { return cartId; }
void Cart::setCartId(int id) { cartId = id;}
QVector<CartItem> Cart::getItems() const { return items; }
int Cart::getItemCount() const {
    return items.size();
}
double Cart::getTotalPrice() const {
    double sum = 0;
    for(const CartItem& item : items)
        sum += item.getPrice();
    return sum;
}
double Cart::getTotalDiscount() const {
    double discount = 0;
    for(const CartItem& item : items)
        discount += (item.getPrice() - item.getFinalPrice());
    return discount;
}
double Cart::getFinalPrice() const {
    double sum = 0;
    for(const CartItem& item : items)
        sum += item.getFinalPrice();
    return sum;
}
void Cart::clearCart(){
    items.clear();
    lastUpdated = QDateTime::currentDateTime();
}
QDateTime Cart::getLastUpdated() const { return lastUpdated; }
QDataStream& operator<<(QDataStream& out, const Cart& cart) {
    out << cart.cartId << cart.userId << cart.items << cart.lastUpdated;
    return out;
}
QDataStream& operator>>(QDataStream& in, Cart& cart) {
    in >> cart.cartId >> cart.userId >> cart.items >> cart.lastUpdated;
    return in;
}