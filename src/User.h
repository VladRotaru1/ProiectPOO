#pragma once
#include <string>
#include <iostream>

// ============================================================
// Roluri utilizatori (RBAC)
// ============================================================
enum class Rol {
    ADMIN,
    MEDIC,
    ASISTENT,
    RECEPTIE
};

std::string rolToString(Rol r);
Rol stringToRol(const std::string& s);

// ============================================================
// User - cont de autentificare
// ============================================================
class User {
private:
    std::string username;
    std::string parolaHash;
    Rol rol;
    std::string idPersoanaAsociata;  // id medic/asistent/etc
    bool activ;
    int tentativeEsuate;

    // Hash simplu (suma caractere XOR cu pozitia)
    static std::string hashParola(const std::string& parola);

public:
    User();
    User(const std::string& username, const std::string& parola,
         Rol rol, const std::string& idPersoana = "");
    ~User() = default;

    // Getters
    std::string getUsername()           const { return username; }
    Rol         getRol()                const { return rol; }
    std::string getIdPersoanaAsociata() const { return idPersoanaAsociata; }
    bool        isActiv()               const { return activ; }
    int         getTentativeEsuate()    const { return tentativeEsuate; }

    // Setters
    void setActiv(bool a)                          { activ = a; }
    void setRol(Rol r)                             { rol = r; }
    void setIdPersoanaAsociata(const std::string& id) { idPersoanaAsociata = id; }

    // Autentificare
    bool verificaParola(const std::string& parola) const;
    void schimbaParola(const std::string& parolaVeche, const std::string& parolaNoua);
    void incrementeazaTentative() { tentativeEsuate++; }
    void reseteazaTentative()     { tentativeEsuate = 0; }
    bool esteBlockat()            const { return tentativeEsuate >= 3; }

    void afisare() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
