#include "librarywindow_c.h"
#include "ui_librarywindow_c.h"
#include "bookdetailswindow_c.h"
#include "bookreaderwindow_c.h"
#include "windownav.h"
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QGuiApplication>
#include <QInputDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QScreen>
#include <QSet>

LibraryWindow_c::LibraryWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LibraryWindow_c)
    , networkManager(networkManager)
    , libraryController(new LibraryController(networkManager, this))
    , savedBookController(new SavedBookController(networkManager, this))
    , applyingServerData(false)
{
    ui->setupUi(this);

    configureReorderableList(ui->shelvesListWidget);
    configureReorderableList(ui->shelfBookListWidget);
    configureReorderableList(ui->favoriteBooksListWidget);

    connect(ui->createShelfButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onCreateShelfButtonClicked);
    connect(ui->renameShelfButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onRenameShelfButtonClicked);
    connect(ui->deleteShelfButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onDeleteShelfButtonClicked);
    connect(ui->addBookToShelfButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onAddBookToShelfButtonClicked);
    connect(ui->removeBookFromShelfButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onRemoveBookFromShelfButtonClicked);
    connect(ui->shelvesListWidget, &QListWidget::currentRowChanged,
            this, &LibraryWindow_c::onShelfSelectionChanged);
    connect(ui->shelvesListWidget->model(), &QAbstractItemModel::rowsMoved,
            this, &LibraryWindow_c::onShelvesRowsMoved);
    connect(ui->shelfBookListWidget->model(), &QAbstractItemModel::rowsMoved,
            this, &LibraryWindow_c::onShelfBooksRowsMoved);

    connect(ui->removeSavedBookButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onRemoveSavedBookButtonClicked);
    connect(ui->addFavoriteBookButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onAddFavoriteBookButtonClicked);
    connect(ui->removeFavoriteBookButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onRemoveFavoriteBookButtonClicked);
    connect(ui->favoriteBooksListWidget->model(), &QAbstractItemModel::rowsMoved,
            this, &LibraryWindow_c::onFavoriteBooksRowsMoved);

    connect(ui->backButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onBackButtonClicked);
    connect(ui->openBookButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onOpenBookButtonClicked);
    connect(ui->viewBookDetailButton, &QPushButton::clicked,
            this, &LibraryWindow_c::onViewBookDetailButtonClicked);

    connect(libraryController, &LibraryController::shelvesLoaded,
            this, &LibraryWindow_c::onShelvesLoaded);
    connect(libraryController, &LibraryController::shelvesLoadFailed,
            this, &LibraryWindow_c::onShelvesLoadFailed);
    connect(libraryController, &LibraryController::shelfCreated,
            this, &LibraryWindow_c::onShelfCreated);
    connect(libraryController, &LibraryController::shelfCreateFailed,
            this, &LibraryWindow_c::onShelfCreateFailed);
    connect(libraryController, &LibraryController::shelfRenamed,
            this, &LibraryWindow_c::onShelfRenamed);
    connect(libraryController, &LibraryController::shelfRenameFailed,
            this, &LibraryWindow_c::onShelfRenameFailed);
    connect(libraryController, &LibraryController::shelfDeleted,
            this, &LibraryWindow_c::onShelfDeleted);
    connect(libraryController, &LibraryController::shelfDeleteFailed,
            this, &LibraryWindow_c::onShelfDeleteFailed);
    connect(libraryController, &LibraryController::bookAddedToShelf,
            this, &LibraryWindow_c::onBookAddedToShelf);
    connect(libraryController, &LibraryController::bookAddToShelfFailed,
            this, &LibraryWindow_c::onBookAddToShelfFailed);
    connect(libraryController, &LibraryController::bookRemovedFromShelf,
            this, &LibraryWindow_c::onBookRemovedFromShelf);
    connect(libraryController, &LibraryController::bookRemoveFromShelfFailed,
            this, &LibraryWindow_c::onBookRemoveFromShelfFailed);
    connect(libraryController, &LibraryController::shelvesReordered,
            this, &LibraryWindow_c::onShelvesReordered);
    connect(libraryController, &LibraryController::shelvesReorderFailed,
            this, &LibraryWindow_c::onShelvesReorderFailed);
    connect(libraryController, &LibraryController::shelfBooksReordered,
            this, &LibraryWindow_c::onShelfBooksReordered);
    connect(libraryController, &LibraryController::shelfBooksReorderFailed,
            this, &LibraryWindow_c::onShelfBooksReorderFailed);
    connect(libraryController, &LibraryController::validationError,
            this, &LibraryWindow_c::onValidationError);
    connect(libraryController, &LibraryController::purchasedBooksLoaded,
            this, &LibraryWindow_c::onPurchasedBooksLoaded);
    connect(libraryController, &LibraryController::purchasedBooksLoadFailed,
            this, &LibraryWindow_c::onPurchasedBooksLoadFailed);

    connect(savedBookController, &SavedBookController::savedBooksLoaded,
            this, &LibraryWindow_c::onSavedBooksLoaded);
    connect(savedBookController, &SavedBookController::savedBooksLoadFailed,
            this, &LibraryWindow_c::onSavedBooksLoadFailed);
    connect(savedBookController, &SavedBookController::bookUnsaved,
            this, &LibraryWindow_c::onBookUnsaved);
    connect(savedBookController, &SavedBookController::bookUnsaveFailed,
            this, &LibraryWindow_c::onBookUnsaveFailed);
    connect(savedBookController, &SavedBookController::favoriteBooksLoaded,
            this, &LibraryWindow_c::onFavoriteBooksLoaded);
    connect(savedBookController, &SavedBookController::favoriteBooksLoadFailed,
            this, &LibraryWindow_c::onFavoriteBooksLoadFailed);
    connect(savedBookController, &SavedBookController::favoriteBookAdded,
            this, &LibraryWindow_c::onFavoriteBookAdded);
    connect(savedBookController, &SavedBookController::favoriteBookAddFailed,
            this, &LibraryWindow_c::onFavoriteBookAddFailed);
    connect(savedBookController, &SavedBookController::favoriteBookRemoved,
            this, &LibraryWindow_c::onFavoriteBookRemoved);
    connect(savedBookController, &SavedBookController::favoriteBookRemoveFailed,
            this, &LibraryWindow_c::onFavoriteBookRemoveFailed);
    connect(savedBookController, &SavedBookController::favoriteBooksReordered,
            this, &LibraryWindow_c::onFavoriteBooksReordered);
    connect(savedBookController, &SavedBookController::favoriteBooksReorderFailed,
            this, &LibraryWindow_c::onFavoriteBooksReorderFailed);
    connect(savedBookController, &SavedBookController::validationError,
            this, &LibraryWindow_c::onValidationError);

    libraryController->refreshShelves();
    libraryController->refreshPurchasedBooks();
    savedBookController->refreshSavedBooks();
    savedBookController->refreshFavoriteBooks();
}

LibraryWindow_c::~LibraryWindow_c()
{
    delete ui;
}

void LibraryWindow_c::configureReorderableList(QListWidget *listWidget)
{
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setDragEnabled(true);
    listWidget->setAcceptDrops(true);
    listWidget->setDropIndicatorShown(true);
    listWidget->setDefaultDropAction(Qt::MoveAction);
    listWidget->setDragDropMode(QAbstractItemView::InternalMove);
}

QVariantList LibraryWindow_c::itemIds(const QListWidget *listWidget) const
{
    QVariantList ids;
    for (int i = 0; i < listWidget->count(); ++i)
        ids.append(listWidget->item(i)->data(Qt::UserRole).toInt());
    return ids;
}

int LibraryWindow_c::getSelectedShelfId() const
{
    QListWidgetItem *item = ui->shelvesListWidget->currentItem();
    return item ? item->data(Qt::UserRole).toInt() : -1;
}

QVariantMap LibraryWindow_c::selectedShelfData() const
{
    const int shelfId = getSelectedShelfId();
    for (const QVariant &value : currentShelves) {
        const QVariantMap shelf = value.toMap();
        if (shelf.value("shelfId").toInt() == shelfId)
            return shelf;
    }
    return QVariantMap();
}

void LibraryWindow_c::syncCurrentShelvesToWidgetOrder()
{
    const QVariantList orderedIds = itemIds(ui->shelvesListWidget);
    QVariantList orderedShelves;
    for (const QVariant &idValue : orderedIds) {
        const int shelfId = idValue.toInt();
        for (const QVariant &shelfValue : currentShelves) {
            const QVariantMap shelf = shelfValue.toMap();
            if (shelf.value("shelfId").toInt() == shelfId) {
                orderedShelves.append(shelf);
                break;
            }
        }
    }
    if (orderedShelves.size() == currentShelves.size())
        currentShelves = orderedShelves;
}

void LibraryWindow_c::populateShelvesList(const QVariantList &shelves)
{
    const int previousShelfId = getSelectedShelfId();
    currentShelves = shelves;
    applyingServerData = true;
    ui->shelvesListWidget->clear();

    int selectedRow = -1;
    for (int i = 0; i < shelves.size(); ++i) {
        const QVariantMap shelf = shelves.at(i).toMap();
        const int shelfId = shelf.value("shelfId").toInt();
        const QString text = QString("%1 (%2 کتاب)")
                                 .arg(shelf.value("shelfName").toString())
                                 .arg(shelf.value("bookCount").toInt());
        QListWidgetItem *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, shelfId);
        ui->shelvesListWidget->addItem(item);
        if (shelfId == previousShelfId)
            selectedRow = i;
    }

    if (selectedRow < 0 && ui->shelvesListWidget->count() > 0)
        selectedRow = 0;
    ui->shelvesListWidget->setCurrentRow(selectedRow);
    applyingServerData = false;
    populateAddBookCombo();
    onShelfSelectionChanged();
}

void LibraryWindow_c::onShelvesLoaded(const QVariantList &shelves)
{
    populateShelvesList(shelves);
}

void LibraryWindow_c::onShelvesLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onShelfSelectionChanged()
{
    applyingServerData = true;
    ui->shelfBookListWidget->clear();
    const QVariantMap shelf = selectedShelfData();
    const QVariantList bookIds = shelf.value("bookIds").toList();
    const QVariantList bookNames = shelf.value("bookNames").toList();

    if (bookIds.size() != bookNames.size()) {
        ui->statusLabel->setText("اطلاعات کتاب های قفسه با پاسخ سرور تطبیق ندارد");
        applyingServerData = false;
        return;
    }

    for (int i = 0; i < bookIds.size(); ++i) {
        const int bookId = bookIds.at(i).toInt();
        const QString bookName = bookNames.at(i).toString().trimmed();
        if (bookId <= 0 || bookName.isEmpty()) {
            ui->statusLabel->setText("نام یکی از کتاب های قفسه از جدول Books دریافت نشد");
            continue;
        }
        QListWidgetItem *item = new QListWidgetItem(bookName);
        item->setData(Qt::UserRole, bookId);
        ui->shelfBookListWidget->addItem(item);
    }
    applyingServerData = false;
}

void LibraryWindow_c::onCreateShelfButtonClicked()
{
    libraryController->createShelf(ui->newShelfNameLineEdit->text().trimmed());
}

void LibraryWindow_c::onShelfCreated(int shelfId, const QString &message)
{
    Q_UNUSED(shelfId)
    ui->statusLabel->setText(message);
    ui->newShelfNameLineEdit->clear();
    libraryController->refreshShelves();
}

void LibraryWindow_c::onShelfCreateFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onRenameShelfButtonClicked()
{
    const int shelfId = getSelectedShelfId();
    if (shelfId <= 0) {
        ui->statusLabel->setText("ابتدا یک قفسه را انتخاب کنید");
        return;
    }
    bool ok = false;
    const QString newName = QInputDialog::getText(
        this, "تغییر نام قفسه", "نام جدید:", QLineEdit::Normal, QString(), &ok);
    if (ok && !newName.trimmed().isEmpty())
        libraryController->renameShelf(shelfId, newName.trimmed());
}

void LibraryWindow_c::onShelfRenamed(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::onShelfRenameFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onDeleteShelfButtonClicked()
{
    const int shelfId = getSelectedShelfId();
    if (shelfId <= 0) {
        ui->statusLabel->setText("ابتدا یک قفسه را انتخاب کنید");
        return;
    }
    if (QMessageBox::question(this, "حذف قفسه", "مطمئن هستید؟") == QMessageBox::Yes)
        libraryController->deleteShelf(shelfId);
}

void LibraryWindow_c::onShelfDeleted(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::onShelfDeleteFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onAddBookToShelfButtonClicked()
{
    const int shelfId = getSelectedShelfId();
    const int bookId = ui->addBookComboBox->currentData().toInt();
    if (shelfId <= 0 || bookId <= 0) {
        ui->statusLabel->setText("ابتدا قفسه و کتاب را انتخاب کنید");
        return;
    }
    libraryController->addBookToShelf(shelfId, bookId);
}

void LibraryWindow_c::onBookAddedToShelf(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::onBookAddToShelfFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onRemoveBookFromShelfButtonClicked()
{
    const int shelfId = getSelectedShelfId();
    QListWidgetItem *item = ui->shelfBookListWidget->currentItem();
    if (shelfId <= 0 || !item) {
        ui->statusLabel->setText("ابتدا قفسه و کتاب را انتخاب کنید");
        return;
    }
    libraryController->removeBookFromShelf(shelfId, item->data(Qt::UserRole).toInt());
}

void LibraryWindow_c::onBookRemovedFromShelf(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::onBookRemoveFromShelfFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onShelvesRowsMoved(const QModelIndex &, int, int,
                                         const QModelIndex &, int)
{
    if (applyingServerData)
        return;
    syncCurrentShelvesToWidgetOrder();
    libraryController->reorderShelves(itemIds(ui->shelvesListWidget));
}

void LibraryWindow_c::onShelfBooksRowsMoved(const QModelIndex &, int, int,
                                            const QModelIndex &, int)
{
    if (applyingServerData)
        return;
    const int shelfId = getSelectedShelfId();
    if (shelfId > 0)
        libraryController->reorderShelfBooks(shelfId, itemIds(ui->shelfBookListWidget));
}

void LibraryWindow_c::onShelvesReordered(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::onShelvesReorderFailed(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::onShelfBooksReordered(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::onShelfBooksReorderFailed(const QString &message)
{
    ui->statusLabel->setText(message);
    libraryController->refreshShelves();
}

void LibraryWindow_c::populateSavedBooksList()
{
    applyingServerData = true;
    ui->saveBooksListWidget->clear();
    for (const QVariant &value : currentSavedBooks) {
        const QVariantMap book = value.toMap();
        const int bookId = book.value("bookId").toInt();
        const QString bookName = book.value("bookName").toString().trimmed();
        if (bookId <= 0 || bookName.isEmpty()) {
            ui->statusLabel->setText("نام کتاب ذخیره شده از جدول Books دریافت نشد");
            continue;
        }
        QListWidgetItem *item = new QListWidgetItem(bookName);
        item->setData(Qt::UserRole, bookId);
        ui->saveBooksListWidget->addItem(item);
    }
    applyingServerData = false;
}

void LibraryWindow_c::populateFavoriteBooksList()
{
    applyingServerData = true;
    ui->favoriteBooksListWidget->clear();
    for (const QVariant &value : currentFavoriteBooks) {
        const QVariantMap book = value.toMap();
        const int bookId = book.value("bookId").toInt();
        const QString bookName = book.value("bookName").toString().trimmed();
        if (bookId <= 0 || bookName.isEmpty()) {
            ui->statusLabel->setText("نام کتاب علاقه مندی از جدول Books دریافت نشد");
            continue;
        }
        QListWidgetItem *item = new QListWidgetItem(bookName);
        item->setData(Qt::UserRole, bookId);
        ui->favoriteBooksListWidget->addItem(item);
    }
    applyingServerData = false;
}

void LibraryWindow_c::populateFavoriteBookCombo()
{
    QSet<int> favoriteIds;
    for (const QVariant &value : currentFavoriteBooks)
        favoriteIds.insert(value.toMap().value("bookId").toInt());

    ui->favoriteBooksComboBox->clear();
    for (const QVariant &value : currentSavedBooks) {
        const QVariantMap book = value.toMap();
        const int bookId = book.value("bookId").toInt();
        const QString bookName = book.value("bookName").toString().trimmed();
        if (bookId > 0 && !bookName.isEmpty() && !favoriteIds.contains(bookId))
            ui->favoriteBooksComboBox->addItem(bookName, bookId);
    }
    ui->addFavoriteBookButton->setEnabled(ui->favoriteBooksComboBox->count() > 0);
}

void LibraryWindow_c::onSavedBooksLoaded(const QVariantList &books)
{
    currentSavedBooks = books;
    populateSavedBooksList();
    populateFavoriteBookCombo();
}

void LibraryWindow_c::onSavedBooksLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onRemoveSavedBookButtonClicked()
{
    QListWidgetItem *item = ui->saveBooksListWidget->currentItem();
    if (!item) {
        ui->statusLabel->setText("ابتدا یک کتاب ذخیره شده را انتخاب کنید");
        return;
    }
    savedBookController->unsaveBook(item->data(Qt::UserRole).toInt());
}

void LibraryWindow_c::onBookUnsaved(const QString &message)
{
    ui->statusLabel->setText(message);
    savedBookController->refreshSavedBooks();
    savedBookController->refreshFavoriteBooks();
}

void LibraryWindow_c::onBookUnsaveFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onFavoriteBooksLoaded(const QVariantList &books)
{
    currentFavoriteBooks = books;
    populateFavoriteBooksList();
    populateFavoriteBookCombo();
}

void LibraryWindow_c::onFavoriteBooksLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onAddFavoriteBookButtonClicked()
{
    savedBookController->addFavoriteBook(ui->favoriteBooksComboBox->currentData().toInt());
}

void LibraryWindow_c::onRemoveFavoriteBookButtonClicked()
{
    QListWidgetItem *item = ui->favoriteBooksListWidget->currentItem();
    if (!item) {
        ui->statusLabel->setText("ابتدا یک کتاب را از لیست علاقه مندی انتخاب کنید");
        return;
    }
    savedBookController->removeFavoriteBook(item->data(Qt::UserRole).toInt());
}

void LibraryWindow_c::onFavoriteBookAdded(const QString &message)
{
    ui->statusLabel->setText(message);
    savedBookController->refreshFavoriteBooks();
}

void LibraryWindow_c::onFavoriteBookAddFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onFavoriteBookRemoved(const QString &message)
{
    ui->statusLabel->setText(message);
    savedBookController->refreshFavoriteBooks();
}

void LibraryWindow_c::onFavoriteBookRemoveFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onFavoriteBooksRowsMoved(const QModelIndex &, int, int,
                                               const QModelIndex &, int)
{
    if (applyingServerData)
        return;
    savedBookController->reorderFavoriteBooks(itemIds(ui->favoriteBooksListWidget));
}

void LibraryWindow_c::onFavoriteBooksReordered(const QString &message)
{
    ui->statusLabel->setText(message);
    savedBookController->refreshFavoriteBooks();
}

void LibraryWindow_c::onFavoriteBooksReorderFailed(const QString &message)
{
    ui->statusLabel->setText(message);
    savedBookController->refreshFavoriteBooks();
}

void LibraryWindow_c::onValidationError(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onBackButtonClicked()
{
    emit backRequested();
}

void LibraryWindow_c::onPurchasedBooksLoaded(const QVariantList &bookIds)
{
    ui->myBooksListWidget->clear();
    for (const QVariant &value : bookIds) {
        const QVariantMap bookData = value.toMap();
        const int bookId = bookData.value("bookId").toInt();
        const QString bookName = bookData.value("bookName").toString();
        if (bookId <= 0 || bookName.trimmed().isEmpty())
            continue;
        QListWidgetItem *item = new QListWidgetItem(bookName);
        item->setData(Qt::UserRole, bookId);
        ui->myBooksListWidget->addItem(item);
    }
    populateAddBookCombo();
}

void LibraryWindow_c::populateAddBookCombo()
{
    ui->addBookComboBox->clear();
    for (int i = 0; i < ui->myBooksListWidget->count(); ++i) {
        QListWidgetItem *item = ui->myBooksListWidget->item(i);
        ui->addBookComboBox->addItem(item->text(), item->data(Qt::UserRole).toInt());
    }
}

void LibraryWindow_c::onViewBookDetailButtonClicked()
{
    QListWidgetItem *item = ui->myBooksListWidget->currentItem();
    if (!item) {
        ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید");
        return;
    }
    BookDetailsWindow_c *detailsWindow =
        new BookDetailsWindow_c(networkManager, item->data(Qt::UserRole).toInt());
    detailsWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(detailsWindow, &BookDetailsWindow_c::backRequested,
            detailsWindow, &QWidget::close);
    connect(detailsWindow, &QObject::destroyed, this, [this]() { this->show(); });
    showFollowingState(detailsWindow, this);
    this->hide();
}

void LibraryWindow_c::onPurchasedBooksLoadFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void LibraryWindow_c::onOpenBookButtonClicked()
{
    QListWidgetItem *item = ui->myBooksListWidget->currentItem();
    if (!item) {
        ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید");
        return;
    }
    BookReaderWindow_c *readerWindow =
        new BookReaderWindow_c(networkManager, item->data(Qt::UserRole).toInt());
    readerWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(readerWindow, &BookReaderWindow_c::backRequested,
            readerWindow, &QWidget::close);
    connect(readerWindow, &QObject::destroyed, this, [this]() { this->show(); });
    showFollowingState(readerWindow, this);
    this->hide();
}
