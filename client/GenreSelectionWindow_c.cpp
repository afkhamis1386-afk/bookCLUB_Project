#include "genreselectionwindow_c.h"
#include "ui_GenreSelectionWindow_c.h"
#include <QMessageBox>

GenreSelectionWindow_c::GenreSelectionWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GenreSelectionWindow_c)
    , networkManager(networkManager)
    , profileController(new ProfileController(networkManager, this)) {
    ui->setupUi(this);
    connect(ui->genresListWidget, &QListWidget::itemChanged, this, &GenreSelectionWindow_c::onListItemChanged);
    connect(ui->confirmButton, &QPushButton::clicked, this, &GenreSelectionWindow_c::onConfirmButtonClicked);
    connect(profileController, &ProfileController::genresLoaded, this, &GenreSelectionWindow_c::onGenresLoaded);
    connect(profileController, &ProfileController::genresLoadFailed, this, &GenreSelectionWindow_c::onGenresLoadFailed);
    connect(profileController, &ProfileController::favoriteGenresSaved, this, &GenreSelectionWindow_c::onFavoriteGenresSaved);
    connect(profileController, &ProfileController::favoriteGenresSaveFailed, this, &GenreSelectionWindow_c::onFavoriteGenresSaveFailed);
    connect(profileController, &ProfileController::validationError, this, &GenreSelectionWindow_c::onValidationError);

    profileController->loadGenres();
}
GenreSelectionWindow_c::~GenreSelectionWindow_c() {
    delete ui;
}
int GenreSelectionWindow_c::countCheckedItems() const {
    int count = 0;
    for (int i = 0; i < ui->genresListWidget->count(); ++i) {
        if (ui->genresListWidget->item(i)->checkState() == Qt::Checked)
            count++;
    }
    return count;
}
void GenreSelectionWindow_c::onGenresLoaded(const QVariantList &genres) {
    ui->genresListWidget->clear();
    for (const QVariant &v : genres) {
        QVariantMap genreMap = v.toMap();
        QListWidgetItem *item = new QListWidgetItem(genreMap.value("genreTitle").toString());
        item->setData(Qt::UserRole, genreMap.value("genreId").toInt());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        ui->genresListWidget->addItem(item);
    }
}
void GenreSelectionWindow_c::onGenresLoadFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void GenreSelectionWindow_c::onListItemChanged(QListWidgetItem *item)
{
    if (item->checkState() == Qt::Checked && countCheckedItems() > 3) {
        item->setCheckState(Qt::Unchecked);
        ui->statusLabel->setText("حداکثر ۳ ژانر می توانید انتخاب کنید");
    }
}
void GenreSelectionWindow_c::onConfirmButtonClicked() {
    ui->statusLabel->clear();
    QVector<int> selectedGenreIds;
    for (int i = 0; i < ui->genresListWidget->count(); ++i) {
        QListWidgetItem *item = ui->genresListWidget->item(i);
        if (item->checkState() == Qt::Checked)
            selectedGenreIds.append(item->data(Qt::UserRole).toInt());
    }
    profileController->setFavoriteGenres(selectedGenreIds);
}
void GenreSelectionWindow_c::onFavoriteGenresSaved(const QString &message) {
    QMessageBox::information(this, "ثبت شد", message);
    emit genresConfirmed();
}
void GenreSelectionWindow_c::onFavoriteGenresSaveFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void GenreSelectionWindow_c::onValidationError(const QString &message) {
    ui->statusLabel->setText(message);
}
