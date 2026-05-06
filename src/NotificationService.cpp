#include "NotificationService.h"
#include <fstream>
#include <ctime>
#include <algorithm>

// ============================================================
// FileObserver
// ============================================================
void FileObserver::onNotificare(const std::string& tip, const std::string& mesaj) {
    std::ofstream ofs(fisierLog, std::ios::app);
    if (ofs.is_open()) {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
        ofs << "[" << buf << "] NOTIFICARE [" << tip << "]: " << mesaj << "\n";
    }
}

// ============================================================
// NotificationService Singleton
// ============================================================
NotificationService* NotificationService::getInstance() {
    static NotificationService instance;
    return &instance;
}

void NotificationService::inregistreazaObserver(std::shared_ptr<IObserver> obs) {
    observeri.push_back(obs);
}

void NotificationService::eliminaObserver(std::shared_ptr<IObserver> obs) {
    observeri.erase(
        std::remove(observeri.begin(), observeri.end(), obs),
        observeri.end());
}

void NotificationService::notifica(const std::string& tip, const std::string& mesaj) {
    istoricNotificari.push_back({tip, mesaj});
    for (auto& obs : observeri)
        obs->onNotificare(tip, mesaj);
}

void NotificationService::afisareIstoricNotificari() const {
    std::cout << "\n========== ISTORIC NOTIFICARI (" << istoricNotificari.size() << ") ==========\n";
    if (istoricNotificari.empty()) {
        std::cout << "  Nu exista notificari.\n";
        return;
    }
    for (const auto& n : istoricNotificari)
        std::cout << "  [" << n.first << "] " << n.second << "\n";
    std::cout << "================================================\n";
}
