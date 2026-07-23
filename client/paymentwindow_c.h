#ifndef PAYMENTWINDOW_C_H
#define PAYMENTWINDOW_C_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class PaymentWindow_c;
}
QT_END_NAMESPACE

class PaymentWindow_c : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentWindow_c(double totalAmount, const QString &expectedCardHolderName, QWidget *parent = nullptr);
    ~PaymentWindow_c() override;

    QString getCardNumber() const;

private slots:
    void onPayButtonClicked();
    void onCancelButtonClicked();

private:
    Ui::PaymentWindow_c *ui;
    QString expectedCardHolderName;
};

#endif // PAYMENTWINDOW_C_H