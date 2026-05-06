#include "Produs.h"
#include "Exceptii.h"
#include <ctime>
#include <iostream>

int Produs::contorProduse = 0;

// ============================================================
// Produs
// ============================================================
Produs::Produs() : idProdus(""), denumire(""), cantitate(0), pret(0.0), termenExpirare("") {}

Produs::Produs(const std::string& id_, const std::string& denumire_,
               int cantitate_, double pret_, const std::string& termenExpirare_)
    : idProdus(id_), denumire(denumire_), cantitate(cantitate_),
      pret(pret_), termenExpirare(termenExpirare_) {}

void Produs::adaugaStoc(int qty) {
    if (qty > 0) cantitate += qty;
}

void Produs::scadeStoc(int qty) {
    if (qty > cantitate)
        throw StocInsuficientException(denumire, cantitate, qty);
    cantitate -= qty;
}

bool Produs::esteExpirat() const {
    if (termenExpirare.size() < 10) return false;
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    int anCurent  = 1900 + t->tm_year;
    int lunaCurenta = 1 + t->tm_mon;
    int ziCurenta   = t->tm_mday;

    int an   = std::stoi(termenExpirare.substr(0, 4));
    int luna = std::stoi(termenExpirare.substr(5, 2));
    int zi   = std::stoi(termenExpirare.substr(8, 2));

    if (an < anCurent)  return true;
    if (an == anCurent && luna < lunaCurenta) return true;
    if (an == anCurent && luna == lunaCurenta && zi < ziCurenta) return true;
    return false;
}

bool Produs::stocCritic(int prag) const {
    return cantitate <= prag;
}

std::string Produs::serializa() const {
    return idProdus + "|" + denumire + "|" + std::to_string(cantitate) + "|" +
           std::to_string(pret) + "|" + termenExpirare;
}

void Produs::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() >= 5) {
        idProdus        = tokens[0];
        denumire        = tokens[1];
        cantitate       = std::stoi(tokens[2]);
        pret            = std::stod(tokens[3]);
        termenExpirare  = tokens[4];
    }
}

// ============================================================
// Medicament
// ============================================================
Medicament::Medicament()
    : Produs(), substActiva(""), forma(""), contraindicatii(""),
      necesitaReteta(false), producator("") {}

Medicament::Medicament(const std::string& id_, const std::string& denumire_,
                       int cantitate_, double pret_, const std::string& termenExpirare_,
                       const std::string& substActiva_, const std::string& forma_,
                       bool necesitaReteta_, const std::string& producator_)
    : Produs(id_, denumire_, cantitate_, pret_, termenExpirare_),
      substActiva(substActiva_), forma(forma_), contraindicatii(""),
      necesitaReteta(necesitaReteta_), producator(producator_) {}

void Medicament::afisare() const {
    std::cout << "[MEDICAMENT] " << idProdus << " | " << denumire
              << " | Subst: " << substActiva
              << " | Forma: " << forma
              << " | Stoc: " << cantitate
              << " | Pret: " << pret << " RON"
              << " | Exp: " << termenExpirare
              << (esteExpirat() ? " [EXPIRAT!]" : "")
              << (stocCritic() ? " [STOC CRITIC!]" : "")
              << (necesitaReteta ? " [Reteta necesara]" : "")
              << "\n";
}

std::string Medicament::serializa() const {
    return "MEDICAMENT|" + Produs::serializa() + "|" + substActiva + "|" + forma + "|" +
           (necesitaReteta ? "1" : "0") + "|" + producator + "|" + contraindicatii;
}

void Medicament::deserializa(const std::string& linie) {
    // Formatul: MEDICAMENT|<baza>|substActiva|forma|necesitaReteta|producator|contraindicatii
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    // tokens[0] = "MEDICAMENT", tokens[1..5] = baza, tokens[6..] = specifice
    if (tokens.size() < 6) return;
    std::string bazaLinie = tokens[1] + "|" + tokens[2] + "|" + tokens[3] + "|" +
                            tokens[4] + "|" + tokens[5];
    Produs::deserializa(bazaLinie);
    if (tokens.size() > 6) substActiva   = tokens[6];
    if (tokens.size() > 7) forma         = tokens[7];
    if (tokens.size() > 8) necesitaReteta = (tokens[8] == "1");
    if (tokens.size() > 9) producator    = tokens[9];
    if (tokens.size() > 10) contraindicatii = tokens[10];
}

// ============================================================
// Consumabil
// ============================================================
Consumabil::Consumabil()
    : Produs(), categorie(""), unitate("bucati") {}

Consumabil::Consumabil(const std::string& id_, const std::string& denumire_,
                       int cantitate_, double pret_, const std::string& termenExpirare_,
                       const std::string& categorie_, const std::string& unitate_)
    : Produs(id_, denumire_, cantitate_, pret_, termenExpirare_),
      categorie(categorie_), unitate(unitate_) {}

void Consumabil::afisare() const {
    std::cout << "[CONSUMABIL] " << idProdus << " | " << denumire
              << " | Cat: " << categorie
              << " | Stoc: " << cantitate << " " << unitate
              << " | Pret: " << pret << " RON"
              << " | Exp: " << termenExpirare
              << (esteExpirat() ? " [EXPIRAT!]" : "")
              << (stocCritic() ? " [STOC CRITIC!]" : "")
              << "\n";
}

std::string Consumabil::serializa() const {
    return "CONSUMABIL|" + Produs::serializa() + "|" + categorie + "|" + unitate;
}

void Consumabil::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 6) return;
    std::string bazaLinie = tokens[1] + "|" + tokens[2] + "|" + tokens[3] + "|" +
                            tokens[4] + "|" + tokens[5];
    Produs::deserializa(bazaLinie);
    if (tokens.size() > 6) categorie = tokens[6];
    if (tokens.size() > 7) unitate   = tokens[7];
}

// ============================================================
// Echipament
// ============================================================
Echipament::Echipament()
    : Produs(), tip(""), stare("nou") {}

Echipament::Echipament(const std::string& id_, const std::string& denumire_,
                       int cantitate_, double pret_, const std::string& termenExpirare_,
                       const std::string& tip_, const std::string& stare_)
    : Produs(id_, denumire_, cantitate_, pret_, termenExpirare_),
      tip(tip_), stare(stare_) {}

void Echipament::afisare() const {
    std::cout << "[ECHIPAMENT] " << idProdus << " | " << denumire
              << " | Tip: " << tip
              << " | Stare: " << stare
              << " | Stoc: " << cantitate
              << " | Pret: " << pret << " RON\n";
}

std::string Echipament::serializa() const {
    return "ECHIPAMENT|" + Produs::serializa() + "|" + tip + "|" + stare;
}

void Echipament::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 6) return;
    std::string bazaLinie = tokens[1] + "|" + tokens[2] + "|" + tokens[3] + "|" +
                            tokens[4] + "|" + tokens[5];
    Produs::deserializa(bazaLinie);
    if (tokens.size() > 6) tip   = tokens[6];
    if (tokens.size() > 7) stare = tokens[7];
}
