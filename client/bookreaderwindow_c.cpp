#include "bookreaderwindow_c.h"
#include "ui_bookreaderwindow_c.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPointF>
#include <QPdfPageNavigator>


BookReaderWindow_c::BookReaderWindow_c(NetworkManager *networkManager, int bookId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BookReaderWindow_c)
    , networkManager(networkManager)
    , bookId(bookId)
    , bookReaderController(new BookReaderController(networkManager, this))
    , pdfDocument(new QPdfDocument(this))
{
    ui->setupUi(this);
    pdfView = new QPdfView(ui->pdfViewContainer);
    pdfView->setDocument(pdfDocument);
    pdfView->setPageMode(QPdfView::PageMode::SinglePage);
    QVBoxLayout *containerLayout = new QVBoxLayout(ui->pdfViewContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(pdfView);
    connect(ui->prevPageButton, &QPushButton::clicked, this, &BookReaderWindow_c::onPrevPageButtonClicked);
    connect(ui->nextPageButton, &QPushButton::clicked, this, &BookReaderWindow_c::onNextPageButtonClicked);
    connect(ui->goToPageButton, &QPushButton::clicked, this, &BookReaderWindow_c::onGoToPageButtonClicked);
    connect(ui->zoomInButton, &QPushButton::clicked, this, &BookReaderWindow_c::onZoomInButtonClicked);
    connect(ui->zoomOutButton, &QPushButton::clicked, this, &BookReaderWindow_c::onZoomOutButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &BookReaderWindow_c::onBackButtonClicked);
    connect(bookReaderController, &BookReaderController::bookReady, this, &BookReaderWindow_c::onBookReady);
    connect(bookReaderController, &BookReaderController::bookOpenFailed, this, &BookReaderWindow_c::onBookOpenFailed);
    connect(bookReaderController, &BookReaderController::progressSaveFailed, this, &BookReaderWindow_c::onProgressSaveFailed);
    connect(pdfView->pageNavigator(), &QPdfPageNavigator::currentPageChanged, this, &BookReaderWindow_c::onCurrentPageChanged);
    ui->statusLabel->setText("در حال دریافت فایل از سرور...");
    bookReaderController->openBook(bookId);
}
BookReaderWindow_c::~BookReaderWindow_c()
{
    delete ui;
}

void BookReaderWindow_c::onBookReady(const QString &localFilePath, int startPage)
{
    ui->statusLabel->clear();
    QPdfDocument::Error err = pdfDocument->load(localFilePath);
    if (err != QPdfDocument::Error::None) {
        ui->statusLabel->setText("خطا در باز کردن فایل PDF");
        return;
    }
    pdfView->pageNavigator()->jump(startPage - 1, QPointF());
    updatePageInfoLabel();
}

void BookReaderWindow_c::onBookOpenFailed(const QString &message)
{
    ui->statusLabel->setText(message);
}

void BookReaderWindow_c::onProgressSaveFailed(const QString &message)
{
    Q_UNUSED(message)
}

void BookReaderWindow_c::onCurrentPageChanged(int page)
{
    updatePageInfoLabel();
    bookReaderController->updateCurrentPage(page + 1);
}

void BookReaderWindow_c::updatePageInfoLabel()
{
    int current = pdfView->pageNavigator()->currentPage() + 1;
    int total = pdfDocument->pageCount();
    ui->pageInfoLabel->setText(QString("صفحه %1 از %2").arg(current).arg(total));
}

void BookReaderWindow_c::onPrevPageButtonClicked()
{
    int current = pdfView->pageNavigator()->currentPage();
    if (current > 0)
        pdfView->pageNavigator()->jump(current - 1, QPointF());
}

void BookReaderWindow_c::onNextPageButtonClicked()
{
    int current = pdfView->pageNavigator()->currentPage();
    if (current < pdfDocument->pageCount() - 1)
        pdfView->pageNavigator()->jump(current + 1, QPointF());
}
void BookReaderWindow_c::onGoToPageButtonClicked()
{
    bool ok;
    int page = ui->pageNumberLineEdit->text().toInt(&ok);
    if (!ok || page < 1 || page > pdfDocument->pageCount()) {
        ui->statusLabel->setText("شماره صفحه نامعتبر است");
        return;
    }
    pdfView->pageNavigator()->jump(page - 1, QPointF());
}

void BookReaderWindow_c::onZoomInButtonClicked()
{
    pdfView->setZoomFactor(pdfView->zoomFactor() * 1.2);
}

void BookReaderWindow_c::onZoomOutButtonClicked()
{
    pdfView->setZoomFactor(pdfView->zoomFactor() / 1.2);
}

void BookReaderWindow_c::onBackButtonClicked()
{
    emit backRequested();
}
