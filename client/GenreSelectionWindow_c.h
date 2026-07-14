#ifndef GENRESELECTIONWINDOW_C_H
#define GENRESELECTIONWINDOW_C_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "NetworkManager.h"
#include "ProfileController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class GenreSelectionWindow_c;
}
QT_END_NAMESPACE

class GenreSelectionWindow_c : public QMainWindow
{
    Q_OBJECT

public:
    explicit GenreSelectionWindow_c(NetworkManager *networkManager, QWidget *parent = nullptr);
    ~GenreSelectionWindow_c() override;
signals:
    void genresConfirmed();
private slots:
    void onGenresLoaded(const QVariantList &genres);
    void onGenresLoadFailed(const QString &message);
    void onListItemChanged(QListWidgetItem *item);
    void onConfirmButtonClicked();
    void onFavoriteGenresSaved(const QString &message);
    void onFavoriteGenresSaveFailed(const QString &message);
    void onValidationError(const QString &message);
private:
    Ui::GenreSelectionWindow_c *ui;
    NetworkManager *networkManager;
    ProfileController *profileController;
    int countCheckedItems() const;
};

#endif // GENRESELECTIONWINDOW_C_H
