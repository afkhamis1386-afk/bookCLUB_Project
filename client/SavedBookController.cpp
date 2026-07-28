#include "SavedBookController.h"

SavedBookController::SavedBookController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager) {
    connect(networkManager, &NetworkManager::responseReceived,
            this, &SavedBookController::onResponseReceived);
}

void SavedBookController::saveBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit bookSaveFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->saveBook(bookId);
}

void SavedBookController::unsaveBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit bookUnsaveFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->unsaveBook(bookId);
}

void SavedBookController::refreshSavedBooks() {
    if (!networkManager->isConnected()) {
        emit savedBooksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getSavedBooks();
}

void SavedBookController::addFavoriteBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("ابتدا یک کتاب ذخیره شده را انتخاب کنید");
        return;
    }
    if (!networkManager->isConnected()) {
        emit favoriteBookAddFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->addFavoriteBook(bookId);
}

void SavedBookController::removeFavoriteBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("ابتدا یک کتاب را از لیست علاقه مندی انتخاب کنید");
        return;
    }
    if (!networkManager->isConnected()) {
        emit favoriteBookRemoveFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->removeFavoriteBook(bookId);
}

void SavedBookController::refreshFavoriteBooks() {
    if (!networkManager->isConnected()) {
        emit favoriteBooksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getFavoriteBooks();
}

void SavedBookController::reorderFavoriteBooks(const QVariantList &bookIds) {
    if (!networkManager->isConnected()) {
        emit favoriteBooksReorderFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->reorderFavoriteBooks(bookIds);
}

void SavedBookController::onResponseReceived(RequestType type, const Response &response) {
    switch (type) {
    case RequestType::SaveBook:
        if (response.isSuccess())
            emit bookSaved(response.getMessage());
        else
            emit bookSaveFailed(response.getMessage());
        break;
    case RequestType::UnsaveBook:
        if (response.isSuccess())
            emit bookUnsaved(response.getMessage());
        else
            emit bookUnsaveFailed(response.getMessage());
        break;
    case RequestType::GetSavedBooks:
        if (response.isSuccess())
            emit savedBooksLoaded(response.getData().value("books").toList());
        else
            emit savedBooksLoadFailed(response.getMessage());
        break;
    case RequestType::AddFavoriteBook:
        if (response.isSuccess())
            emit favoriteBookAdded(response.getMessage());
        else
            emit favoriteBookAddFailed(response.getMessage());
        break;
    case RequestType::RemoveFavoriteBook:
        if (response.isSuccess())
            emit favoriteBookRemoved(response.getMessage());
        else
            emit favoriteBookRemoveFailed(response.getMessage());
        break;
    case RequestType::GetFavoriteBooks:
        if (response.isSuccess())
            emit favoriteBooksLoaded(response.getData().value("books").toList());
        else
            emit favoriteBooksLoadFailed(response.getMessage());
        break;
    case RequestType::ReorderFavoriteBooks:
        if (response.isSuccess())
            emit favoriteBooksReordered(response.getMessage());
        else
            emit favoriteBooksReorderFailed(response.getMessage());
        break;
    default:
        break;
    }
}
