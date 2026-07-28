#include "publisheraddbookwindow_c.h"
#include "ui_publisheraddbookwindow_c.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

PublisherAddBookWindow_c::PublisherAddBookWindow_c(NetworkManager *networkManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PublisherAddBookWindow_c)
    , networkManager(networkManager)
    , publisherBookController(new PublisherBookController(networkManager, this))
    , profileController(new ProfileController(networkManager, this)) {
    ui->setupUi(this);
    setupCommon();
}
PublisherAddBookWindow_c::PublisherAddBookWindow_c(NetworkManager *networkManager, int editBookId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PublisherAddBookWindow_c)
    , networkManager(networkManager)
    , publisherBookController(new PublisherBookController(networkManager, this))
    , profileController(new ProfileController(networkManager, this)) {
    ui->setupUi(this);
    isEditMode = true;
    editingBookId = editBookId;
    setupCommon();
    setWindowTitle("ویرایش کتاب");
    ui->submitButton->setText("ذخیره تغییرات");
    ui->coverPathLabel->setText("برای تغییر عکس جلد انتخاب کنید (در غیر این صورت حفظ می شود)");
    ui->pdfPathLabel->setText("برای تغییر فایل PDF انتخاب کنید (در غیر این صورت حفظ می شود)");
    ui->statusLabel->setText("در حال بارگذاری اطلاعات کتاب...");
    connect(publisherBookController, &PublisherBookController::bookDetailsForEditLoaded, this, &PublisherAddBookWindow_c::onBookDetailsForEditLoaded);
    connect(publisherBookController, &PublisherBookController::bookDetailsForEditLoadFailed, this, &PublisherAddBookWindow_c::onBookDetailsForEditLoadFailed);
    connect(publisherBookController, &PublisherBookController::bookUpdated, this, &PublisherAddBookWindow_c::onBookUpdated);
    connect(publisherBookController, &PublisherBookController::bookUpdateFailed, this, &PublisherAddBookWindow_c::onBookUpdateFailed);
    publisherBookController->loadBookForEdit(editingBookId);
}
void PublisherAddBookWindow_c::setupCommon() {
    ui->descriptionTextEdit->setPlaceholderText("توضیحات کتاب");
    ui->priceDoubleSpinBox->setSuffix(" تومان");
    ui->timedDiscountStartEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    ui->timedDiscountEndEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    connect(ui->selectCoverButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onSelectCoverButtonClicked);
    connect(ui->selectPdfButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onSelectPdfButtonClicked);
    connect(ui->submitButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onSubmitButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &PublisherAddBookWindow_c::onBackButtonClicked);
    connect(ui->noDiscountRadio, &QRadioButton::toggled, this, &PublisherAddBookWindow_c::onDiscountTypeToggled);
    connect(ui->normalDiscountRadio, &QRadioButton::toggled, this, &PublisherAddBookWindow_c::onDiscountTypeToggled);
    connect(ui->timedDiscountRadio, &QRadioButton::toggled, this, &PublisherAddBookWindow_c::onDiscountTypeToggled);
    connect(ui->noDiscountRadio, &QRadioButton::clicked, this, [this]() { discountSelectionChangedByUser = true; });
    connect(ui->normalDiscountRadio, &QRadioButton::clicked, this, [this]() { discountSelectionChangedByUser = true; });
    connect(ui->timedDiscountRadio, &QRadioButton::clicked, this, [this]() { discountSelectionChangedByUser = true; });
    connect(publisherBookController, &PublisherBookController::bookAdded, this, &PublisherAddBookWindow_c::onBookAdded);
    connect(publisherBookController, &PublisherBookController::bookAddFailed, this, &PublisherAddBookWindow_c::onBookAddFailed);
    connect(publisherBookController, &PublisherBookController::bookFileReadFailed, this, &PublisherAddBookWindow_c::onBookFileReadFailed);
    connect(publisherBookController, &PublisherBookController::validationError, this, &PublisherAddBookWindow_c::onValidationError);
    connect(profileController, &ProfileController::genresLoaded, this, &PublisherAddBookWindow_c::onGenresLoaded);
    connect(profileController, &ProfileController::genresLoadFailed, this, &PublisherAddBookWindow_c::onGenresLoadFailed);
    onDiscountTypeToggled();
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
void PublisherAddBookWindow_c::onDiscountTypeToggled() {
    bool normal = ui->normalDiscountRadio->isChecked();
    bool timed = ui->timedDiscountRadio->isChecked();
    ui->discountPercentSpinBox->setEnabled(normal || timed);
    ui->timedDiscountStartEdit->setEnabled(timed);
    ui->timedDiscountEndEdit->setEnabled(timed);
    if (!normal && !timed) {
        ui->discountPercentSpinBox->setValue(0);
    }
}
void PublisherAddBookWindow_c::applySelectedDiscount(int bookId) {
    bool normal = ui->normalDiscountRadio->isChecked();
    bool timed = ui->timedDiscountRadio->isChecked();
    double percent = ui->discountPercentSpinBox->value();
    const bool preserveAmountDiscount = isEditMode
                                        && originalDiscountAmount > 0.0
                                        && !discountSelectionChangedByUser
                                        && ui->noDiscountRadio->isChecked();
    if (isEditMode && !preserveAmountDiscount) {
        publisherBookController->applyDiscount(bookId, normal ? percent : 0, 0);
    }
    if (timed) {
        if (isEditMode && hadTimedDiscount) {
            publisherBookController->cancelTimedDiscount(bookId);
        }
        publisherBookController->applyTimedDiscount(bookId, percent, ui->timedDiscountStartEdit->dateTime(), ui->timedDiscountEndEdit->dateTime());
    } else if (isEditMode && hadTimedDiscount) {
        publisherBookController->cancelTimedDiscount(bookId);
    }
}
void PublisherAddBookWindow_c::onSubmitButtonClicked() {
    ui->statusLabel->clear();
    if (ui->genreComboBox->count() == 0) {
        ui->statusLabel->setText("لیست ژانرها هنوز بارگذاری نشده، کمی صبر کنید یا اتصال به سرور را بررسی کنید");
        return;
    }
    bool normal = ui->normalDiscountRadio->isChecked();
    bool timed = ui->timedDiscountRadio->isChecked();
    double percent = ui->discountPercentSpinBox->value();
    if (normal && percent <= 0) {
        ui->statusLabel->setText("درصد تخفیف عادی باید بیشتر از صفر باشد");
        return;
    }
    if (timed) {
        QDateTime start = ui->timedDiscountStartEdit->dateTime();
        QDateTime end = ui->timedDiscountEndEdit->dateTime();
        if (percent <= 0) {
            ui->statusLabel->setText("درصد تخفیف زمان دار باید بیشتر از صفر باشد");
            return;
        }
        if (start >= end) {
            ui->statusLabel->setText("تاریخ شروع باید قبل از تاریخ پایان باشد");
            return;
        }
        if (end <= QDateTime::currentDateTime()) {
            ui->statusLabel->setText("تاریخ پایان تخفیف باید در آینده باشد");
            return;
        }
    }
    if (!isEditMode) {
        double addDiscountPercent = normal ? percent : 0.0;
        publisherBookController->addBook(
            ui->bookNameLineEdit->text(),
            ui->descriptionTextEdit->toPlainText(),
            ui->priceDoubleSpinBox->value(),
            ui->genreComboBox->currentText(),
            ui->categoryTitleLineEdit->text(),
            ui->authorNameLineEdit->text(),
            selectedCoverPath,
            selectedPdfPath,
            addDiscountPercent
            );
    }
    else{
        publisherBookController->updateBook(
            editingBookId,
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
}
void PublisherAddBookWindow_c::onBookAdded(int bookId, const QString &message) {
    applySelectedDiscount(bookId);
    QMessageBox::information(this, "ثبت موفق", message);
    ui->bookNameLineEdit->clear();
    ui->descriptionTextEdit->clear();
    ui->priceDoubleSpinBox->setValue(0);
    ui->noDiscountRadio->setChecked(true);
    ui->discountPercentSpinBox->setValue(0);
    ui->timedDiscountStartEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    ui->timedDiscountEndEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
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
    if (pendingGenreSelection) {
        int idx = ui->genreComboBox->findText(pendingGenreTitle);
        if (idx >= 0) ui->genreComboBox->setCurrentIndex(idx);
        pendingGenreSelection = false;
    }
}
void PublisherAddBookWindow_c::onGenresLoadFailed(const QString &message) {
    ui->statusLabel->setText("خطا در بارگذاری لیست ژانرها: " + message);
}
void PublisherAddBookWindow_c::onBookUpdated(const QString &message) {
    applySelectedDiscount(editingBookId);
    QMessageBox::information(this, "ذخیره موفق", message);
    emit backRequested();
}
void PublisherAddBookWindow_c::onBookUpdateFailed(const QString &message) {
    ui->statusLabel->setText(message);
}
void PublisherAddBookWindow_c::onBookDetailsForEditLoaded(const QVariantMap &bookData) {
    ui->statusLabel->clear();
    ui->bookNameLineEdit->setText(bookData.value("bookName").toString());
    ui->descriptionTextEdit->setPlainText(bookData.value("description").toString());
    ui->priceDoubleSpinBox->setValue(bookData.value("price").toDouble());
    ui->categoryTitleLineEdit->setText(bookData.value("categoryTitle").toString());
    ui->authorNameLineEdit->setText(bookData.value("authorName").toString());

    const bool hasCoverImage = bookData.value("hasCoverImage").toBool()
                               || !bookData.value("coverImagePath").toString().trimmed().isEmpty();
    const bool hasPdfFile = bookData.value("hasPdfFile").toBool();
    ui->coverPathLabel->setText(hasCoverImage
                                    ? "عکس جلد فعلی ثبت شده است؛ برای جایگزینی، فایل جدید انتخاب کنید"
                                    : "عکس جلدی ثبت نشده است؛ برای افزودن، فایل انتخاب کنید");
    ui->pdfPathLabel->setText(hasPdfFile
                                  ? "فایل PDF فعلی ثبت شده است؛ برای جایگزینی، فایل جدید انتخاب کنید"
                                  : "فایل PDF فعلی ثبت نشده است؛ برای افزودن، فایل انتخاب کنید");

    pendingGenreTitle = bookData.value("genreTitle").toString();
    pendingGenreSelection = true;
    if (ui->genreComboBox->count() > 0) {
        int idx = ui->genreComboBox->findText(pendingGenreTitle);
        if (idx >= 0) ui->genreComboBox->setCurrentIndex(idx);
        pendingGenreSelection = false;
    }

    hadTimedDiscount = bookData.contains("timedDiscountId");
    double normalPercent = bookData.value("discountPercent").toDouble();
    originalDiscountAmount = bookData.value("discountAmount").toDouble();
    discountSelectionChangedByUser = false;
    if (hadTimedDiscount) {
        ui->timedDiscountRadio->setChecked(true);
        ui->discountPercentSpinBox->setValue(bookData.value("timedDiscountPercent").toDouble());
        ui->timedDiscountStartEdit->setDateTime(bookData.value("timedDiscountStart").toDateTime());
        ui->timedDiscountEndEdit->setDateTime(bookData.value("timedDiscountEnd").toDateTime());
    } else if (normalPercent > 0) {
        ui->normalDiscountRadio->setChecked(true);
        ui->discountPercentSpinBox->setValue(normalPercent);
    } else {
        ui->noDiscountRadio->setChecked(true);
        if (originalDiscountAmount > 0.0) {
            ui->statusLabel->setText(
                QString("این کتاب %1 تومان تخفیف مبلغی دارد. تا زمانی که نوع تخفیف را تغییر ندهید، مقدار فعلی حفظ می شود.")
                    .arg(originalDiscountAmount, 0, 'f', 2));
        }
    }
    onDiscountTypeToggled();
}
void PublisherAddBookWindow_c::onBookDetailsForEditLoadFailed(const QString &message) {
    ui->statusLabel->setText("خطا در بارگذاری اطلاعات کتاب: " + message);
}
