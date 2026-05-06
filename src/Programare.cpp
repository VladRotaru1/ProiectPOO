#include "Programare.h"
#include <sstream>
#include <vector>
#include <algorithm>

std::string statusProgToString(StatusProgramare s) {
    switch (s) {
        case StatusProgramare::ACTIVA:       return "ACTIVA";
        case StatusProgramare::ANULATA:      return "ANULATA";
        case StatusProgramare::EFECTUATA:    return "EFECTUATA";
        case StatusProgramare::NEPREZENTAT:  return "NEPREZENTAT";
    }
    return "ACTIVA";
}

StatusProgramare stringToStatusProg(const std::string& s) {
    if (s == "ANULATA")     return StatusProgramare::ANULATA;
    if (s == "EFECTUATA")   return StatusProgramare::EFECTUATA;
    if (s == "NEPREZENTAT") return StatusProgramare::NEPREZENTAT;
    return StatusProgramare::ACTIVA;
}

int Programare::contorProgramari = 0;

Programare::Programare()
    : idProgramare(""), idPacient(""), idMedic(""),
      data(""), ora(""), cabinet(""), status(StatusProgramare::ACTIVA), descriere("") {}

Programare::Programare(const std::string& id_, const std::string& idPacient_,
                       const std::string& idMedic_, const std::string& data_,
                       const std::string& ora_, const std::string& cabinet_,
                       const std::string& descriere_)
    : idProgramare(id_), idPacient(idPacient_), idMedic(idMedic_),
      data(data_), ora(ora_), cabinet(cabinet_),
      status(StatusProgramare::ACTIVA), descriere(descriere_) {}

bool Programare::suprapuneCu(const Programare& alta) const {
    // Suprapunere: acelasi medic, aceeasi data, aceeasi ora
    if (idMedic != alta.idMedic) return false;
    if (data != alta.data) return false;
    if (status == StatusProgramare::ANULATA || alta.status == StatusProgramare::ANULATA)
        return false;
    return ora == alta.ora;
}

int Programare::estimareTimpAsteptare(const std::vector<std::shared_ptr<Programare>>& listaProgramari,
                                       const std::string& idMedicCautat) const {
    // Numara programarile active ale medicului in aceeasi zi, inainte de ora curenta
    int count = 0;
    for (const auto& p : listaProgramari) {
        if (p->idMedic == idMedicCautat &&
            p->data == data &&
            p->status == StatusProgramare::ACTIVA &&
            p->ora < ora)
            count++;
    }
    // Estimam 20 minute per consultatie
    return count * 20;
}

void Programare::afisare() const {
    std::cout << "[PROGRAMARE] " << idProgramare
              << " | Pacient: " << idPacient
              << " | Medic: " << idMedic
              << " | Data: " << data << " ora " << ora
              << " | Cabinet: " << cabinet
              << " | Status: " << statusProgToString(status);
    if (!descriere.empty()) std::cout << " | Desc: " << descriere;
    std::cout << "\n";
}

std::string Programare::serializa() const {
    return idProgramare + "|" + idPacient + "|" + idMedic + "|" +
           data + "|" + ora + "|" + cabinet + "|" +
           statusProgToString(status) + "|" + descriere;
}

void Programare::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 7) return;
    idProgramare = tokens[0];
    idPacient    = tokens[1];
    idMedic      = tokens[2];
    data         = tokens[3];
    ora          = tokens[4];
    cabinet      = tokens[5];
    status       = stringToStatusProg(tokens[6]);
    descriere    = (tokens.size() > 7) ? tokens[7] : "";
}
