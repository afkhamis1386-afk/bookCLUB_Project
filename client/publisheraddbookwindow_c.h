#ifndef PUBLISHERADDBOOKWINDOW_C_H
#define PUBLISHERADDBOOKWINDOW_C_H

#include <QMainWindow>
#include "NetworkManager.h"
#include "PublisherBookController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class PublisherAddBookWindow_c;
}
QT_END_NAMESPACE

class PublisherAddBookWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit PublisherAddBookWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~PublisherAddBookWindow_c() override;

signals:
    void backRequested();
private slots:
    void onSelectCoverButtonClicked();
    void onSelectPdfButtonClicked();
    void onSubmitButtonClicked();
    void onBookAdded(int bookId, const QString &message);
    void onBookAddFailed(const QString &message);
    void onBookFileReadFailed(const QString &message);
    void onValidationError(const QString &message);
    void onBackButtonClicked();

private:
    Ui::PublisherAddBookWindow_c *ui;
    NetworkManager *networkManager;
    PublisherBookController *publisherBookController;
    QString selectedCoverPath;
    QString selectedPdfPath;
};

#endif // PUBLISHERADDBOOKWINDOW_C_H
