#include "PublisherDashboardController.h"

PublisherDashboardController::PublisherDashboardController(NetworkManager *networkManager, QObject *parent)
    : QObject(parent), networkManager(networkManager) {
    connect(networkManager, &NetworkManager::responseReceived, this, &PublisherDashboardController::onResponseReceived);
}
void PublisherDashboardController::refreshDashboard() {
    if (!networkManager->isConnected()) {
        emit dashboardLoadFailed("اتصال به سرور برقرار نیست");
        return;
    }
    networkManager->getPublisherStats();
}
void PublisherDashboardController::onResponseReceived(RequestType type, const Response &response) {
    if (type != RequestType::GetPublisherStats)
        return;
    if (response.isSuccess())
        emit dashboardLoaded(response.getData());
    else
        emit dashboardLoadFailed(response.getMessage());
}