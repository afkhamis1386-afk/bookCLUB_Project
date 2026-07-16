#include "librarywindow_c.h"
#include "ui_librarywindow_c.h"
#include "bookdetailswindow_c.h"
#include "bookreaderwindow_c.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QListWidgetItem>

LibraryWindow_c::LibraryWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LibraryWindow_c)
    , networkManager(networkManager)
    , libraryController(new LibraryController(networkManager, this))
    , savedBookController(new SavedBookController(networkManager, this))
{
    ui->setupUi(this);
    connect(ui->createShelfButton, &QPushButton::clicked, this, &LibraryWindow_c::onCreateShelfButtonClicked);
    connect(ui->renameShelfButton, &QPushButton::clicked, this, &LibraryWindow_c::onRenameShelfButtonClicked);
    connect(ui->deleteShelfButton, &QPushButton::clicked, this, &LibraryWindow_c::onDeleteShelfButtonClicked);
    connect(ui->moveBookButton, &QPushButton::clicked, this, &LibraryWindow_c::onMoveBookButtonClicked);
    connect(ui->shelvesListWidget, &QListWidget::currentRowChanged, this, &LibraryWindow_c::onShelfSelectionChanged);
    connect(ui->removeSavedBookButton, &QPushButton::clicked, this, &LibraryWindow_c::onRemoveSavedBookButtonClicked);
    connect(ui->viewSavedBookButton, &QPushButton::clicked, this, &LibraryWindow_c::onViewSavedBookButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &LibraryWindow_c::onBackButtonClicked);
    connect(libraryController, &LibraryController::shelvesLoaded, this, &LibraryWindow_c::onShelvesLoaded);
    connect(libraryController, &LibraryController::shelvesLoadFailed, this, &LibraryWindow_c::onShelvesLoadFailed);
    connect(libraryController, &LibraryController::shelfCreated, this, &LibraryWindow_c::onShelfCreated);
    connect(libraryController, &LibraryController::shelfCreateFailed, this, &LibraryWindow_c::onShelfCreateFailed);
    connect(libraryController, &LibraryController::shelfRenamed, this, &LibraryWindow_c::onShelfRenamed);
    connect(libraryController, &LibraryController::shelfRenameFailed, this, &LibraryWindow_c::onShelfRenameFailed);
    connect(libraryController, &LibraryController::shelfDeleted, this, &LibraryWindow_c::onShelfDeleted);
    connect(libraryController, &LibraryController::shelfDeleteFailed, this, &LibraryWindow_c::onShelfDeleteFailed);
    connect(libraryController, &LibraryController::bookMoved, this, &LibraryWindow_c::onBookMoved);
    connect(libraryController, &LibraryController::bookMoveFailed, this, &LibraryWindow_c::onBookMoveFailed);
    connect(libraryController, &LibraryController::validationError, this, &LibraryWindow_c::onValidationError);
    connect(savedBookController, &SavedBookController::savedBooksLoaded, this, &LibraryWindow_c::onSavedBooksLoaded);
    connect(savedBookController, &SavedBookController::savedBooksLoadFailed, this, &LibraryWindow_c::onSavedBooksLoadFailed);
    connect(savedBookController, &SavedBookController::bookUnsaved, this, &LibraryWindow_c::onBookUnsaved);
    connect(savedBookController, &SavedBookController::bookUnsaveFailed, this, &LibraryWindow_c::onBookUnsaveFailed);
    libraryController->refreshShelves();
    savedBookController->refreshSavedBooks();
    connect(ui->openBookButton, &QPushButton::clicked, this, &LibraryWindow_c::onOpenBookButtonClicked);
    connect(libraryController, &LibraryController::purchasedBooksLoaded, this, &LibraryWindow_c::onPurchasedBooksLoaded);
    connect(libraryController, &LibraryController::purchasedBooksLoadFailed, this, &LibraryWindow_c::onPurchasedBooksLoadFailed);
    libraryController->refreshPurchasedBooks();
}

LibraryWindow_c::~LibraryWindow_c()
{
    delete ui;
}

void LibraryWindow_c::populateShelvesList(const QVariantList &shelves)
{
    currentShelves = shelves;
    ui->shelvesListWidget->clear();
    for (const QVariant &v : shelves) {
        QVariantMap shelf = v.toMap();
        QString text = QString("%1 (%2 کتاب)").arg(shelf.value("shelfName").toString()).arg(shelf.value("bookCount").toInt());
        QListWidgetItem *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, shelf.value("shelfId").toInt());
        ui->shelvesListWidget->addItem(item);
    }
    populateMoveToCombo();
    ui->shelfBookListWidget->clear();
}
void LibraryWindow_c::populateMoveToCombo()
{
    ui->moveToShelfComboBox->clear();
    for (const QVariant &v : currentShelves) {
        QVariantMap shelf = v.toMap();
        ui->moveToShelfComboBox->addItem(shelf.value("shelfName").toString(), shelf.value("shelfId").toInt());
    }
}

int LibraryWindow_c::getSelectedShelfId() const
{
    int row = ui->shelvesListWidget->currentRow();
    if (row < 0) return -1;
    return ui->shelvesListWidget->item(row)->data(Qt::UserRole).toInt();
}

void LibraryWindow_c::onShelvesLoaded(const QVariantList &shelves) { populateShelvesList(shelves); }
void LibraryWindow_c::onShelvesLoadFailed(const QString &message) { ui->statusLabel->setText(message); }

void LibraryWindow_c::onShelfSelectionChanged()
{
    int row = ui->shelvesListWidget->currentRow();
    ui->shelfBookListWidget->clear();
    if (row < 0 || row >= currentShelves.size()) return;

    QVariantMap shelf = currentShelves[row].toMap();
    QVariantList bookIds = shelf.value("bookIds").toList();
    for (const QVariant &v : bookIds)
        ui->shelfBookListWidget->addItem(QString("کتاب #%1").arg(v.toInt()));
}

void LibraryWindow_c::onCreateShelfButtonClicked()
{
    QString name = ui->newShelfNameLineEdit->text().trimmed();
    libraryController->createShelf(name);
}

void LibraryWindow_c::onShelfCreated(int shelfId, const QString &message)
{
    Q_UNUSED(shelfId)
    ui->statusLabel->setText(message);
    ui->newShelfNameLineEdit->clear();
    libraryController->refreshShelves();
}
void LibraryWindow_c::onShelfCreateFailed(const QString &message) { ui->statusLabel->setText(message); }

void LibraryWindow_c::onRenameShelfButtonClicked()
{
    int shelfId = getSelectedShelfId();
    if (shelfId <= 0) {
        ui->statusLabel->setText("ابتدا یک قفسه را انتخاب کنید");
        return;
    }
    bool ok;
    QString newName = QInputDialog::getText(this, "تغییر نام قفسه", "نام جدید:", QLineEdit::Normal, "", &ok);
    if (ok && !newName.trimmed().isEmpty())
        libraryController->renameShelf(shelfId, newName.trimmed());
}
void LibraryWindow_c::onShelfRenamed(const QString &message) { ui->statusLabel->setText(message); libraryController->refreshShelves(); }
void LibraryWindow_c::onShelfRenameFailed(const QString &message) { ui->statusLabel->setText(message); }

void LibraryWindow_c::onDeleteShelfButtonClicked()
{
    int shelfId = getSelectedShelfId();
    if (shelfId <= 0) {
        ui->statusLabel->setText("ابتدا یک قفسه را انتخاب کنید");
        return;
    }
    if (QMessageBox::question(this, "حذف قفسه", "مطمئن هستید؟") == QMessageBox::Yes)
        libraryController->deleteShelf(shelfId);
}
void LibraryWindow_c::onShelfDeleted(const QString &message) { ui->statusLabel->setText(message); libraryController->refreshShelves(); }
void LibraryWindow_c::onShelfDeleteFailed(const QString &message) { ui->statusLabel->setText(message); }

void LibraryWindow_c::onMoveBookButtonClicked()
{
    int sourceShelfId = getSelectedShelfId();
    int bookRow = ui->shelfBookListWidget->currentRow();
    int destShelfId = ui->moveToShelfComboBox->currentData().toInt();

    if (sourceShelfId <= 0 || bookRow < 0) {
        ui->statusLabel->setText("ابتدا قفسه و کتاب را انتخاب کنید");
        return;
    }
    QString itemText = ui->shelfBookListWidget->item(bookRow)->text();
    int bookId = itemText.section('#', 1).toInt();

    libraryController->moveBookBetweenShelves(sourceShelfId, destShelfId, bookId);
}
void LibraryWindow_c::onBookMoved(const QString &message) { ui->statusLabel->setText(message); libraryController->refreshShelves(); }
void LibraryWindow_c::onBookMoveFailed(const QString &message) { ui->statusLabel->setText(message); }
void LibraryWindow_c::onSavedBooksLoaded(const QVariantList &bookIds)
{
    ui->saveBooksListWidget->clear();
    for (const QVariant &v : bookIds) {
        QListWidgetItem *item = new QListWidgetItem(QString("کتاب #%1").arg(v.toInt()));
        item->setData(Qt::UserRole, v.toInt());
        ui->saveBooksListWidget->addItem(item);
    }
}
void LibraryWindow_c::onSavedBooksLoadFailed(const QString &message) { ui->statusLabel->setText(message); }

void LibraryWindow_c::onRemoveSavedBookButtonClicked()
{
    int row = ui->saveBooksListWidget->currentRow();
    if (row < 0) {
        ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید");
        return;
    }
    int bookId = ui->saveBooksListWidget->item(row)->data(Qt::UserRole).toInt();
    savedBookController->unsaveBook(bookId);
}
void LibraryWindow_c::onBookUnsaved(const QString &message) { ui->statusLabel->setText(message); savedBookController->refreshSavedBooks(); }
void LibraryWindow_c::onBookUnsaveFailed(const QString &message) { ui->statusLabel->setText(message); }

void LibraryWindow_c::onViewSavedBookButtonClicked()
{
    int row = ui->saveBooksListWidget->currentRow();
    if (row < 0) return;
    int bookId = ui->saveBooksListWidget->item(row)->data(Qt::UserRole).toInt();
    BookDetailsWindow_c *detailsWindow = new BookDetailsWindow_c(networkManager, bookId);
    detailsWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(detailsWindow, &BookDetailsWindow_c::backRequested, detailsWindow, &QWidget::close);
    detailsWindow->show();
}
void LibraryWindow_c::onValidationError(const QString &message) { ui->statusLabel->setText(message); }
void LibraryWindow_c::onBackButtonClicked() { emit backRequested(); }
void LibraryWindow_c::onPurchasedBooksLoaded(const QVariantList &bookIds)
{
    ui->myBooksListWidget->clear();
    for (const QVariant &v : bookIds) {
        QListWidgetItem *item = new QListWidgetItem(QString("کتاب #%1").arg(v.toInt()));
        item->setData(Qt::UserRole, v.toInt());
        ui->myBooksListWidget->addItem(item);
    }
}
void LibraryWindow_c::onPurchasedBooksLoadFailed(const QString &message) { ui->statusLabel->setText(message); }
void LibraryWindow_c::onOpenBookButtonClicked()
{
    int row = ui->myBooksListWidget->currentRow();
    if (row < 0) {
        ui->statusLabel->setText("ابتدا یک کتاب را انتخاب کنید");
        return;
    }
    int bookId = ui->myBooksListWidget->item(row)->data(Qt::UserRole).toInt();
    BookReaderWindow_c *readerWindow = new BookReaderWindow_c(networkManager, bookId);
    readerWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(readerWindow, &BookReaderWindow_c::backRequested, readerWindow, &QWidget::close);
    readerWindow->show();
}

