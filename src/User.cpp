#include "User.h"
#include "Exceptii.h"
#include <sstream>
#include <functional>

// ============================================================
// Helper rol
// ============================================================
std::string rolToString(Rol r) {
    switch (r) {
        case Rol::ADMIN:    return "ADMIN";
        case Rol::MEDIC:    return "MEDIC";
        case Rol::ASISTENT: return "ASISTENT";
        case Rol::RECEPTIE: return "RECEPTIE";
    }
    return "RECEPTIE";
}

Rol stringToRol(const std::string& s) {
    if (s == "ADMIN")    return Rol::ADMIN;
    if (s == "MEDIC")    return Rol::MEDIC;
    if (s == "ASISTENT") return Rol::ASISTENT;
    return Rol::RECEPTIE;
}

// ============================================================
// Hash parola - combinatie XOR + djb2
// ============================================================
std::string User::hashParola(const std::string& parola) {
    // djb2 hash
    unsigned long hash = 5381;
    for (char c : parola)
        hash = ((hash << 5) + hash) + (unsigned char)c;
    // Adaugam salt simplu (XOR cu o constanta)
    hash ^= 0xDEADBEEF;
    // Convertim in hex
    char buf[32];
    snprintf(buf, sizeof(buf), "%016lx", hash);
    return std::string(buf);
}

// ============================================================
// User
// ============================================================
User::User()
    : username(""), parolaHash(""), rol(Rol::RECEPTIE),
      idPersoanaAsociata(""), activ(true), tentativeEsuate(0) {}

User::User(const std::string& username_, const std::string& parola_,
           Rol rol_, const std::string& idPersoana_)
    : username(username_), parolaHash(hashParola(parola_)), rol(rol_),
      idPersoanaAsociata(idPersoana_), activ(true), tentativeEsuate(0) {}

bool User::verificaParola(const std::string& parola) const {
    return parolaHash == hashParola(parola);
}

void User::schimbaParola(const std::string& parolaVeche, const std::string& parolaNoua) {
    if (!verificaParola(parolaVeche))
        throw AutentificareException("Parola veche incorecta");
    if (parolaNoua.size() < 4)
        throw AutentificareException("Parola noua prea scurta (minim 4 caractere)");
    parolaHash = hashParola(parolaNoua);
}

void User::afisare() const {
    std::cout << "[USER] " << username
              << " | Rol: " << rolToString(rol)
              << " | Activ: " << (activ ? "DA" : "NU")
              << " | Persoana: " << idPersoanaAsociata << "\n";
}

std::string User::serializa() const {
    return username + "|" + parolaHash + "|" + rolToString(rol) + "|" +
           idPersoanaAsociata + "|" + (activ ? "1" : "0") + "|" +
           std::to_string(tentativeEsuate);
}

void User::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() >= 4) {
        username            = tokens[0];
        parolaHash          = tokens[1];
        rol                 = stringToRol(tokens[2]);
        idPersoanaAsociata  = tokens[3];
    }
    if (tokens.size() > 4) activ             = (tokens[4] == "1");
    if (tokens.size() > 5) tentativeEsuate   = std::stoi(tokens[5]);
}
