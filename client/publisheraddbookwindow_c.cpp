#include "publisheraddbookwindow_c.h"
#include "ui_publisheraddbookwindow_c.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>

PublisherAddBookWindow_c::PublisherAddBookWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PublisherAddBookWindow_c)
    , networkManager(networkManager)
    , publisherBookController(new PublisherBookController(networkManager, this))
    , profileController(new ProfileController(networkManager, this)) {
    ui->setupUi(this);
    ui->descriptionTextEdit->setPlaceholderText("توضیحات کتاب");
    ui->priceDoubleSpinBox->setSuffix(" تومان");
    connect(ui->selectCoverButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onSelectCoverButtonClicked);
    connect(ui->selectPdfButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onSelectPdfButtonClicked);
    connect(ui->submitButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onSubmitButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onBackButtonClicked);
    connect(publisherBookController, &PublisherBookController::bookAdded, this, &PublisherAddBookWindow_c::onBookAdded);
    connect(publisherBookController, &PublisherBookController::bookAddFailed, this, &PublisherAddBookWindow_c::onBookAddFailed);
    connect(publisherBookController, &PublisherBookController::bookFileReadFailed, this, &PublisherAddBookWindow_c::onBookFileReadFailed);
    connect(publisherBookController, &PublisherBookController::validationError, this, &PublisherAddBookWindow_c::onValidationError);
    connect(profileController, &ProfileController::genresLoaded, this, &PublisherAddBookWindow_c::onGenresLoaded);
    connect(profileController, &ProfileController::genresLoadFailed, this, &PublisherAddBookWindow_c::onGenresLoadFailed);
    profileController->loadGenres();
}

PublisherAddBookWindow_c::~PublisherAddBookWindow_c() {
    delete ui;
}
void PublisherAddBookWindow_c::onSelectCoverButtonClicked() {
    QString path = QFileDialog::getOpenFileName(this, "انتخاب عکس جلد", QString(), "تصاویر (*.jpg *.jpeg *.png)");
    if (!path.isEmpty()) {
        selectedCoverPath = path;
        ui->coverPathLabel->setText(QFileInfo(path).fileName());
    }
}
void PublisherAddBookWindow_c::onSelectPdfButtonClicked() {
    QString path = QFileDialog::getOpenFileName(this, "انتخاب فایل PDF", QString(), "فایل PDF (*.pdf)");
    if (!path.isEmpty()) {
        selectedPdfPath = path;
        ui->pdfPathLabel->setText(QFileInfo(path).fileName());
    }
}
void PublisherAddBookWindow_c::onSubmitButtonClicked() {
    ui->statusLabel->clear();
    if (ui->genreComboBox->count() == 0) {
        ui->statusLabel->setText("لیست ژانرها هنوز بارگذاری نشده، کمی صبر کنید یا اتصال به سرور را بررسی کنید");
        return;
    }
    publisherBookController->addBook(
        ui->bookNameLineEdit->text(),
        ui->descriptionTextEdit->toPlainText(),
        ui->priceDoubleSpinBox->value(),
        ui->genreComboBox->currentText(),
        ui->categoryTitleLineEdit->text(),
        ui->authorNameLineEdit->text(),
        selectedCoverPath,
        selectedPdfPath
        );
}
void PublisherAddBookWindow_c::onBookAdded(int bookId, const QString &message) {
    Q_UNUSED(bookId)
    QMessageBox::information(this, "ثبت موفق", message);
    ui->bookNameLineEdit->clear();
    ui->descriptionTextEdit->clear();
    ui->priceDoubleSpinBox->setValue(0);
    ui->genreComboBox->setCurrentIndex(0);
    ui->categoryTitleLineEdit->clear();
    ui->authorNameLineEdit->clear();
    ui->coverPathLabel->setText("فایلی انتخاب نشده");
    ui->pdfPathLabel->setText("فایلی انتخاب نشده");
    selectedCoverPath.clear();
    selectedPdfPath.clear();
}
void PublisherAddBookWindow_c::onBookAddFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherAddBookWindow_c::onBookFileReadFailed(const QString &message) { ui->statusLabel->setText(message); }
void PublisherAddBookWindow_c::onValidationError(const QString &message) { ui->statusLabel->setText(message); }
void PublisherAddBookWindow_c::onBackButtonClicked() { emit backRequested(); }
void PublisherAddBookWindow_c::onGenresLoaded(const QVariantList &genres) {
    ui->genreComboBox->clear();
    for (const QVariant &g : genres) {
        ui->genreComboBox->addItem(g.toMap().value("genreTitle").toString());
    }
}
void PublisherAddBookWindow_c::onGenresLoadFailed(const QString &message) {
    ui->statusLabel->setText("خطا در بارگذاری لیست ژانرها: " + message);
}
