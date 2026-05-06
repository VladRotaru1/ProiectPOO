#include "AnalizaLaborator.h"
#include <sstream>

int AnalizaLaborator::contorAnalize = 0;

AnalizaLaborator::AnalizaLaborator()
    : idAnaliza(""), idPacient(""), idMedicResponsabil(""),
      tipAnaliza(""), dataRecoltare(""), dataRezultat(""),
      observatii(""), finalizata(false) {}

AnalizaLaborator::AnalizaLaborator(const std::string& id_, const std::string& idPacient_,
                                   const std::string& idMedic_, const std::string& tip_,
                                   const std::string& dataRec_)
    : idAnaliza(id_), idPacient(idPacient_), idMedicResponsabil(idMedic_),
      tipAnaliza(tip_), dataRecoltare(dataRec_), dataRezultat(""),
      observatii(""), finalizata(false) {}

void AnalizaLaborator::adaugaRezultat(const std::string& param, const std::string& val) {
    rezultate.push_back({param, val});
}

void AnalizaLaborator::adaugaValoareReferinta(const ValoareReferinta& vr) {
    valoriReferinta.push_back(vr);
}

void AnalizaLaborator::finalizeaza(const std::string& data, const std::string& obs) {
    dataRezultat = data;
    observatii   = obs;
    finalizata   = true;
}

void AnalizaLaborator::afisare() const {
    std::cout << "[ANALIZA] " << idAnaliza
              << " | Tip: " << tipAnaliza
              << " | Pacient: " << idPacient
              << " | Recoltata: " << dataRecoltare
              << " | " << (finalizata ? "FINALIZATA" : "IN_ASTEPTARE") << "\n";
}

void AnalizaLaborator::afisareCompleta() const {
    std::cout << "\n========== ANALIZA LABORATOR ==========\n";
    std::cout << "ID: " << idAnaliza << " | Tip: " << tipAnaliza << "\n";
    std::cout << "Pacient: " << idPacient << " | Medic: " << idMedicResponsabil << "\n";
    std::cout << "Data recoltare: " << dataRecoltare << "\n";
    if (finalizata) {
        std::cout << "Data rezultat: " << dataRezultat << "\n";
        std::cout << "Observatii: " << observatii << "\n";
        std::cout << "\n  Rezultate:\n";
        for (const auto& r : rezultate) {
            std::cout << "    " << r.first << ": " << r.second;
            // Cautam valoarea de referinta
            for (const auto& vr : valoriReferinta) {
                if (vr.parametru == r.first)
                    std::cout << " " << vr.unitateMasura
                              << " (ref: " << vr.valoareNormala << ")";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "Status: IN ASTEPTARE\n";
    }
    std::cout << "=========================================\n";
}

std::string AnalizaLaborator::serializa() const {
    std::string rezStr;
    for (size_t i = 0; i < rezultate.size(); i++) {
        rezStr += rezultate[i].first + "~" + rezultate[i].second;
        if (i+1 < rezultate.size()) rezStr += "^^";
    }
    return idAnaliza + "|" + idPacient + "|" + idMedicResponsabil + "|" +
           tipAnaliza + "|" + dataRecoltare + "|" + dataRezultat + "|" +
           observatii + "|" + (finalizata ? "1" : "0") + "|" + rezStr;
}

void AnalizaLaborator::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 8) return;
    idAnaliza           = tokens[0];
    idPacient           = tokens[1];
    idMedicResponsabil  = tokens[2];
    tipAnaliza          = tokens[3];
    dataRecoltare       = tokens[4];
    dataRezultat        = tokens[5];
    observatii          = tokens[6];
    finalizata          = (tokens[7] == "1");
    if (tokens.size() > 8 && !tokens[8].empty()) {
        rezultate.clear();
        std::istringstream issR(tokens[8]);
        std::string rStr;
        while (std::getline(issR, rStr, '^')) {
            if (rStr.empty()) continue;
            auto pos = rStr.find('~');
            if (pos != std::string::npos)
                rezultate.push_back({rStr.substr(0, pos), rStr.substr(pos+1)});
        }
    }
}
