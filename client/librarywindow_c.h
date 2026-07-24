#ifndef LIBRARYWINDOW_C_H
#define LIBRARYWINDOW_C_H

#include <QMainWindow>
#include "NetworkManager.h"
#include "LibraryController.h"
#include "SavedBookController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LibraryWindow_c;
}
QT_END_NAMESPACE

class LibraryWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit LibraryWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~LibraryWindow_c() override;

signals:
    void backRequested();

private slots:
    void onShelvesLoaded(const QVariantList &shelves);
    void onShelvesLoadFailed(const QString &message);
    void onShelfSelectionChanged();
    void onCreateShelfButtonClicked();
    void onShelfCreated(int shelfId, const QString &message);
    void onShelfCreateFailed(const QString &message);
    void onRenameShelfButtonClicked();
    void onShelfRenamed(const QString &message);
    void onShelfRenameFailed(const QString &message);
    void onDeleteShelfButtonClicked();
    void onShelfDeleted(const QString &message);
    void onShelfDeleteFailed(const QString &message);
    void onAddBookToShelfButtonClicked();
    void onBookAddedToShelf(const QString &message);
    void onBookAddToShelfFailed(const QString &message);
    void onRemoveBookFromShelfButtonClicked();
    void onBookRemovedFromShelf(const QString &message);
    void onBookRemoveFromShelfFailed(const QString &message);
    void onSavedBooksLoaded(const QVariantList &bookIds);
    void onSavedBooksLoadFailed(const QString &message);
    void onRemoveSavedBookButtonClicked();
    void onBookUnsaved(const QString &message);
    void onBookUnsaveFailed(const QString &message);
    void onViewSavedBookButtonClicked();
    void onValidationError(const QString &message);
    void onBackButtonClicked();
    void onPurchasedBooksLoaded(const QVariantList &bookIds);
    void onPurchasedBooksLoadFailed(const QString &message);
    void onOpenBookButtonClicked();
    void onViewBookDetailButtonClicked();
private:
    Ui::LibraryWindow_c *ui;
    NetworkManager *networkManager;
    LibraryController *libraryController;
    SavedBookController *savedBookController;
    QVariantList currentShelves;
    void populateShelvesList(const QVariantList &shelves);
    void populateAddBookCombo();
    int getSelectedShelfId() const;
};

#endif // LIBRARYWINDOW_C_H
