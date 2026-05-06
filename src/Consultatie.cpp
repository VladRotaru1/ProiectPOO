#include "Consultatie.h"
#include <sstream>

int Consultatie::contorConsultatii = 0;

Consultatie::Consultatie()
    : idConsultatie(""), idProgramare(""), idPacient(""), idMedic(""),
      dataConsultatiei(""), simptome(""), diagnostic(""), tratament(""),
      recomandari(""), zileConcediu(0), reteta(nullptr) {}

Consultatie::Consultatie(const std::string& id_, const std::string& idProg_,
                         const std::string& idPac_, const std::string& idMed_,
                         const std::string& data_)
    : idConsultatie(id_), idProgramare(idProg_), idPacient(idPac_), idMedic(idMed_),
      dataConsultatiei(data_), simptome(""), diagnostic(""), tratament(""),
      recomandari(""), zileConcediu(0), reteta(nullptr) {}

void Consultatie::afisare() const {
    std::cout << "[CONSULTATIE] " << idConsultatie
              << " | Pacient: " << idPacient
              << " | Medic: " << idMedic
              << " | Data: " << dataConsultatiei
              << " | Diag: " << diagnostic
              << (areReteta() ? " | [Are reteta]" : "") << "\n";
}

void Consultatie::afisareCompleta() const {
    std::cout << "\n========== CONSULTATIE MEDICALA ==========\n";
    std::cout << "ID Consultatie: " << idConsultatie << "\n";
    std::cout << "Data: " << dataConsultatiei << "\n";
    std::cout << "Pacient: " << idPacient << " | Medic: " << idMedic << "\n";
    std::cout << "Programare: " << idProgramare << "\n";
    std::cout << "Simptome: " << simptome << "\n";
    std::cout << "Diagnostic: " << diagnostic << "\n";
    std::cout << "Tratament: " << tratament << "\n";
    std::cout << "Recomandari: " << recomandari << "\n";
    if (zileConcediu > 0)
        std::cout << "Concediu medical: " << zileConcediu << " zile\n";
    if (areReteta()) {
        std::cout << "\nReteta prescrisa:\n";
        reteta->afisare();
    }
    std::cout << "==========================================\n";
}

std::string Consultatie::serializa() const {
    std::string retetaStr = reteta ? reteta->serializa() : "";
    return idConsultatie + "|" + idProgramare + "|" + idPacient + "|" +
           idMedic + "|" + dataConsultatiei + "|" + simptome + "|" +
           diagnostic + "|" + tratament + "|" + recomandari + "|" +
           std::to_string(zileConcediu) + "|" + retetaStr;
}

void Consultatie::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 10) return;
    idConsultatie    = tokens[0];
    idProgramare     = tokens[1];
    idPacient        = tokens[2];
    idMedic          = tokens[3];
    dataConsultatiei = tokens[4];
    simptome         = tokens[5];
    diagnostic       = tokens[6];
    tratament        = tokens[7];
    recomandari      = tokens[8];
    zileConcediu     = std::stoi(tokens[9]);
    if (tokens.size() > 10 && !tokens[10].empty()) {
        reteta = std::make_shared<Reteta>();
        reteta->deserializa(tokens[10]);
    }
}
