#include "SavedBookController.h"

SavedBookController::SavedBookController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager) {
    connect(networkManager, &NetworkManager::responseReceived, this, &SavedBookController::onResponseReceived);
}
void SavedBookController::saveBook(int bookId) {
    if (bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if (!networkManager->isConnected()) { emit bookSaveFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->saveBook(bookId);
}
void SavedBookController::unsaveBook(int bookId) {
    if (bookId <= 0) { emit validationError("شناسه کتاب نامعتبر است"); return; }
    if (!networkManager->isConnected()) { emit bookUnsaveFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->unsaveBook(bookId);
}
void SavedBookController::refreshSavedBooks() {
    if (!networkManager->isConnected()) { emit savedBooksLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getSavedBooks();
}
void SavedBookController::onResponseReceived(RequestType type, const Response &response) {
    switch (type) {
    case RequestType::SaveBook:
        if (response.isSuccess()) emit bookSaved(response.getMessage());
        else emit bookSaveFailed(response.getMessage());
        break;
    case RequestType::UnsaveBook:
        if (response.isSuccess()) emit bookUnsaved(response.getMessage());
        else emit bookUnsaveFailed(response.getMessage());
        break;
    case RequestType::GetSavedBooks:
        if (response.isSuccess()) emit savedBooksLoaded(response.getData().value("bookIds").toList());
        else emit savedBooksLoadFailed(response.getMessage());
        break;
    default:
        break;
    }
}