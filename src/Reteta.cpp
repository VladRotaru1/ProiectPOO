#include "Reteta.h"
#include <sstream>
#include <fstream>
#include <ctime>

int Reteta::contorRetete = 0;

Reteta::Reteta()
    : idReteta(""), idConsultatie(""), idPacient(""),
      codParafaMedic(""), dataPrescrierii(""), eliberata(false) {}

Reteta::Reteta(const std::string& id_, const std::string& idConsultatie_,
               const std::string& idPacient_, const std::string& codParafa_,
               const std::string& data_)
    : idReteta(id_), idConsultatie(idConsultatie_), idPacient(idPacient_),
      codParafaMedic(codParafa_), dataPrescrierii(data_), eliberata(false) {}

void Reteta::adaugaMedicament(const MedicamentReteta& med) {
    medicamente.push_back(med);
}

void Reteta::genereazaFisier(const std::string& numePacient, const std::string& numeMedic) const {
    std::string numeFisier = "reteta_" + idReteta + ".txt";
    std::ofstream ofs(numeFisier);
    if (!ofs.is_open()) {
        std::cout << "  Nu s-a putut genera fisierul reteta.\n";
        return;
    }
    ofs << "============================================\n";
    ofs << "           RETETA MEDICALA ELECTRONICA\n";
    ofs << "============================================\n";
    ofs << "ID Reteta: " << idReteta << "\n";
    ofs << "Data: " << dataPrescrierii << "\n";
    ofs << "Pacient: " << numePacient << "\n";
    ofs << "Medic prescriptor: " << numeMedic << "\n";
    ofs << "Cod parafa: " << codParafaMedic << "\n";
    ofs << "--------------------------------------------\n";
    ofs << "MEDICAMENTE PRESCRISE:\n";
    for (int i = 0; i < (int)medicamente.size(); i++) {
        const auto& m = medicamente[i];
        ofs << "\n  " << (i+1) << ". " << m.denumireMedicament << "\n";
        ofs << "     Dozaj: " << m.dozaj << "\n";
        ofs << "     Durata: " << m.numarZile << " zile\n";
        if (!m.observatii.empty())
            ofs << "     Observatii: " << m.observatii << "\n";
    }
    ofs << "\n--------------------------------------------\n";
    ofs << "Semnatura medic: ________________\n";
    ofs << "Stampila unitate: ________________\n";
    ofs << "============================================\n";
    ofs.close();
    std::cout << "  Reteta generata: " << numeFisier << "\n";
}

void Reteta::afisare() const {
    std::cout << "[RETETA] " << idReteta
              << " | Pacient: " << idPacient
              << " | Data: " << dataPrescrierii
              << " | Parafa: " << codParafaMedic
              << " | " << medicamente.size() << " medicamente"
              << " | " << (eliberata ? "ELIBERATA" : "NEELIBERATA") << "\n";
    for (const auto& m : medicamente) {
        std::cout << "   - " << m.denumireMedicament
                  << " | " << m.dozaj
                  << " | " << m.numarZile << " zile\n";
    }
}

std::string Reteta::serializa() const {
    std::string medStr;
    for (size_t i = 0; i < medicamente.size(); i++) {
        const auto& m = medicamente[i];
        medStr += m.denumireMedicament + "~" + m.dozaj + "~" +
                  std::to_string(m.numarZile) + "~" + m.observatii;
        if (i + 1 < medicamente.size()) medStr += "^^";
    }
    return idReteta + "|" + idConsultatie + "|" + idPacient + "|" +
           codParafaMedic + "|" + dataPrescrierii + "|" +
           (eliberata ? "1" : "0") + "|" + medStr;
}

void Reteta::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 6) return;
    idReteta        = tokens[0];
    idConsultatie   = tokens[1];
    idPacient       = tokens[2];
    codParafaMedic  = tokens[3];
    dataPrescrierii = tokens[4];
    eliberata       = (tokens[5] == "1");
    if (tokens.size() > 6 && !tokens[6].empty()) {
        medicamente.clear();
        std::istringstream issM(tokens[6]);
        std::string mStr;
        while (std::getline(issM, mStr, '^')) {
            if (mStr.empty()) continue;
            std::istringstream issOne(mStr);
            std::string t;
            std::vector<std::string> mv;
            while (std::getline(issOne, t, '~')) mv.push_back(t);
            if (mv.size() >= 3) {
                MedicamentReteta mr;
                mr.denumireMedicament = mv[0];
                mr.dozaj              = mv[1];
                mr.numarZile          = std::stoi(mv[2]);
                mr.observatii         = (mv.size() > 3) ? mv[3] : "";
                medicamente.push_back(mr);
            }
        }
    }
}
