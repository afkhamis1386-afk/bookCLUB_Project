#ifndef CARTREPOSITORY_H
#define CARTREPOSITORY_H
#include "../common/Cart.h"
class CartRepository {
public:
    CartRepository();
    int getOrCreateCartId(int userId);
    Cart* loadCartByUserId(int userId);
    bool addItemToCart(int cartId, int bookId);
    bool removeItemFromCart(int cartId, int bookId);
    bool isBookInCart(int cartId, int bookId);
    bool clearCart(int cartId);
    int getItemCount(int cartId);
};

#endif // CARTREPOSITORY_H
