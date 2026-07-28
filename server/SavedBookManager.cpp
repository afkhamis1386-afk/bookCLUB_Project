#include "SavedBookManager.h"
#include "SavedBookRepository.h"
#include "BookRepository.h"
#include "../common/Book.h"
#include <QSet>
#include <memory>

namespace {
bool convertUniquePositiveIds(const QVariantList &values, QVector<int> &ids) {
    ids.clear();
    QSet<int> seen;
    for (const QVariant &value : values) {
        bool ok = false;
        const int id = value.toInt(&ok);
        if (!ok || id <= 0 || seen.contains(id))
            return false;
        seen.insert(id);
        ids.append(id);
    }
    return true;
}
bool containSameIds(const QVector<int> &left, const QVector<int> &right) {
    if (left.size() != right.size())
        return false;
    QSet<int> leftSet;
    QSet<int> rightSet;
    for (int id : left)
        leftSet.insert(id);
    for (int id : right)
        rightSet.insert(id);
    return leftSet == rightSet && leftSet.size() == left.size();
}
Response buildBookListResponse(const QVector<int> &ids, const QString &message) {
    BookRepository bookRepo;
    QVariantList books;
    for (int id : ids) {
        std::unique_ptr<Book> book(bookRepo.loadBookById(id));
        if (!book) {
            return Response(ResponseStatus::Error, "اطلاعات کتاب ذخیره شده با جدول Books تطبیق ندارد");
        }
        QVariantMap bookData;
        bookData["bookId"] = id;
        bookData["bookName"] = book->getBookName();
        books.append(bookData);
    }
    QVariantMap data;
    data["books"] = books;
    data["bookIds"] = books;
    return Response(ResponseStatus::Success, message, data);
}
}
SavedBookManager::SavedBookManager() {}

Response SavedBookManager::saveBook(int userId, int bookId) {
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if (!book)
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    SavedBookRepository savedRepo;
    if (savedRepo.isBookSaved(userId, bookId))
        return Response(ResponseStatus::Error, "این کتاب قبلاً ذخیره شده است");
    if (!savedRepo.saveBook(userId, bookId))
        return Response(ResponseStatus::Error, "خطا در ذخیره کتاب");
    return Response(ResponseStatus::Success, "کتاب به کتاب های ذخیره شده اضافه شد");
}
Response SavedBookManager::unsaveBook(int userId, int bookId) {
    SavedBookRepository savedRepo;
    if (!savedRepo.isBookSaved(userId, bookId))
        return Response(ResponseStatus::Error, "این کتاب در کتاب های ذخیره شده نیست");
    if (!savedRepo.unsaveBook(userId, bookId))
        return Response(ResponseStatus::Error, "خطا در حذف کتاب از کتاب های ذخیره شده");
    return Response(ResponseStatus::Success, "کتاب از کتاب های ذخیره شده و لیست علاقه مندی حذف شد");
}
Response SavedBookManager::getSavedBooks(int userId) {
    SavedBookRepository savedRepo;
    return buildBookListResponse(savedRepo.getSavedBookIds(userId), "کتاب های ذخیره شده بازیابی شدند");
}
Response SavedBookManager::addFavoriteBook(int userId, int bookId) {
    if (userId <= 0 || bookId <= 0)
        return Response(ResponseStatus::ValidationFailed, "شناسه کاربر یا کتاب نامعتبر است");
    SavedBookRepository savedRepo;
    if (!savedRepo.isBookSaved(userId, bookId)) {
        return Response(ResponseStatus::ValidationFailed, "فقط کتاب های ذخیره شده را می توان به لیست علاقه مندی افزود");
    }
    if (savedRepo.isFavoriteBook(userId, bookId))
        return Response(ResponseStatus::Error, "این کتاب قبلاً در لیست علاقه مندی ها وجود دارد");
    if (!savedRepo.addFavoriteBook(userId, bookId))
        return Response(ResponseStatus::Error, "خطا در افزودن کتاب به لیست علاقه مندی ها");
    return Response(ResponseStatus::Success, "کتاب به لیست علاقه مندی ها اضافه شد");
}
Response SavedBookManager::removeFavoriteBook(int userId, int bookId) {
    if (userId <= 0 || bookId <= 0)
        return Response(ResponseStatus::ValidationFailed, "شناسه کاربر یا کتاب نامعتبر است");
    SavedBookRepository savedRepo;
    if (!savedRepo.isFavoriteBook(userId, bookId))
        return Response(ResponseStatus::Error, "این کتاب در لیست علاقه مندی ها نیست");
    if (!savedRepo.removeFavoriteBook(userId, bookId))
        return Response(ResponseStatus::Error, "خطا در حذف کتاب از لیست علاقه مندی ها");
    return Response(ResponseStatus::Success, "کتاب از لیست علاقه مندی ها حذف شد");
}
Response SavedBookManager::getFavoriteBooks(int userId) {
    SavedBookRepository savedRepo;
    return buildBookListResponse(savedRepo.getFavoriteBookIds(userId), "لیست علاقه مندی ها بازیابی شد");
}
Response SavedBookManager::reorderFavoriteBooks(int userId, const QVariantList &bookIds) {
    QVector<int> requestedIds;
    if (!convertUniquePositiveIds(bookIds, requestedIds))
        return Response(ResponseStatus::ValidationFailed, "ترتیب لیست علاقه مندی ها نامعتبر است");
    SavedBookRepository savedRepo;
    const QVector<int> currentIds = savedRepo.getFavoriteBookIds(userId);
    if (!containSameIds(currentIds, requestedIds)) {
        return Response(ResponseStatus::ValidationFailed, "ترتیب ارسالی باید شامل تمام کتاب های لیست علاقه مندی باشد");
    }
    if (!savedRepo.reorderFavoriteBooks(userId, requestedIds))
        return Response(ResponseStatus::Error, "خطا در ذخیره ترتیب لیست علاقه مندی ها");
    return Response(ResponseStatus::Success, "ترتیب لیست علاقه مندی ها ذخیره شد");
}