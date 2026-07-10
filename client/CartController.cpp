#include "CartController.h"

CartController::CartController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager)
{
    connect(networkManager, &NetworkManager::responseReceived,
            this, &CartController::onResponseReceived);
}
void CartController::addBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit addToCartFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->addToCart(bookId);
}
void CartController::removeBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit removeFromCartFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->removeFromCart(bookId);
}
void CartController::refreshCart() {
    if (!networkManager->isConnected()) {
        emit cartLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getCart();
}
void CartController::checkout() {
    if (!networkManager->isConnected()) {
        emit checkoutFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->checkout();
}
void CartController::onResponseReceived(RequestType type, const Response &response) {
    switch (type) {
    case RequestType::AddToCart:
        if (response.isSuccess())
            emit addToCartSucceeded(response.getMessage());
        else
            emit addToCartFailed(response.getMessage());
        break;
    case RequestType::RemoveFromCart:
        if (response.isSuccess())
            emit removeFromCartSucceeded(response.getMessage());
        else
            emit removeFromCartFailed(response.getMessage());
        break;
    case RequestType::GetCart:
        if (response.isSuccess())
            emit cartLoaded(response.getData());
        else
            emit cartLoadFailed(response.getMessage());
        break;
    case RequestType::Checkout:
        if (response.isSuccess())
            emit checkoutSucceeded(response.getData());
        else
            emit checkoutFailed(response.getMessage());
        break;

    default:
        break;
    }
}