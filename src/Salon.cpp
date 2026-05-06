#include "Salon.h"
#include <sstream>
#include <iostream>

std::string tipSalonToString(TipSalon t) {
    switch (t) {
        case TipSalon::STANDARD: return "STANDARD";
        case TipSalon::ATI:      return "ATI";
        case TipSalon::IZOLARE:  return "IZOLARE";
    }
    return "STANDARD";
}

TipSalon stringToTipSalon(const std::string& s) {
    if (s == "ATI")     return TipSalon::ATI;
    if (s == "IZOLARE") return TipSalon::IZOLARE;
    return TipSalon::STANDARD;
}

Salon::Salon() : numarSalon(""), tip(TipSalon::STANDARD), capacitate(0) {}

Salon::Salon(const std::string& numar, TipSalon tip_, int capacitate_)
    : numarSalon(numar), tip(tip_), capacitate(capacitate_)
{
    // Cream paturile
    for (int i = 1; i <= capacitate_; i++) {
        paturi.emplace_back(numar + "-P" + std::to_string(i));
    }
}

int Salon::getPaturiLibere() const {
    int cnt = 0;
    for (const auto& p : paturi)
        if (!p.isOcupat()) cnt++;
    return cnt;
}

int Salon::getPaturiOcupate() const {
    int cnt = 0;
    for (const auto& p : paturi)
        if (p.isOcupat()) cnt++;
    return cnt;
}

Pat* Salon::primulPatLiber() {
    for (auto& p : paturi)
        if (p.isDisponibil()) return &p;
    return nullptr;
}

bool Salon::ocupa(const std::string& idPacient, std::string& codPatOcupat) {
    Pat* p = primulPatLiber();
    if (!p) return false;
    p->ocupa(idPacient);
    codPatOcupat = p->getCodPat();
    return true;
}

bool Salon::elibereazaPat(const std::string& codPat) {
    for (auto& p : paturi) {
        if (p.getCodPat() == codPat) {
            p.elibereaza();
            return true;
        }
    }
    return false;
}

bool Salon::igienizeazaPat(const std::string& codPat) {
    for (auto& p : paturi) {
        if (p.getCodPat() == codPat) {
            p.marcheazaIgienizat();
            return true;
        }
    }
    return false;
}

Pat* Salon::getPat(const std::string& codPat) {
    for (auto& p : paturi)
        if (p.getCodPat() == codPat) return &p;
    return nullptr;
}

void Salon::afisare() const {
    std::cout << "  Salon " << numarSalon
              << " [" << tipSalonToString(tip) << "]"
              << " - Capacitate: " << capacitate
              << " | Libere: " << getPaturiLibere()
              << " | Ocupate: " << getPaturiOcupate() << "\n";
    for (const auto& p : paturi)
        p.afisare();
}

std::string Salon::serializa() const {
    std::string paturiStr;
    for (size_t i = 0; i < paturi.size(); i++) {
        paturiStr += paturi[i].serializa();
        if (i + 1 < paturi.size()) paturiStr += "*";
    }
    return numarSalon + "#" + tipSalonToString(tip) + "#" +
           std::to_string(capacitate) + "#" + paturiStr;
}

void Salon::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '#')) tokens.push_back(tok);

    if (tokens.size() < 3) return;
    numarSalon = tokens[0];
    tip        = stringToTipSalon(tokens[1]);
    capacitate = std::stoi(tokens[2]);

    paturi.clear();
    if (tokens.size() > 3 && !tokens[3].empty()) {
        std::istringstream issP(tokens[3]);
        std::string pStr;
        while (std::getline(issP, pStr, '*')) {
            if (!pStr.empty()) {
                Pat p;
                p.deserializa(pStr);
                paturi.push_back(p);
            }
        }
    } else {
        // Daca nu avem date de paturi, cream paturi goale
        for (int i = 1; i <= capacitate; i++)
            paturi.emplace_back(numarSalon + "-P" + std::to_string(i));
    }
}
