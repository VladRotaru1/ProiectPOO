#include "AparaturaMedicala.h"
#include "Exceptii.h"
#include <sstream>
#include <ctime>

std::string stareToString(StareAparatura s) {
    switch (s) {
        case StareAparatura::FUNCTIONALA:    return "FUNCTIONALA";
        case StareAparatura::DEFECTA:        return "DEFECTA";
        case StareAparatura::IN_MENTENANTA:  return "IN_MENTENANTA";
        case StareAparatura::REZERVATA:      return "REZERVATA";
    }
    return "FUNCTIONALA";
}

StareAparatura stringToStare(const std::string& s) {
    if (s == "DEFECTA")        return StareAparatura::DEFECTA;
    if (s == "IN_MENTENANTA")  return StareAparatura::IN_MENTENANTA;
    if (s == "REZERVATA")      return StareAparatura::REZERVATA;
    return StareAparatura::FUNCTIONALA;
}

std::string InregistrareMentenanta::serializa() const {
    return data + "~" + descriere + "~" + tehnician + "~" + std::to_string(cost);
}

InregistrareMentenanta InregistrareMentenanta::deserializa(const std::string& s) {
    InregistrareMentenanta im;
    std::istringstream iss(s);
    std::string tok;
    std::vector<std::string> v;
    while (std::getline(iss, tok, '~')) v.push_back(tok);
    if (v.size() >= 4) {
        im.data       = v[0];
        im.descriere  = v[1];
        im.tehnician  = v[2];
        im.cost       = std::stod(v[3]);
    }
    return im;
}

int AparaturaMedicala::contorAparatura = 0;

AparaturaMedicala::AparaturaMedicala()
    : idAparatura(""), denumire(""), serie(""), producator(""),
      stare(StareAparatura::FUNCTIONALA), idSectie(""),
      ultimaMentenanta("N/A"), disponibila(true) {}

AparaturaMedicala::AparaturaMedicala(const std::string& id_, const std::string& denumire_,
                                     const std::string& serie_, const std::string& producator_,
                                     const std::string& idSectie_)
    : idAparatura(id_), denumire(denumire_), serie(serie_), producator(producator_),
      stare(StareAparatura::FUNCTIONALA), idSectie(idSectie_),
      ultimaMentenanta("N/A"), disponibila(true) {}

void AparaturaMedicala::marcheazaDefecta(const std::string& motivul) {
    stare = StareAparatura::DEFECTA;
    disponibila = false;
    // Adaugam in jurnal
    InregistrareMentenanta im;
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    im.data      = buf;
    im.descriere = "DEFECTIUNE: " + motivul;
    im.tehnician = "N/A";
    im.cost      = 0;
    jurnalMentenanta.push_back(im);
}

void AparaturaMedicala::trimiteLaMentenanta(const std::string& data, const std::string& descriere,
                                             const std::string& tehnician, double cost) {
    stare = StareAparatura::IN_MENTENANTA;
    disponibila = false;
    ultimaMentenanta = data;
    InregistrareMentenanta im;
    im.data      = data;
    im.descriere = descriere;
    im.tehnician = tehnician;
    im.cost      = cost;
    jurnalMentenanta.push_back(im);
}

void AparaturaMedicala::marcheazaFunctionala() {
    stare = StareAparatura::FUNCTIONALA;
    disponibila = true;
}

void AparaturaMedicala::deblocheazaFolosire() {
    if (stare == StareAparatura::FUNCTIONALA)
        disponibila = true;
}

void AparaturaMedicala::afisare() const {
    std::cout << "[APARATURA] " << idAparatura << " | " << denumire
              << " | Serie: " << serie
              << " | Producator: " << producator
              << " | Stare: " << stareToString(stare)
              << " | Sectie: " << idSectie
              << " | Ultima mentenanta: " << ultimaMentenanta << "\n";
}

void AparaturaMedicala::afisareJurnal() const {
    std::cout << "\n--- Jurnal mentenanta: " << denumire << " ---\n";
    if (jurnalMentenanta.empty()) {
        std::cout << "  Nu exista inregistrari.\n";
        return;
    }
    for (const auto& im : jurnalMentenanta) {
        std::cout << "  [" << im.data << "] " << im.descriere
                  << " | Tehnician: " << im.tehnician
                  << " | Cost: " << im.cost << " RON\n";
    }
}

std::string AparaturaMedicala::serializa() const {
    std::string jurnalStr;
    for (size_t i = 0; i < jurnalMentenanta.size(); i++) {
        jurnalStr += jurnalMentenanta[i].serializa();
        if (i+1 < jurnalMentenanta.size()) jurnalStr += "^^";
    }
    return idAparatura + "|" + denumire + "|" + serie + "|" + producator + "|" +
           stareToString(stare) + "|" + idSectie + "|" + ultimaMentenanta + "|" +
           (disponibila ? "1" : "0") + "|" + jurnalStr;
}

void AparaturaMedicala::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);

    if (tokens.size() < 8) return;
    idAparatura     = tokens[0];
    denumire        = tokens[1];
    serie           = tokens[2];
    producator      = tokens[3];
    stare           = stringToStare(tokens[4]);
    idSectie        = tokens[5];
    ultimaMentenanta = tokens[6];
    disponibila     = (tokens[7] == "1");

    if (tokens.size() > 8 && !tokens[8].empty()) {
        jurnalMentenanta.clear();
        std::istringstream issJ(tokens[8]);
        std::string jStr;
        while (std::getline(issJ, jStr, '^'))
            if (!jStr.empty())
                jurnalMentenanta.push_back(InregistrareMentenanta::deserializa(jStr));
    }
}
