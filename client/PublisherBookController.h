#ifndef PUBLISHERBOOKCONTROLLER_H
#define PUBLISHERBOOKCONTROLLER_H

#include <QObject>
#include "NetworkManager.h"
class PublisherBookController : public QObject {
    Q_OBJECT
public:
    explicit PublisherBookController(NetworkManager *networkManager, QObject *parent = nullptr);
    void addBook(const QString &bookName, const QString &description, double price,
                 const QString &genreTitle, const QString &categoryTitle, const QString &authorName,
                 const QString &coverImageFilePath, const QString &pdfFilePath);
    void updateBook(int bookId, const QString &bookName, const QString &description, double price);
    void deactivateBook(int bookId);
    void reactivateBook(int bookId);
    void applyDiscount(int bookId, double discountPercent, double discountAmount);
signals:
    void bookAdded(int bookId, const QString &message);
    void bookAddFailed(const QString &message);
    void bookFileReadFailed(const QString &message);
    void bookUpdated(const QString &message);
    void bookUpdateFailed(const QString &message);
    void bookDeactivated(const QString &message);
    void bookDeactivateFailed(const QString &message);
    void validationError(const QString &message);
    void bookReactivated(const QString &message);
    void bookReactivateFailed(const QString &message);
    void discountApplied(const QString &message);
    void discountApplyFailed(const QString &message);
private slots:
    void onResponseReceived(RequestType type, const Response &response);
private:
    NetworkManager *networkManager;
};

#endif // PUBLISHERBOOKCONTROLLER_H
