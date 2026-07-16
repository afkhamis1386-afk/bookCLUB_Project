#ifndef PROFILECONTROLLER_H
#define PROFILECONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "NetworkManager.h"

class ProfileController : public QObject {
    Q_OBJECT

public:
    explicit ProfileController(NetworkManager *networkManager, QObject *parent = nullptr);

    void loadGenres();
    void loadCategories();
    void setFavoriteGenres(const QVector<int> &genreIds);
    void changePassword(const QString &oldPassword, const QString &newPassword);
    void loadAccountInfo();
    void loadOrderHistory();
signals:
    void genresLoaded(const QVariantList &genres);
    void genresLoadFailed(const QString &message);
    void categoriesLoaded(const QVariantList &categories);
    void categoriesLoadFailed(const QString &message);
    void favoriteGenresSaved(const QString &message);
    void favoriteGenresSaveFailed(const QString &message);
    void passwordChanged(const QString &message);
    void passwordChangeFailed(const QString &message);
    void accountInfoLoaded(const QVariantMap &accountData);
    void accountInfoLoadFailed(const QString &message);
    void orderHistoryLoaded(const QVariantList &orders);
    void orderHistoryLoadFailed(const QString &message);
    void validationError(const QString &message);

private slots:
    void onResponseReceived(RequestType type, const Response &response);

private:
    NetworkManager *networkManager;
};

#endif // PROFILECONTROLLER_H