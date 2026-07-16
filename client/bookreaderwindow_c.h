#ifndef BOOKREADERWINDOW_C_H
#define BOOKREADERWINDOW_C_H

#include <QMainWindow>
#include <QPdfDocument>
#include <QPdfView>
#include "NetworkManager.h"
#include "BookReaderController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class BookReaderWindow_c;
}
QT_END_NAMESPACE

class BookReaderWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit BookReaderWindow_c(NetworkManager *networkManager, int bookId, QWidget *parent = nullptr);
    ~BookReaderWindow_c() override;

signals:
    void backRequested();

private slots:
    void onBookReady(const QString &localFilePath, int startPage);
    void onBookOpenFailed(const QString &message);
    void onProgressSaveFailed(const QString &message);

    void onPrevPageButtonClicked();
    void onNextPageButtonClicked();
    void onGoToPageButtonClicked();
    void onZoomInButtonClicked();
    void onZoomOutButtonClicked();
    void onBackButtonClicked();

    void onCurrentPageChanged(int page);

private:
    Ui::BookReaderWindow_c *ui;
    NetworkManager *networkManager;
    int bookId;

    BookReaderController *bookReaderController;
    QPdfDocument *pdfDocument;
    QPdfView *pdfView;

    void updatePageInfoLabel();
};

#endif // BOOKREADERWINDOW_C_H
