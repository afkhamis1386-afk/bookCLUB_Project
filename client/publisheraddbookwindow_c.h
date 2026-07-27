#ifndef PUBLISHERADDBOOKWINDOW_C_H
#define PUBLISHERADDBOOKWINDOW_C_H

#include <QMainWindow>
#include "NetworkManager.h"
#include "PublisherBookController.h"
#include "ProfileController.h"

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
    explicit PublisherAddBookWindow_c(NetworkManager *networkManager, int editBookId, QWidget *parent = nullptr);
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
    void onGenresLoaded(const QVariantList &genres);
    void onGenresLoadFailed(const QString &message);
    void onDiscountTypeToggled();
    void onBookUpdated(const QString &message);
    void onBookUpdateFailed(const QString &message);
    void onBookDetailsForEditLoaded(const QVariantMap &bookData);
    void onBookDetailsForEditLoadFailed(const QString &message);

private:
    void setupCommon();
    void applySelectedDiscount(int bookId);

    Ui::PublisherAddBookWindow_c *ui;
    NetworkManager *networkManager;
    PublisherBookController *publisherBookController;
    ProfileController *profileController;
    QString selectedCoverPath;
    QString selectedPdfPath;

    bool isEditMode = false;
    int editingBookId = -1;
    bool pendingGenreSelection = false;
    QString pendingGenreTitle;
    bool hadTimedDiscount = false;
};

#endif // PUBLISHERADDBOOKWINDOW_C_H
