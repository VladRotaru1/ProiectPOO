#pragma once
#include "User.h"
#include "AuditLog.h"
#include "Exceptii.h"
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

// ============================================================
// AuthService - gestioneaza autentificarea si sesiunile
// ============================================================
class AuthService {
private:
    std::vector<User> utilizatori;
    User* utilizatorCurent;
    std::string fisierUseri;
    static const int MAX_TENTATIVE = 3;

public:
    explicit AuthService(const std::string& fisier = "users.txt");
    ~AuthService() = default;

    // Login / Logout
    bool login(const std::string& username, const std::string& parola);
    void logout();
    bool esteAutentificat() const { return utilizatorCurent != nullptr; }
    User* getUtilizatorCurent() { return utilizatorCurent; }
    const User* getUtilizatorCurent() const { return utilizatorCurent; }

    // RBAC - verificare permisiuni
    bool arePermisiune(Rol rolNecesar) const;
    void verificaPermisiune(Rol rolNecesar) const;  // throws PermisiuneException
    // Admin are acces la tot
    bool arePermisiuneMinima(Rol rolMinim) const;

    // Gestionare utilizatori
    bool adaugaUtilizator(const User& user);
    bool eliminaUtilizator(const std::string& username);
    User* gaseste(const std::string& username);
    void afisareUtilizatori() const;
    void schimbaParola(const std::string& username, const std::string& parolaVeche,
                       const std::string& parolaNoua);

    // Persistenta
    void salveaza() const;
    void incarca();

    // Creare utilizatori default (pentru prima rulare)
    void creeazaUtilizatoriDefault();

    int getNumarUtilizatori() const { return (int)utilizatori.size(); }
};
