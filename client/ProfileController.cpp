#include "ProfileController.h"

ProfileController::ProfileController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager) {
    connect(networkManager, &NetworkManager::responseReceived, this, &ProfileController::onResponseReceived);
}
void ProfileController::loadGenres() {
    if (!networkManager->isConnected()) { emit genresLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getAllGenres();
}
void ProfileController::loadCategories() {
    if (!networkManager->isConnected()) { emit categoriesLoadFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->getAllCategories();
}
void ProfileController::setFavoriteGenres(const QVector<int> &genreIds) {
    if (genreIds.isEmpty() || genreIds.size() > 3) {
        emit validationError("باید بین ۱ تا ۳ ژانر انتخاب کنید");
        return;
    }
    if (!networkManager->isConnected()) { emit favoriteGenresSaveFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->setFavoriteGenres(genreIds);
}
void ProfileController::changePassword(const QString &oldPassword, const QString &newPassword) {
    if (oldPassword.isEmpty() || newPassword.isEmpty()) {
        emit validationError("رمز عبور فعلی و جدید را وارد کنید");
        return;
    }
    if (!networkManager->isConnected()) { emit passwordChangeFailed("اتصال به سرور برقرار نیست"); return; }
    networkManager->changePassword(oldPassword, newPassword);
}
void ProfileController::onResponseReceived(RequestType type, const Response &response) {
    switch (type) {
    case RequestType::GetAllGenres:
        if (response.isSuccess()) emit genresLoaded(response.getData().value("genres").toList());
        else emit genresLoadFailed(response.getMessage());
        break;
    case RequestType::GetAllCategories:
        if (response.isSuccess()) emit categoriesLoaded(response.getData().value("categories").toList());
        else emit categoriesLoadFailed(response.getMessage());
        break;
    case RequestType::SetFavoriteGenres:
        if (response.isSuccess()) emit favoriteGenresSaved(response.getMessage());
        else emit favoriteGenresSaveFailed(response.getMessage());
        break;
    case RequestType::ChangePassword:
        if (response.isSuccess()) emit passwordChanged(response.getMessage());
        else emit passwordChangeFailed(response.getMessage());
        break;
    default:
        break;
    }
}