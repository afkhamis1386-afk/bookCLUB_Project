#ifndef CARTMANAGER_H
#define CARTMANAGER_H

#include "../common/Response.h"
class CartManager {
public:
    CartManager();
    Response addBookToCart(int userId, int bookId);
    Response removeBookFromCart(int userId, int bookId);
    Response getCart(int userId);
    Response clearCart(int userId);
};

#endif // CARTMANAGER_H
