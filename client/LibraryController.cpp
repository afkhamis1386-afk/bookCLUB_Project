#include "LibraryController.h"
LibraryController::LibraryController(NetworkManager *networkManager, QObject *parent):QObject(parent), networkManager(networkManager){
    connect(networkManager, &NetworkManager::responseReceived, this, &LibraryController::onResponseReceived);
}
void LibraryController::refreshShelves(){
    if(!networkManager->isConnected()){
        emit shelvesLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getShelf();
}
void LibraryController::createShelf(const QString &shelfName){
    if(shelfName.trimmed().isEmpty() || shelfName.length() > 100){
        emit validationError("نام قفسه نامعتبر است (باید بین ۱ تا ۱۰۰ کاراکتر باشد)");
        return;
    }
    if(!networkManager->isConnected()){
        emit shelfCreateFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->createShelf(shelfName.trimmed());
}
void LibraryController::renameShelf(int shelfId, const QString &newName){
    if(shelfId <= 0 || newName.trimmed().isEmpty() || newName.length() > 100){
        emit validationError("ورودی نامعتبر است");
        return;
    }
    if(!networkManager->isConnected()){
        emit shelfRenameFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->renameShelf(shelfId, newName.trimmed());
}
void LibraryController::deleteShelf(int shelfId){
    if(shelfId <= 0){
        emit validationError("شناسه قفسه نامعتبر است");
        return;
    }
    if (!networkManager->isConnected()){
        emit shelfDeleteFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->deleteShelf(shelfId);
}
void LibraryController::addBookToShelf(int shelfId, int bookId){
    if(shelfId <= 0 || bookId <= 0){
        emit validationError("ورودی نامعتبر است");
        return;
    }
    if(!networkManager->isConnected()){
        emit bookAddToShelfFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->addBookToShelf(shelfId, bookId);
}
void LibraryController::moveBookBetweenShelves(int sourceShelfId, int destShelfId, int bookId){
    if(sourceShelfId <= 0 || destShelfId <= 0 || bookId <= 0){
        emit validationError("ورودی نامعتبر است");
        return;
    }
    if(sourceShelfId == destShelfId){
        emit validationError("قفسه مبدا و مقصد نمی توانند یکسان باشند");
        return;
    }
    if(!networkManager->isConnected()){
        emit bookMoveFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->moveBookBetweenShelves(sourceShelfId, destShelfId, bookId);
}
void LibraryController::onResponseReceived(RequestType type, const Response &response){
    switch(type){
    case RequestType::GetShelf:
        if(response.isSuccess())
            emit shelvesLoaded(response.getData().value("shelves").toList());
        else
            emit shelvesLoadFailed(response.getMessage());
        break;
    case RequestType::CreateShelf:
        if(response.isSuccess())
            emit shelfCreated(response.getData().value("shelfId").toInt(), response.getMessage());
        else
            emit shelfCreateFailed(response.getMessage());
        break;
    case RequestType::RenameShelf:
        if(response.isSuccess())
            emit shelfRenamed(response.getMessage());
        else
            emit shelfRenameFailed(response.getMessage());
        break;
    case RequestType::DeleteShelf:
        if(response.isSuccess())
            emit shelfDeleted(response.getMessage());
        else
            emit shelfDeleteFailed(response.getMessage());
        break;
    case RequestType::AddBookToShelf:
        if(response.isSuccess())
            emit bookAddedToShelf(response.getMessage());
        else
            emit bookAddToShelfFailed(response.getMessage());
        break;
    case RequestType::MoveBookBetweenShelves:
        if(response.isSuccess())
            emit bookMoved(response.getMessage());
        else
            emit bookMoveFailed(response.getMessage());
        break;
    default:
        break;
    }
}