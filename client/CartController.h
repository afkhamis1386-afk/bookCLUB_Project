#ifndef CARTCONTROLLER_H
#define CARTCONTROLLER_H

#include <QObject>
#include <QVariantMap>
#include "NetworkManager.h"

class CartController : public QObject {
    Q_OBJECT
public:
    explicit CartController(NetworkManager *networkManager, QObject *parent = nullptr);
    void addBook(int bookId);
    void removeBook(int bookId);
    void refreshCart();
    void checkout();
signals:
    void addToCartSucceeded(const QString &message);
    void addToCartFailed(const QString &message);
    void removeFromCartSucceeded(const QString &message);
    void removeFromCartFailed(const QString &message);
    void cartLoaded(const QVariantMap &cartData);
    void cartLoadFailed(const QString &message);
    void checkoutSucceeded(const QVariantMap &orderData);
    void checkoutFailed(const QString &message);
    void validationError(const QString &message);
private slots:
    void onResponseReceived(RequestType type, const Response &response);
private:
    NetworkManager *networkManager;
};
#endif // CARTCONTROLLER_H
