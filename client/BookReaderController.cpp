#include "BookReaderController.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
BookReaderController::BookReaderController(NetworkManager *networkManager, QObject *parent):QObject(parent), networkManager(networkManager), currentBookId(-1), pendingPageToSave(-1){
    connect(networkManager, &NetworkManager::responseReceived, this, &BookReaderController::onResponseReceived);
    saveDebounceTimer = new QTimer(this);
    saveDebounceTimer->setSingleShot(true);
    saveDebounceTimer->setInterval(2000);
    connect(saveDebounceTimer, &QTimer::timeout, this, &BookReaderController::onSaveTimerTimeout);
}
QString BookReaderController::localCachePathFor(int bookId) const {
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/BookCache";
    QDir dir(baseDir);
    if(!dir.exists()) dir.mkpath(".");
    return QString("%1/book_%2.pdf").arg(baseDir).arg(bookId);
}
void BookReaderController::openBook(int bookId){
    if(bookId <= 0){
        emit validationError("شناسه کتاب نامعتبر است");
        return;
    }
    currentBookId = bookId;
    if(!networkManager->isConnected()){
        emit bookOpenFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getBookFile(bookId);
}
void BookReaderController::updateCurrentPage(int currentPage){
    if(currentBookId <= 0 || currentPage < 1){
        return;
    }
    pendingPageToSave = currentPage;
    saveDebounceTimer->start();
}
void BookReaderController::onSaveTimerTimeout(){
    if(currentBookId <= 0 || pendingPageToSave < 1) return;
    if(!networkManager->isConnected()){
        emit progressSaveFailed("اتصال به سرور برقرار نیست — پیشرفت مطالعه ذخیره نشد");
        return;
    }
    networkManager->saveReadingProgress(currentBookId, pendingPageToSave);
}
void BookReaderController::onResponseReceived(RequestType type, const Response &response) {
    switch(type){
    case RequestType::GetBookFile: {
        if(!response.isSuccess()){
            emit bookOpenFailed(response.getMessage());
            return;
        }
        QByteArray pdfData = response.getData().value("pdfData").toByteArray();
        int lastPage = response.getData().value("lastPage").toInt();
        if(lastPage < 1) lastPage = 1;
        QString localPath = localCachePathFor(currentBookId);
        QFile file(localPath);
        if(!file.open(QIODevice::WriteOnly)){
            emit bookOpenFailed("خطا در ذخیره سازی موقت فایل کتاب روی سیستم شما");
            return;
        }
        file.write(pdfData);
        file.close();
        emit bookReady(localPath, lastPage);
        break;
    }
    case RequestType::SaveReadingProgress:
        if(response.isSuccess())
            emit progressSaved();
        else
            emit progressSaveFailed(response.getMessage());
        break;
    default:
        break;
    }
}