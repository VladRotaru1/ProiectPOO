#include "Sectie.h"
#include <sstream>
#include <iostream>
#include <algorithm>

int Sectie::contorSectii = 0;

Sectie::Sectie() : idSectie(""), numeSectie(""), etaj(0) {}

Sectie::Sectie(const std::string& id_, const std::string& nume_, int etaj_)
    : idSectie(id_), numeSectie(nume_), etaj(etaj_) {}

void Sectie::adaugaMedic(const std::string& idMedic) {
    if (std::find(idMedici.begin(), idMedici.end(), idMedic) == idMedici.end())
        idMedici.push_back(idMedic);
}

void Sectie::eliminaMedic(const std::string& idMedic) {
    idMedici.erase(std::remove(idMedici.begin(), idMedici.end(), idMedic), idMedici.end());
}

void Sectie::adaugaAsistent(const std::string& idAsistent) {
    if (std::find(idAsistenti.begin(), idAsistenti.end(), idAsistent) == idAsistenti.end())
        idAsistenti.push_back(idAsistent);
}

void Sectie::eliminaAsistent(const std::string& idAsistent) {
    idAsistenti.erase(std::remove(idAsistenti.begin(), idAsistenti.end(), idAsistent), idAsistenti.end());
}

void Sectie::adaugaSalon(const Salon& salon) {
    saloane.push_back(salon);
}

Salon* Sectie::getSalon(const std::string& numarSalon) {
    for (auto& s : saloane)
        if (s.getNumarSalon() == numarSalon) return &s;
    return nullptr;
}

int Sectie::getPaturiLibereTotal() const {
    int total = 0;
    for (const auto& s : saloane)
        total += s.getPaturiLibere();
    return total;
}

int Sectie::getPaturiOcupateTotal() const {
    int total = 0;
    for (const auto& s : saloane)
        total += s.getPaturiOcupate();
    return total;
}

bool Sectie::gastestePatLiber(std::string& numarSalon, std::string& codPat) {
    for (auto& salon : saloane) {
        Pat* p = salon.primulPatLiber();
        if (p) {
            numarSalon = salon.getNumarSalon();
            codPat = p->getCodPat();
            return true;
        }
    }
    return false;
}

bool Sectie::ocupa(const std::string& idPacient, std::string& numarSalon, std::string& codPat) {
    for (auto& salon : saloane) {
        if (salon.ocupa(idPacient, codPat)) {
            numarSalon = salon.getNumarSalon();
            return true;
        }
    }
    return false;
}

bool Sectie::elibereazaPat(const std::string& codPat) {
    for (auto& salon : saloane)
        if (salon.elibereazaPat(codPat)) return true;
    return false;
}

void Sectie::adaugaAparatura(const std::string& idApar) {
    if (std::find(idAparatura.begin(), idAparatura.end(), idApar) == idAparatura.end())
        idAparatura.push_back(idApar);
}

void Sectie::afisare() const {
    std::cout << "Sectie: " << numeSectie << " (Etaj " << etaj << ")\n";
    std::cout << "  Medici: " << idMedici.size()
              << " | Asistenti: " << idAsistenti.size()
              << " | Paturi libere: " << getPaturiLibereTotal()
              << "/" << (getPaturiLibereTotal() + getPaturiOcupateTotal()) << "\n";
    for (const auto& s : saloane)
        s.afisare();
}

std::string Sectie::serializa() const {
    std::string mediciStr, asistentiStr, saloaneStr, aparStr;

    for (size_t i = 0; i < idMedici.size(); i++) {
        mediciStr += idMedici[i];
        if (i+1 < idMedici.size()) mediciStr += ";";
    }
    for (size_t i = 0; i < idAsistenti.size(); i++) {
        asistentiStr += idAsistenti[i];
        if (i+1 < idAsistenti.size()) asistentiStr += ";";
    }
    for (size_t i = 0; i < saloane.size(); i++) {
        saloaneStr += saloane[i].serializa();
        if (i+1 < saloane.size()) saloaneStr += "^";
    }
    for (size_t i = 0; i < idAparatura.size(); i++) {
        aparStr += idAparatura[i];
        if (i+1 < idAparatura.size()) aparStr += ";";
    }

    return idSectie + "|" + numeSectie + "|" + std::to_string(etaj) + "|" +
           mediciStr + "|" + asistentiStr + "|" + saloaneStr + "|" + aparStr;
}

void Sectie::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);

    if (tokens.size() < 3) return;
    idSectie    = tokens[0];
    numeSectie  = tokens[1];
    etaj        = std::stoi(tokens[2]);

    if (tokens.size() > 3 && !tokens[3].empty()) {
        idMedici.clear();
        std::istringstream issM(tokens[3]);
        std::string m;
        while (std::getline(issM, m, ';'))
            if (!m.empty()) idMedici.push_back(m);
    }
    if (tokens.size() > 4 && !tokens[4].empty()) {
        idAsistenti.clear();
        std::istringstream issA(tokens[4]);
        std::string a;
        while (std::getline(issA, a, ';'))
            if (!a.empty()) idAsistenti.push_back(a);
    }
    if (tokens.size() > 5 && !tokens[5].empty()) {
        saloane.clear();
        std::istringstream issS(tokens[5]);
        std::string sStr;
        while (std::getline(issS, sStr, '^')) {
            if (!sStr.empty()) {
                Salon s;
                s.deserializa(sStr);
                saloane.push_back(s);
            }
        }
    }
    if (tokens.size() > 6 && !tokens[6].empty()) {
        idAparatura.clear();
        std::istringstream issAp(tokens[6]);
        std::string ap;
        while (std::getline(issAp, ap, ';'))
            if (!ap.empty()) idAparatura.push_back(ap);
    }
}
