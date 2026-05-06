#include "Internare.h"
#include <sstream>
#include <iostream>

std::string stareInternToString(StareInternare s) {
    switch (s) {
        case StareInternare::ACTIVA:      return "ACTIVA";
        case StareInternare::EXTERNATA:   return "EXTERNATA";
        case StareInternare::TRANSFERATA: return "TRANSFERATA";
        case StareInternare::DECEDAT:     return "DECEDAT";
    }
    return "ACTIVA";
}

StareInternare stringToStareIntern(const std::string& s) {
    if (s == "EXTERNATA")   return StareInternare::EXTERNATA;
    if (s == "TRANSFERATA") return StareInternare::TRANSFERATA;
    if (s == "DECEDAT")     return StareInternare::DECEDAT;
    return StareInternare::ACTIVA;
}

int Internare::contorInternari = 0;

Internare::Internare()
    : idInternare(""), idPacient(""), idMedicResponsabil(""),
      idSectie(""), numarSalon(""), codPat(""),
      dataInternarii(""), dataExternarii(""),
      tratamentActiv(""), stare(StareInternare::ACTIVA),
      observatii(""), diagnosticInternare(""), diagnosticExternare(""),
      costTotal(0.0) {}

Internare::Internare(const std::string& id_, const std::string& idPacient_,
                     const std::string& idMedic_, const std::string& idSectie_,
                     const std::string& numarSalon_, const std::string& codPat_,
                     const std::string& dataInternarii_, const std::string& diagIntern_)
    : idInternare(id_), idPacient(idPacient_), idMedicResponsabil(idMedic_),
      idSectie(idSectie_), numarSalon(numarSalon_), codPat(codPat_),
      dataInternarii(dataInternarii_), dataExternarii(""),
      tratamentActiv(""), stare(StareInternare::ACTIVA),
      observatii(""), diagnosticInternare(diagIntern_), diagnosticExternare(""),
      costTotal(0.0) {}

void Internare::externeaza(const std::string& data, const std::string& diagExt) {
    dataExternarii     = data;
    diagnosticExternare = diagExt;
    stare = StareInternare::EXTERNATA;
}

void Internare::transfera(const std::string& nouaSectie, const std::string& noulSalon,
                           const std::string& noulPat) {
    idSectie    = nouaSectie;
    numarSalon  = noulSalon;
    codPat      = noulPat;
    stare       = StareInternare::TRANSFERATA;
    // Dupa transfer, revine la ACTIVA
    stare       = StareInternare::ACTIVA;
}

void Internare::marcheazaDecedat() {
    stare = StareInternare::DECEDAT;
}

void Internare::adaugaParametriVitali(const std::string& timestamp, const std::string& valori) {
    parametriVitali.push_back({timestamp, valori});
}

void Internare::afisare() const {
    std::cout << "[INTERNARE] " << idInternare
              << " | Pacient: " << idPacient
              << " | Medic: " << idMedicResponsabil
              << " | Sectie: " << idSectie
              << " | Salon: " << numarSalon << " Pat: " << codPat
              << " | De la: " << dataInternarii
              << " | Stare: " << stareInternToString(stare)
              << " | Cost: " << costTotal << " RON\n";
}

void Internare::afisareCompleta() const {
    std::cout << "\n========== INTERNARE ==========\n";
    std::cout << "ID: " << idInternare << "\n";
    std::cout << "Pacient: " << idPacient << "\n";
    std::cout << "Medic responsabil: " << idMedicResponsabil << "\n";
    std::cout << "Sectie: " << idSectie << " | Salon: " << numarSalon << " | Pat: " << codPat << "\n";
    std::cout << "Data internarii: " << dataInternarii << "\n";
    if (!dataExternarii.empty())
        std::cout << "Data externarii: " << dataExternarii << "\n";
    std::cout << "Diagnostic internare: " << diagnosticInternare << "\n";
    if (!diagnosticExternare.empty())
        std::cout << "Diagnostic externare: " << diagnosticExternare << "\n";
    std::cout << "Tratament activ: " << tratamentActiv << "\n";
    std::cout << "Stare: " << stareInternToString(stare) << "\n";
    std::cout << "Observatii: " << observatii << "\n";
    std::cout << "Cost total: " << costTotal << " RON\n";
    if (!parametriVitali.empty()) {
        std::cout << "\nParametri vitali monitorizati:\n";
        for (const auto& pv : parametriVitali)
            std::cout << "  [" << pv.first << "] " << pv.second << "\n";
    }
    std::cout << "================================\n";
}

std::string Internare::serializa() const {
    std::string pvStr;
    for (size_t i = 0; i < parametriVitali.size(); i++) {
        pvStr += parametriVitali[i].first + "~" + parametriVitali[i].second;
        if (i+1 < parametriVitali.size()) pvStr += "^";
    }
    return idInternare + "|" + idPacient + "|" + idMedicResponsabil + "|" +
           idSectie + "|" + numarSalon + "|" + codPat + "|" +
           dataInternarii + "|" + dataExternarii + "|" + tratamentActiv + "|" +
           stareInternToString(stare) + "|" + observatii + "|" +
           diagnosticInternare + "|" + diagnosticExternare + "|" +
           std::to_string(costTotal) + "|" + pvStr;
}

void Internare::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 14) return;
    idInternare          = tokens[0];
    idPacient            = tokens[1];
    idMedicResponsabil   = tokens[2];
    idSectie             = tokens[3];
    numarSalon           = tokens[4];
    codPat               = tokens[5];
    dataInternarii       = tokens[6];
    dataExternarii       = tokens[7];
    tratamentActiv       = tokens[8];
    stare                = stringToStareIntern(tokens[9]);
    observatii           = tokens[10];
    diagnosticInternare  = tokens[11];
    diagnosticExternare  = tokens[12];
    costTotal            = std::stod(tokens[13]);
    if (tokens.size() > 14 && !tokens[14].empty()) {
        parametriVitali.clear();
        std::istringstream issPV(tokens[14]);
        std::string pvStr;
        while (std::getline(issPV, pvStr, '^')) {
            if (pvStr.empty()) continue;
            auto pos = pvStr.find('~');
            if (pos != std::string::npos)
                parametriVitali.push_back({pvStr.substr(0, pos), pvStr.substr(pos+1)});
        }
    }
}
