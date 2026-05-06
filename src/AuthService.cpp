#include "AuthService.h"
#include <iostream>
#include <algorithm>

AuthService::AuthService(const std::string& fisier)
    : utilizatorCurent(nullptr), fisierUseri(fisier) {
    incarca();
    if (utilizatori.empty())
        creeazaUtilizatoriDefault();
}

bool AuthService::login(const std::string& username, const std::string& parola) {
    User* u = gaseste(username);
    if (!u) {
        std::cout << "  Utilizatorul '" << username << "' nu exista.\n";
        return false;
    }
    if (!u->isActiv()) {
        std::cout << "  Contul este dezactivat.\n";
        return false;
    }
    if (u->esteBlockat()) {
        std::cout << "  Contul este blocat (prea multe tentative esuate).\n";
        return false;
    }
    if (!u->verificaParola(parola)) {
        u->incrementeazaTentative();
        std::cout << "  Parola incorecta. Tentative ramase: "
                  << (MAX_TENTATIVE - u->getTentativeEsuate()) << "\n";
        AuditLog::getInstance()->log(username, "LOGIN_ESUAT");
        salveaza();
        return false;
    }
    u->reseteazaTentative();
    utilizatorCurent = u;
    AuditLog::getInstance()->log(username, "LOGIN_SUCCES (rol: " + rolToString(u->getRol()) + ")");
    salveaza();
    return true;
}

void AuthService::logout() {
    if (utilizatorCurent) {
        AuditLog::getInstance()->log(utilizatorCurent->getUsername(), "LOGOUT");
        utilizatorCurent = nullptr;
    }
}

bool AuthService::arePermisiune(Rol rolNecesar) const {
    if (!utilizatorCurent) return false;
    if (utilizatorCurent->getRol() == Rol::ADMIN) return true;
    return utilizatorCurent->getRol() == rolNecesar;
}

bool AuthService::arePermisiuneMinima(Rol rolMinim) const {
    if (!utilizatorCurent) return false;
    if (utilizatorCurent->getRol() == Rol::ADMIN) return true;
    return (int)utilizatorCurent->getRol() <= (int)rolMinim;
}

void AuthService::verificaPermisiune(Rol rolNecesar) const {
    if (!arePermisiune(rolNecesar))
        throw PermisiuneException(rolToString(rolNecesar));
}

bool AuthService::adaugaUtilizator(const User& user) {
    if (gaseste(user.getUsername())) return false;
    utilizatori.push_back(user);
    salveaza();
    AuditLog::getInstance()->log(
        utilizatorCurent ? utilizatorCurent->getUsername() : "SYSTEM",
        "ADAUGARE_USER: " + user.getUsername());
    return true;
}

bool AuthService::eliminaUtilizator(const std::string& username) {
    auto it = std::find_if(utilizatori.begin(), utilizatori.end(),
        [&](const User& u) { return u.getUsername() == username; });
    if (it == utilizatori.end()) return false;
    utilizatori.erase(it);
    salveaza();
    AuditLog::getInstance()->log(
        utilizatorCurent ? utilizatorCurent->getUsername() : "SYSTEM",
        "STERGERE_USER: " + username);
    return true;
}

User* AuthService::gaseste(const std::string& username) {
    for (auto& u : utilizatori)
        if (u.getUsername() == username) return &u;
    return nullptr;
}

void AuthService::afisareUtilizatori() const {
    std::cout << "\n========== UTILIZATORI (" << utilizatori.size() << ") ==========\n";
    for (const auto& u : utilizatori)
        u.afisare();
    std::cout << "==========================================\n";
}

void AuthService::schimbaParola(const std::string& username, const std::string& parolaVeche,
                                  const std::string& parolaNoua) {
    User* u = gaseste(username);
    if (!u) throw AutentificareException("Utilizatorul " + username + " nu exista");
    u->schimbaParola(parolaVeche, parolaNoua);
    salveaza();
    AuditLog::getInstance()->log(username, "SCHIMBARE_PAROLA");
}

void AuthService::salveaza() const {
    std::ofstream ofs(fisierUseri);
    if (!ofs.is_open()) return;
    for (const auto& u : utilizatori)
        ofs << u.serializa() << "\n";
}

void AuthService::incarca() {
    std::ifstream ifs(fisierUseri);
    if (!ifs.is_open()) return;
    utilizatori.clear();
    std::string linie;
    while (std::getline(ifs, linie)) {
        if (linie.empty()) continue;
        User u;
        u.deserializa(linie);
        utilizatori.push_back(u);
    }
}

void AuthService::creeazaUtilizatoriDefault() {
    // Utilizatori demo cu parole simple
    utilizatori.push_back(User("admin",     "admin123",    Rol::ADMIN));
    utilizatori.push_back(User("dr.popescu","medic123",    Rol::MEDIC,    "MED1"));
    utilizatori.push_back(User("asist.ion", "asist123",    Rol::ASISTENT, "ASI1"));
    utilizatori.push_back(User("receptie",  "receptie123", Rol::RECEPTIE));
    salveaza();
    std::cout << "\n  [INFO] Utilizatori default creati:\n";
    std::cout << "    admin / admin123\n";
    std::cout << "    dr.popescu / medic123\n";
    std::cout << "    asist.ion / asist123\n";
    std::cout << "    receptie / receptie123\n\n";
}
