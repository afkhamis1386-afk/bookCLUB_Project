#include "SavedBookManager.h"
#include "SavedBookRepository.h"
#include "BookRepository.h"
#include "../common/Book.h"
#include <memory>
SavedBookManager::SavedBookManager(){}
Response SavedBookManager::saveBook(int userId, int bookId){
    BookRepository bookRepo;
    std::unique_ptr<Book> book(bookRepo.loadBookById(bookId));
    if(!book){
        return Response(ResponseStatus::NotFound, "کتاب یافت نشد");
    }
    SavedBookRepository savedRepo;
    if(savedRepo.isBookSaved(userId, bookId)){
        return Response(ResponseStatus::Error, "این کتاب قبلاً ذخیره شده است");
    }
    if(!savedRepo.saveBook(userId, bookId)){
        return Response(ResponseStatus::Error, "خطا در ذخیره کتاب");
    }
    return Response(ResponseStatus::Success, "کتاب به لیست علاقه مندی ها اضافه شد");
}
Response SavedBookManager::unsaveBook(int userId, int bookId){
    SavedBookRepository savedRepo;
    if(!savedRepo.isBookSaved(userId, bookId)){
        return Response(ResponseStatus::Error, "این کتاب در لیست ذخیره شده ها نیست");
    }
    if(!savedRepo.unsaveBook(userId, bookId)){
        return Response(ResponseStatus::Error, "خطا در حذف کتاب از لیست ذخیره شده ها");
    }
    return Response(ResponseStatus::Success, "کتاب از لیست ذخیره شده ها حذف شد");
}
Response SavedBookManager::getSavedBooks(int userId){
    SavedBookRepository savedRepo;
    QVector<int> ids = savedRepo.getSavedBookIds(userId);
    QVariantList bookList;
    for(int id : qAsConst(ids))
        bookList.append(id);
    QVariantMap data;
    data["bookIds"] = bookList;
    return Response(ResponseStatus::Success, "کتاب های ذخیره شده بازیابی شدند", data);
}