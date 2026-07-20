#include "CartManager.h"
#include "CartRepository.h"
#include "BookRepository.h"
#include "TimedDiscountRepository.h"
#include "PriceCalculator.h"
#include "UserRepository.h"
#include "../common/Cart.h"
#include "../common/Book.h"
#include "../common/TimedDiscount.h"
#include "../common/normaluser.h"
#include <memory>

CartManager::CartManager() {}
Response CartManager::addBookToCart(int userId, int bookId) {
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if (!book) {
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    if (!book->isAvailableForPurchase()) {
        return Response(ResponseStatus::Error, "این کتاب در حال حاضر برای خرید موجود نیست");
    }
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if (user && user->hasPurchased(bookId)) {
        return Response(ResponseStatus::ValidationFailed, "شما قبلاً این کتاب را خریداری کرده اید");
    }
    CartRepository cartRepo;
    int cartId = cartRepo.getOrCreateCartId(userId);
    if (cartId == -1) {
        return Response(ResponseStatus::Error, "خطا در دسترسی به سبد خرید");
    }
    if (cartRepo.isBookInCart(cartId, bookId)) {
        return Response(ResponseStatus::Error, "این کتاب از قبل در سبد خرید شما موجود است");
    }
    if (!cartRepo.addItemToCart(cartId, bookId)) {
        return Response(ResponseStatus::Error, "خطا در افزودن کتاب به سبد خرید");
    }
    return Response(ResponseStatus::Success, "کتاب به سبد خرید اضافه شد");
}
Response CartManager::removeBookFromCart(int userId, int bookId) {
    CartRepository cartRepo;
    int cartId = cartRepo.getOrCreateCartId(userId);
    if (cartId == -1) {
        return Response(ResponseStatus::Error, "خطا در دسترسی به سبد خرید");
    }
    if (!cartRepo.isBookInCart(cartId, bookId)) {
        return Response(ResponseStatus::NotFound, "این کتاب در سبد خرید شما موجود نیست");
    }
    if (!cartRepo.removeItemFromCart(cartId, bookId)) {
        return Response(ResponseStatus::Error, "خطا در حذف کتاب از سبد خرید");
    }
    return Response(ResponseStatus::Success, "کتاب از سبد خرید حذف شد");
}
Response CartManager::getCart(int userId) {
    CartRepository cartRepo;
    std::unique_ptr<Cart> cart(cartRepo.loadCartByUserId(userId));
    if (!cart) {
        return Response(ResponseStatus::Error, "خطا در ارتباط با پایگاه داده هنگام بارگذاری سبد خرید");
    }
    TimedDiscountRepository timedDiscountRepo;
    QVector<CartItem> items = cart->getItems();
    QVariantList itemList;
    double totalPrice = 0;
    double totalFinalPrice = 0;
    for (const CartItem &item : qAsConst(items)) {
        std::unique_ptr<TimedDiscount> activeDiscount( timedDiscountRepo.getActiveDiscountForBook(item.getBookId()) );
        double timedPercent = activeDiscount ? activeDiscount->getDiscountPercent() : 0;
        double effectivePercent = PriceCalculator::calculateEffectivePercent( item.getDiscountPercent(), timedPercent );
        double finalPrice = PriceCalculator::calculateFinalPrice( item.getPrice(), effectivePercent, item.getDiscountAmount() );
        QVariantMap itemData;
        itemData["bookId"] = item.getBookId();
        itemData["price"] = item.getPrice();
        itemData["discountPercent"] = effectivePercent;
        itemData["finalPrice"] = finalPrice;
        itemList.append(itemData);
        totalPrice += item.getPrice();
        totalFinalPrice += finalPrice;
    }
    QVariantMap data;
    data["cartId"] = cart->getCartId();
    data["items"] = itemList;
    data["itemCount"] = cart->getItemCount();
    data["totalPrice"] = totalPrice;
    data["totalDiscount"] = totalPrice - totalFinalPrice;
    data["finalPrice"] = totalFinalPrice;
    return Response(ResponseStatus::Success, "سبد خرید بازیابی شد", data);
}
Response CartManager::clearCart(int userId) {
    CartRepository cartRepo;
    int cartId = cartRepo.getOrCreateCartId(userId);
    if (cartId == -1) {
        return Response(ResponseStatus::Error, "خطا در دسترسی به سبد خرید");
    }
    if (!cartRepo.clearCart(cartId)) {
        return Response(ResponseStatus::Error, "خطا در خالی کردن سبد خرید");
    }
    return Response(ResponseStatus::Success, "سبد خرید خالی شد");
}