#ifndef BOOKREADERCONTROLLER_H
#define BOOKREADERCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "NetworkManager.h"
class BookReaderController : public QObject {
    Q_OBJECT

public:
    explicit BookReaderController(NetworkManager *networkManager, QObject *parent = nullptr);
    void openBook(int bookId);
    void updateCurrentPage(int currentPage);
    void flushPendingSave();

signals:
    void bookReady(const QString &localFilePath, int startPage);
    void bookOpenFailed(const QString &message);
    void progressSaved();
    void progressSaveFailed(const QString &message);
    void validationError(const QString &message);

private slots:
    void onResponseReceived(RequestType type, const Response &response);
    void onSaveTimerTimeout();

private:
    NetworkManager *networkManager;
    int currentBookId;
    int pendingPageToSave;
    QTimer *saveDebounceTimer;
    QString localCachePathFor(int bookId) const;
};

#endif // BOOKREADERCONTROLLER_H