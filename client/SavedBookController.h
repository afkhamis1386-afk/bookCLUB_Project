#ifndef SAVEDBOOKCONTROLLER_H
#define SAVEDBOOKCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include "NetworkManager.h"

class SavedBookController : public QObject {
    Q_OBJECT

public:
    explicit SavedBookController(NetworkManager *networkManager, QObject *parent = nullptr);
    void saveBook(int bookId);
    void unsaveBook(int bookId);
    void refreshSavedBooks();
signals:
    void bookSaved(const QString &message);
    void bookSaveFailed(const QString &message);
    void bookUnsaved(const QString &message);
    void bookUnsaveFailed(const QString &message);
    void savedBooksLoaded(const QVariantList &bookIds);
    void savedBooksLoadFailed(const QString &message);
    void validationError(const QString &message);
private slots:
    void onResponseReceived(RequestType type, const Response &response);
private:
    NetworkManager *networkManager;
};

#endif // SAVEDBOOKCONTROLLER_H
