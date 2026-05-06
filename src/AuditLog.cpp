#include "AuditLog.h"
#include <vector>
#include <algorithm>

AuditLog* AuditLog::getInstance() {
    static AuditLog instance;
    return &instance;
}

void AuditLog::setFisier(const std::string& path) {
    fisierLog = path;
}

std::string AuditLog::getTimestamp() const {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return std::string(buf);
}

void AuditLog::log(const std::string& utilizator, const std::string& actiune) {
    std::ofstream ofs(fisierLog, std::ios::app);
    if (ofs.is_open()) {
        ofs << "[" << getTimestamp() << "] "
            << "[" << utilizator << "] "
            << "[" << actiune << "]\n";
    }
    // Afisam si in consola (optional, doar informatii)
}

void AuditLog::afisareLog(int ultimeleN) const {
    std::ifstream ifs(fisierLog);
    if (!ifs.is_open()) {
        std::cout << "Fisierul audit.log nu exista sau nu poate fi deschis.\n";
        return;
    }
    std::vector<std::string> linii;
    std::string linie;
    while (std::getline(ifs, linie)) {
        if (!linie.empty())
            linii.push_back(linie);
    }
    int start = 0;
    if ((int)linii.size() > ultimeleN)
        start = (int)linii.size() - ultimeleN;

    std::cout << "========== AUDIT LOG (ultimele " << ultimeleN << " intrari) ==========\n";
    for (int i = start; i < (int)linii.size(); i++) {
        std::cout << linii[i] << "\n";
    }
    std::cout << "=================================================================\n";
}
