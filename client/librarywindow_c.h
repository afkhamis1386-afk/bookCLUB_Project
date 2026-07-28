#ifndef LIBRARYWINDOW_C_H
#define LIBRARYWINDOW_C_H

#include <QMainWindow>
#include <QModelIndex>
#include <QVariantList>
#include "NetworkManager.h"
#include "LibraryController.h"
#include "SavedBookController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LibraryWindow_c;
}
QT_END_NAMESPACE

class QListWidget;

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
    void onShelvesRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                            const QModelIndex &destinationParent, int destinationRow);
    void onShelfBooksRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                               const QModelIndex &destinationParent, int destinationRow);
    void onShelvesReordered(const QString &message);
    void onShelvesReorderFailed(const QString &message);
    void onShelfBooksReordered(const QString &message);
    void onShelfBooksReorderFailed(const QString &message);

    void onSavedBooksLoaded(const QVariantList &books);
    void onSavedBooksLoadFailed(const QString &message);
    void onRemoveSavedBookButtonClicked();
    void onBookUnsaved(const QString &message);
    void onBookUnsaveFailed(const QString &message);
    void onFavoriteBooksLoaded(const QVariantList &books);
    void onFavoriteBooksLoadFailed(const QString &message);
    void onAddFavoriteBookButtonClicked();
    void onRemoveFavoriteBookButtonClicked();
    void onFavoriteBookAdded(const QString &message);
    void onFavoriteBookAddFailed(const QString &message);
    void onFavoriteBookRemoved(const QString &message);
    void onFavoriteBookRemoveFailed(const QString &message);
    void onFavoriteBooksRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                  const QModelIndex &destinationParent, int destinationRow);
    void onFavoriteBooksReordered(const QString &message);
    void onFavoriteBooksReorderFailed(const QString &message);

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
    QVariantList currentSavedBooks;
    QVariantList currentFavoriteBooks;
    bool applyingServerData;
    void configureReorderableList(QListWidget *listWidget);
    void populateShelvesList(const QVariantList &shelves);
    void populateAddBookCombo();
    void populateSavedBooksList();
    void populateFavoriteBooksList();
    void populateFavoriteBookCombo();
    void syncCurrentShelvesToWidgetOrder();
    QVariantList itemIds(const QListWidget *listWidget) const;
    QVariantMap selectedShelfData() const;
    int getSelectedShelfId() const;
};

#endif // LIBRARYWINDOW_C_H