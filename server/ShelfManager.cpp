#include "ShelfManager.h"
#include "ShelfRepository.h"
#include "BookRepository.h"
#include "DatabaseManager.h"
#include "UserRepository.h"
#include "../common/Shelf.h"
#include "../common/Book.h"
#include "../common/normaluser.h"
#include <QSet>
#include <QSqlDatabase>
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
}
ShelfManager::ShelfManager() {}
Response ShelfManager::createShelf(int userId, const QString &shelfName) {
    if (userId <= 0 || shelfName.trimmed().isEmpty() || shelfName.trimmed().length() > 100) {
        return Response(ResponseStatus::ValidationFailed, "نام قفسه نمی تواند خالی باشد و باید حداکثر ۱۰۰ کاراکتر باشد");
    }
    ShelfRepository shelfRepo;
    if (shelfRepo.shelfNameExistsForUser(userId, shelfName.trimmed())) {
        return Response(ResponseStatus::Error, "قفسه ای با این نام از قبل موجود است");
    }
    Shelf newShelf(userId, shelfName.trimmed());
    const int newShelfId = shelfRepo.insertShelf(newShelf);
    if (newShelfId == -1)
        return Response(ResponseStatus::Error, "خطا در ساخت قفسه");
    QVariantMap data;
    data["shelfId"] = newShelfId;
    return Response(ResponseStatus::Success, "قفسه با موفقیت ساخته شد", data);
}
Response ShelfManager::renameShelf(int userId, int shelfId, const QString &newName) {
    if (userId <= 0 || shelfId <= 0 || newName.trimmed().isEmpty() || newName.trimmed().length() > 100)
        return Response(ResponseStatus::ValidationFailed, "نام قفسه نامعتبر است");
    ShelfRepository shelfRepo;
    if (!shelfRepo.shelfBelongsToUser(shelfId, userId))
        return Response(ResponseStatus::Unauthorized, "شما اجازه ویرایش این قفسه را ندارید");
    if (shelfRepo.shelfNameExistsForUser(userId, newName.trimmed()))
        return Response(ResponseStatus::Error, "قفسه ای با این نام از قبل موجود است");
    if (!shelfRepo.updateShelfName(shelfId, newName.trimmed()))
        return Response(ResponseStatus::Error, "خطا در تغییر نام قفسه");
    return Response(ResponseStatus::Success, "نام قفسه با موفقیت تغییر یافت");
}
Response ShelfManager::deleteShelf(int userId, int shelfId) {
    ShelfRepository shelfRepo;
    if (!shelfRepo.shelfBelongsToUser(shelfId, userId))
        return Response(ResponseStatus::Unauthorized, "شما اجازه حذف این قفسه را ندارید");
    if (!shelfRepo.deleteShelf(shelfId))
        return Response(ResponseStatus::Error, "خطا در حذف قفسه");
    return Response(ResponseStatus::Success, "قفسه با موفقیت حذف شد");
}
Response ShelfManager::addBookToShelf(int userId, int shelfId, int bookId) {
    ShelfRepository shelfRepo;
    if (!shelfRepo.shelfBelongsToUser(shelfId, userId))
        return Response(ResponseStatus::Unauthorized, "شما اجازه افزودن کتاب به این قفسه را ندارید");
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if (!user || !user->hasPurchased(bookId)) {
        return Response(ResponseStatus::Error, "فقط کتاب هایی که خریداری کرده اید قابل افزودن به قفسه هستند");
    }
    if (shelfRepo.shelfContainsBook(shelfId, bookId)) {
        return Response(ResponseStatus::ValidationFailed, "این کتاب قبلاً در قفسه انتخاب شده قرار دارد");
    }
    if (!shelfRepo.addBookToShelf(shelfId, bookId)) {
        return Response(ResponseStatus::Error, "خطا در افزودن کتاب به قفسه (احتمالاً کتاب قبلاً در این قفسه موجود است)");
    }
    return Response(ResponseStatus::Success, "کتاب به قفسه اضافه شد");
}
Response ShelfManager::removeBookFromShelf(int userId, int shelfId, int bookId) {
    ShelfRepository shelfRepo;
    if (!shelfRepo.shelfBelongsToUser(shelfId, userId))
        return Response(ResponseStatus::Unauthorized, "شما اجازه حذف کتاب از این قفسه را ندارید");
    if (!shelfRepo.removeBookFromShelf(shelfId, bookId))
        return Response(ResponseStatus::Error, "کتاب در این قفسه یافت نشد");
    return Response(ResponseStatus::Success, "کتاب از قفسه حذف شد");
}
Response ShelfManager::moveBookBetweenShelves(int userId, int sourceShelfId, int destShelfId, int bookId) {
    ShelfRepository shelfRepo;
    if (!shelfRepo.shelfBelongsToUser(sourceShelfId, userId))
        return Response(ResponseStatus::Unauthorized, "شما مالک قفسه مبدا نیستید");
    if (!shelfRepo.shelfBelongsToUser(destShelfId, userId))
        return Response(ResponseStatus::Unauthorized, "شما مالک قفسه مقصد نیستید");
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if (!user || !user->hasPurchased(bookId)) {
        return Response(ResponseStatus::Error,
                        "فقط کتاب هایی که خریداری کرده اید قابل جابجایی هستند");
    }
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.transaction())
        return Response(ResponseStatus::Error, "خطا در شروع تراکنش انتقال کتاب");
    if (!shelfRepo.removeBookFromShelf(sourceShelfId, bookId)) {
        db.rollback();
        return Response(ResponseStatus::Error, "این کتاب در قفسه مبدا یافت نشد");
    }
    if (!shelfRepo.addBookToShelf(destShelfId, bookId)) {
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در انتقال کتاب به قفسه مقصد");
    }
    if (!db.commit()) {
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در نهایی سازی انتقال کتاب");
    }
    return Response(ResponseStatus::Success, "کتاب با موفقیت منتقل شد");
}
Response ShelfManager::reorderShelves(int userId, const QVariantList &shelfIds) {
    QVector<int> requestedIds;
    if (!convertUniquePositiveIds(shelfIds, requestedIds))
        return Response(ResponseStatus::ValidationFailed, "ترتیب قفسه ها نامعتبر است");
    ShelfRepository shelfRepo;
    const QVector<int> currentIds = shelfRepo.getShelfIdsByUser(userId);
    if (!containSameIds(currentIds, requestedIds)) {
        return Response(ResponseStatus::Unauthorized,
                        "ترتیب ارسالی باید فقط شامل تمام قفسه های متعلق به همین کاربر باشد");
    }
    if (!shelfRepo.reorderShelves(userId, requestedIds))
        return Response(ResponseStatus::Error, "خطا در ذخیره ترتیب قفسه ها");
    return Response(ResponseStatus::Success, "ترتیب قفسه ها ذخیره شد");
}
Response ShelfManager::reorderShelfBooks(int userId, int shelfId, const QVariantList &bookIds) {
    ShelfRepository shelfRepo;
    if (!shelfRepo.shelfBelongsToUser(shelfId, userId))
        return Response(ResponseStatus::Unauthorized, "شما اجازه مرتب سازی این قفسه را ندارید");
    QVector<int> requestedIds;
    if (!convertUniquePositiveIds(bookIds, requestedIds))
        return Response(ResponseStatus::ValidationFailed, "ترتیب کتاب های قفسه نامعتبر است");
    const QVector<int> currentIds = shelfRepo.getBookIdsByShelf(shelfId);
    if (!containSameIds(currentIds, requestedIds)) {
        return Response(ResponseStatus::ValidationFailed, "ترتیب ارسالی باید شامل تمام کتاب های همین قفسه باشد");
    }
    if (!shelfRepo.reorderShelfBooks(shelfId, requestedIds))
        return Response(ResponseStatus::Error, "خطا در ذخیره ترتیب کتاب های قفسه");
    return Response(ResponseStatus::Success, "ترتیب کتاب های قفسه ذخیره شد");
}
Response ShelfManager::getUserShelves(int userId) {
    ShelfRepository shelfRepo;
    const QVector<int> shelfIds = shelfRepo.getShelfIdsByUser(userId);
    QVariantList shelfList;
    BookRepository bookRepo;
    for (int shelfId : shelfIds) {
        std::unique_ptr<Shelf> shelf(shelfRepo.loadShelfById(shelfId));
        if (!shelf)
            return Response(ResponseStatus::Error, "اطلاعات یکی از قفسه ها قابل بازیابی نیست");
        QVariantMap shelfData;
        shelfData["shelfId"] = shelf->getShelfId();
        shelfData["shelfName"] = shelf->getShelfName();
        shelfData["bookCount"] = shelf->getBookCount();
        QVariantList bookIdList;
        QVariantList bookNameList;
        for (int bookId : shelf->getBookIds()) {
            std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
            if (!book) {
                return Response(ResponseStatus::Error, "نام یکی از کتاب های قفسه با جدول Books تطبیق ندارد");
            }
            bookIdList.append(bookId);
            bookNameList.append(book->getBookName());
        }
        shelfData["bookIds"] = bookIdList;
        shelfData["bookNames"] = bookNameList;
        shelfList.append(shelfData);
    }
    QVariantMap data;
    data["shelves"] = shelfList;
    return Response(ResponseStatus::Success, "قفسه ها بازیابی شدند", data);
}