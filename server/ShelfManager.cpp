#include "ShelfManager.h"
#include "ShelfRepository.h"
#include "DatabaseManager.h"
#include "UserRepository.h"
#include "../common/Shelf.h"
#include "../common/normaluser.h"
#include <QSqlDatabase>
#include <memory>
ShelfManager::ShelfManager(){}
Response ShelfManager::createShelf(int userId, const QString &shelfName){
    if(shelfName.trimmed().isEmpty() || shelfName.length() > 100){
        return Response(ResponseStatus::ValidationFailed, "نام قفسه نمی تواند خالی باشد و باید حداکثر ۱۰۰ کاراکتر باشد");
    }
    ShelfRepository shelfRepo;
    if(shelfRepo.shelfNameExistsForUser(userId, shelfName.trimmed())){
        return Response(ResponseStatus::Error, "قفسه ای با این نام از قبل موجود است");
    }
    Shelf newShelf(userId, shelfName.trimmed());
    int newShelfId = shelfRepo.insertShelf(newShelf);
    if(newShelfId == -1){
        return Response(ResponseStatus::Error, "خطا در ساخت قفسه");
    }
    QVariantMap data;
    data["shelfId"] = newShelfId;
    return Response(ResponseStatus::Success, "قفسه با موفقیت ساخته شد", data);
}
Response ShelfManager::renameShelf(int userId, int shelfId, const QString &newName){
    if(newName.trimmed().isEmpty() || newName.length() > 100){
        return Response(ResponseStatus::ValidationFailed, "نام قفسه نامعتبر است");
    }
    ShelfRepository shelfRepo;
    if(!shelfRepo.shelfBelongsToUser(shelfId, userId)){
        return Response(ResponseStatus::Unauthorized, "شما اجازه ویرایش این قفسه را ندارید");
    }
    if(shelfRepo.shelfNameExistsForUser(userId, newName.trimmed())){
        return Response(ResponseStatus::Error, "قفسه ای با این نام از قبل موجود است");
    }
    if(!shelfRepo.updateShelfName(shelfId, newName.trimmed())){
        return Response(ResponseStatus::Error, "خطا در تغییر نام قفسه");
    }
    return Response(ResponseStatus::Success, "نام قفسه با موفقیت تغییر یافت");
}
Response ShelfManager::deleteShelf(int userId, int shelfId){
    ShelfRepository shelfRepo;
    if(!shelfRepo.shelfBelongsToUser(shelfId, userId)){
        return Response(ResponseStatus::Unauthorized, "شما اجازه حذف این قفسه را ندارید");
    }
    if(!shelfRepo.deleteShelf(shelfId)){
        return Response(ResponseStatus::Error, "خطا در حذف قفسه");
    }
    return Response(ResponseStatus::Success, "قفسه با موفقیت حذف شد");
}
Response ShelfManager::addBookToShelf(int userId, int shelfId, int bookId) {
    ShelfRepository shelfRepo;
    if(!shelfRepo.shelfBelongsToUser(shelfId, userId)){
        return Response(ResponseStatus::Unauthorized, "شما اجازه افزودن کتاب به این قفسه را ندارید");
    }
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if(!user || !user->hasPurchased(bookId)){
        return Response(ResponseStatus::Error, "فقط کتاب هایی که خریداری کرده اید قابل افزودن به قفسه هستند");
    }
    if(!shelfRepo.addBookToShelf(shelfId, bookId)){
        return Response(ResponseStatus::Error, "خطا در افزودن کتاب به قفسه (احتمالاً کتاب قبلاً در این قفسه موجود است)");
    }
    return Response(ResponseStatus::Success, "کتاب به قفسه اضافه شد");
}
Response ShelfManager::moveBookBetweenShelves(int userId, int sourceShelfId, int destShelfId, int bookId){
    ShelfRepository shelfRepo;
    if(!shelfRepo.shelfBelongsToUser(sourceShelfId, userId)){
        return Response(ResponseStatus::Unauthorized, "شما مالک قفسه مبدا نیستید");
    }
    if(!shelfRepo.shelfBelongsToUser(destShelfId, userId)){
        return Response(ResponseStatus::Unauthorized, "شما مالک قفسه مقصد نیستید");
    }
    UserRepository userRepo;
    std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
    if(!user || !user->hasPurchased(bookId)){
        return Response(ResponseStatus::Error, "فقط کتاب هایی که خریداری کرده اید قابل جابجایی هستند");
    }
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if(!db.transaction()){
        return Response(ResponseStatus::Error, "خطا در شروع تراکنش انتقال کتاب");
    }
    if(!shelfRepo.removeBookFromShelf(sourceShelfId, bookId)){
        db.rollback();
        return Response(ResponseStatus::Error, "این کتاب در قفسه مبدا یافت نشد");
    }
    if(!shelfRepo.addBookToShelf(destShelfId, bookId)){
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در انتقال کتاب به قفسه مقصد");
    }
    if(!db.commit()){
        db.rollback();
        return Response(ResponseStatus::Error, "خطا در نهایی سازی انتقال کتاب");
    }
    return Response(ResponseStatus::Success, "کتاب با موفقیت منتقل شد");
}
Response ShelfManager::getUserShelves(int userId){
    ShelfRepository shelfRepo;
    QVector<int> shelfIds = shelfRepo.getShelfIdsByUser(userId);
    QVariantList shelfList;
    for(int shelfId : qAsConst(shelfIds)){
        std::unique_ptr<Shelf> shelf(shelfRepo.loadShelfById(shelfId));
        if (!shelf) continue;
        QVariantMap shelfData;
        shelfData["shelfId"] = shelf->getShelfId();
        shelfData["shelfName"] = shelf->getShelfName();
        shelfData["bookCount"] = shelf->getBookCount();
        QVariantList bookIdList;
        for(int bookId : shelf->getBookIds())
            bookIdList.append(bookId);
        shelfData["bookIds"] = bookIdList;
        shelfList.append(shelfData);
    }
    QVariantMap data;
    data["shelves"] = shelfList;
    return Response(ResponseStatus::Success, "قفسه ها بازیابی شدند", data);
}
