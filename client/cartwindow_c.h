#ifndef CARTWINDOW_C_H
#define CARTWINDOW_C_H
#include <QMainWindow>
#include "NetworkManager.h"
#include "CartController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class CartWindow_c;
}
QT_END_NAMESPACE

class CartWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit CartWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~CartWindow_c() override;
signals:
    void backRequested();
private slots:
    void onCartLoaded(const QVariantMap &cartData);
    void onCartLoadFailed(const QString &message);
    void onRemoveSelectedButtonClicked();
    void onRemoveSucceeded(const QString &message);
    void onRemoveFailed(const QString &message);
    void onCheckoutButtonClicked();
    void onCheckoutSucceeded(const QVariantMap &orderData);
    void onCheckoutFailed(const QString &message);
    void onBackButtonClicked();
private:
    Ui::CartWindow_c *ui;
    NetworkManager *networkManager;
    CartController *cartController;
    double currentFinalPrice = 0;
    void populateTable(const QVariantList &items);
};

#endif // CARTWINDOW_C_H

