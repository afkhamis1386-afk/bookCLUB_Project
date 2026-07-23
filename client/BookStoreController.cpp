#include "BookStoreController.h"

BookStoreController::BookStoreController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager), lastBookListRequest(RequestType::GetBooks)
{
    connect(networkManager, &NetworkManager::responseReceived,
            this, &BookStoreController::onResponseReceived);
}
void BookStoreController::loadAllBooks() {
    if (!networkManager->isConnected()) {
        emit booksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::GetBooks;
    networkManager->getBooks();
}
void BookStoreController::search(const QString &query) {
    if (query.trimmed().isEmpty()) {
        emit validationError("لطفاً عبارت جستجو را وارد کنید");
        return;
    }
    if (!networkManager->isConnected()) {
        emit searchFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::SearchBooks;
    networkManager->searchBooks(query.trimmed());
}
void BookStoreController::loadBookDetails(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit bookDetailsFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getBookDetails(bookId);
}
void BookStoreController::loadBooksByGenre(int genreId) {
    if (genreId <= 0) {
        emit validationError("شناسه ژانر نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit booksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::GetBooksByGenre;
    networkManager->getBooksByGenre(genreId);
}

void BookStoreController::loadBooksByCategory(int categoryId) {
    if (categoryId <= 0) {
        emit validationError("شناسه دسته بندی نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit booksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::GetBooksByCategory;
    networkManager->getBooksByCategory(categoryId);
}
void BookStoreController::loadNewestBooks(int limit) {
    if (limit <= 0) {
        emit validationError("تعداد نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit booksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::GetNewestBooks;
    networkManager->getNewestBooks(limit);
}
void BookStoreController::loadFreeBooks() {
    if (!networkManager->isConnected()) {
        emit booksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::GetFreeBooks;
    networkManager->getFreeBooks();
}
void BookStoreController::loadBestSellers(int limit) {
    if (!networkManager->isConnected()) {
        emit booksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::GetBestSellers;
    networkManager->getBestSellers(limit);
}
void BookStoreController::loadPopularBooks(int limit) {
    if (!networkManager->isConnected()) {
        emit booksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    lastBookListRequest = RequestType::GetPopularBooks;
    networkManager->getPopularBooks(limit);
}
void BookStoreController::loadRecommendedBooks() {
    if (!networkManager->isConnected()) {
        emit recommendedBooksLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getRecommendedBooks();
}
void BookStoreController::claimFreeBook(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit freeBookClaimFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->claimFreeBook(bookId);
}
void BookStoreController::onResponseReceived(RequestType type, const Response &response) {
    switch (type) {
    case RequestType::GetBooks:
        if (response.isSuccess())
            emit booksLoaded(response.getData().value("bookIds").toList());
        else
            emit booksLoadFailed(response.getMessage());
        break;
    case RequestType::SearchBooks:
        if (response.isSuccess())
            emit searchResultsReceived(response.getData().value("bookIds").toList());
        else
            emit searchFailed(response.getMessage());
        break;
    case RequestType::GetBookDetails:
        if (response.isSuccess())
            emit bookDetailsReceived(response.getData());
        else
            emit bookDetailsFailed(response.getMessage());
        break;
    case RequestType::GetBookCoverImage: {
        int bookId = -1;
        if (!pendingCoverImageRequests.isEmpty()) {
            bookId = pendingCoverImageRequests.takeFirst();
        }
        if (response.isSuccess()) {
            int actualId = response.getData().contains("bookId")
            ? response.getData().value("bookId").toInt()
            : bookId;
            emit coverImageLoaded(actualId, response.getData().value("imageData").toByteArray());
        }
        else
            emit coverImageLoadFailed(bookId, response.getMessage());
        break;
    }
    case RequestType::GetBooksByGenre:
    case RequestType::GetBooksByCategory:
    case RequestType::GetNewestBooks:
    case RequestType::GetFreeBooks:
    case RequestType::GetBestSellers:
    case RequestType::GetPopularBooks:
        if (response.isSuccess())
            emit booksLoaded(response.getData().value("bookIds").toList());
        else
            emit booksLoadFailed(response.getMessage());
        break;
    case RequestType::GetRecommendedBooks:
        if (response.isSuccess())
            emit recommendedBooksLoaded(response.getData().value("bookIds").toList());
        else
            emit recommendedBooksLoadFailed(response.getMessage());
        break;
    case RequestType::ClaimFreeBook:
        if (response.isSuccess())
            emit freeBookClaimed(response.getMessage());
        else
            emit freeBookClaimFailed(response.getMessage());
        break;
    default:
        break;
    }
}
void BookStoreController::loadCoverImage(int bookId) {
    if (bookId <= 0) {
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()) {
        emit coverImageLoadFailed(bookId, "اتصال به سرور برقرار نیست");
        return;
    }
    pendingCoverImageRequests.append(bookId);
    networkManager->getBookCoverImage(bookId);
}