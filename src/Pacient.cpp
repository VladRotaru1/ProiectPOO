#include "Pacient.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// ============================================================
// Helper functions pentru PrioritateTriage
// ============================================================
std::string prioritateToString(PrioritateTriage p) {
    switch (p) {
        case PrioritateTriage::ROSU:       return "ROSU";
        case PrioritateTriage::PORTOCALIU: return "PORTOCALIU";
        case PrioritateTriage::GALBEN:     return "GALBEN";
        case PrioritateTriage::VERDE:      return "VERDE";
    }
    return "VERDE";
}

PrioritateTriage stringToPrioritate(const std::string& s) {
    if (s == "ROSU")       return PrioritateTriage::ROSU;
    if (s == "PORTOCALIU") return PrioritateTriage::PORTOCALIU;
    if (s == "GALBEN")     return PrioritateTriage::GALBEN;
    return PrioritateTriage::VERDE;
}

// ============================================================
// IntrareIstoricMedical
// ============================================================
std::string IntrareIstoricMedical::serializa() const {
    return data + "~" + diagnostic + "~" + tratament + "~" +
           medicResponsabil + "~" + observatii;
}

IntrareIstoricMedical IntrareIstoricMedical::deserializa(const std::string& linie) {
    IntrareIstoricMedical i;
    std::istringstream iss(linie);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '~'))
        tokens.push_back(token);
    if (tokens.size() >= 5) {
        i.data = tokens[0];
        i.diagnostic = tokens[1];
        i.tratament = tokens[2];
        i.medicResponsabil = tokens[3];
        i.observatii = tokens[4];
    }
    return i;
}

// ============================================================
// Pacient
// ============================================================
int Pacient::contorPacienti = 0;

Pacient::Pacient()
    : Persoana(), codPacient(""), asiguratCAS(false),
      grupaSanguina(""), contactUrgenta(""),
      prioritateTriage(PrioritateTriage::VERDE), numarVizite(0) {}

Pacient::Pacient(const std::string& id_, const std::string& nume_,
                 const std::string& prenume_, const std::string& cnp_,
                 const std::string& sex_, const std::string& dataNasterii_,
                 const std::string& telefon_, const std::string& email_,
                 const std::string& adresa_, bool asiguratCAS_,
                 const std::string& grupaSanguina_, const std::string& contactUrgenta_)
    : Persoana(id_, nume_, prenume_, cnp_, sex_, dataNasterii_, telefon_, email_, adresa_),
      asiguratCAS(asiguratCAS_), grupaSanguina(grupaSanguina_),
      contactUrgenta(contactUrgenta_),
      prioritateTriage(PrioritateTriage::VERDE), numarVizite(0)
{
    codPacient = "PAC" + std::to_string(++contorPacienti);
    if (!id_.empty()) codPacient = id_;
}

void Pacient::adaugaAlergie(const std::string& alergie) {
    // Nu duplicam alergiile
    for (const auto& a : alergii)
        if (a == alergie) return;
    alergii.push_back(alergie);
}

void Pacient::adaugaBoalaCronica(const std::string& boala) {
    for (const auto& b : boliCronice)
        if (b == boala) return;
    boliCronice.push_back(boala);
}

void Pacient::adaugaIntrareIstoricMedical(const IntrareIstoricMedical& intrare) {
    istoricMedical.push_back(intrare);
}

bool Pacient::areAlergieLa(const std::string& substanta) const {
    for (const auto& a : alergii) {
        std::string al = a, sub = substanta;
        // Case-insensitive
        std::transform(al.begin(), al.end(), al.begin(), ::tolower);
        std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
        if (al.find(sub) != std::string::npos) return true;
    }
    return false;
}

void Pacient::afisareFisaMedicala() const {
    std::cout << "\n========== FISA MEDICALA ==========\n";
    std::cout << "Cod pacient: " << codPacient << "\n";
    std::cout << "Nume: " << prenume << " " << nume << "\n";
    std::cout << "CNP: " << cnp << " | Sex: " << sex
              << " | Varsta: " << varsta << " ani\n";
    std::cout << "Data nasterii: " << dataNasterii << "\n";
    std::cout << "Telefon: " << telefon << " | Email: " << email << "\n";
    std::cout << "Adresa: " << adresa << "\n";
    std::cout << "Asigurat CAS: " << (asiguratCAS ? "DA" : "NU") << "\n";
    std::cout << "Grupa sanguina: " << grupaSanguina << "\n";
    std::cout << "Contact urgenta: " << contactUrgenta << "\n";
    std::cout << "Prioritate triage: " << prioritateToString(prioritateTriage) << "\n";
    std::cout << "Numar vizite: " << numarVizite << "\n";

    if (!alergii.empty()) {
        std::cout << "Alergii: ";
        for (size_t i = 0; i < alergii.size(); i++) {
            std::cout << alergii[i];
            if (i + 1 < alergii.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    if (!boliCronice.empty()) {
        std::cout << "Boli cronice: ";
        for (size_t i = 0; i < boliCronice.size(); i++) {
            std::cout << boliCronice[i];
            if (i + 1 < boliCronice.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    if (!istoricMedical.empty()) {
        std::cout << "\n--- Istoric Medical ---\n";
        for (const auto& intr : istoricMedical) {
            std::cout << "  [" << intr.data << "] " << intr.diagnostic
                      << " | Tratament: " << intr.tratament
                      << " | Medic: " << intr.medicResponsabil;
            if (!intr.observatii.empty())
                std::cout << " | Obs: " << intr.observatii;
            std::cout << "\n";
        }
    }
    std::cout << "===================================\n";
}

void Pacient::afisare() const {
    std::cout << "[PACIENT] " << codPacient << " | " << prenume << " " << nume
              << " | " << varsta << " ani | "
              << (asiguratCAS ? "CAS" : "Neasigurat")
              << " | Triage: " << prioritateToString(prioritateTriage)
              << " | Sange: " << grupaSanguina << "\n";
}

std::string Pacient::serializa() const {
    // Linie 1: date baza
    std::string baza = Persoana::serializa();
    // Alergii separate prin ';'
    std::string alergiiStr;
    for (size_t i = 0; i < alergii.size(); i++) {
        alergiiStr += alergii[i];
        if (i + 1 < alergii.size()) alergiiStr += ";";
    }
    // Boli cronice
    std::string boliStr;
    for (size_t i = 0; i < boliCronice.size(); i++) {
        boliStr += boliCronice[i];
        if (i + 1 < boliCronice.size()) boliStr += ";";
    }
    // Istoric
    std::string istoricStr;
    for (size_t i = 0; i < istoricMedical.size(); i++) {
        istoricStr += istoricMedical[i].serializa();
        if (i + 1 < istoricMedical.size()) istoricStr += "^^";
    }

    return baza + "|" + codPacient + "|" + (asiguratCAS ? "1" : "0") + "|" +
           grupaSanguina + "|" + alergiiStr + "|" + boliStr + "|" +
           contactUrgenta + "|" + prioritateToString(prioritateTriage) + "|" +
           std::to_string(numarVizite) + "|" + istoricStr;
}

void Pacient::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|'))
        tokens.push_back(token);

    // primele 9 tokenuri = Persoana (id, nume, prenume, cnp_cript, sex, dataNasterii, tel, email, adresa)
    if (tokens.size() < 10) return;

    // reconstituim linia de baza pentru Persoana::deserializa
    std::string bazaLinie = tokens[0] + "|" + tokens[1] + "|" + tokens[2] + "|" +
                            tokens[3] + "|" + tokens[4] + "|" + tokens[5] + "|" +
                            tokens[6] + "|" + tokens[7] + "|" + tokens[8];
    Persoana::deserializa(bazaLinie);

    if (tokens.size() > 9)  codPacient  = tokens[9];
    if (tokens.size() > 10) asiguratCAS = (tokens[10] == "1");
    if (tokens.size() > 11) grupaSanguina = tokens[11];
    if (tokens.size() > 12) {
        alergii.clear();
        if (!tokens[12].empty()) {
            std::istringstream issA(tokens[12]);
            std::string a;
            while (std::getline(issA, a, ';'))
                if (!a.empty()) alergii.push_back(a);
        }
    }
    if (tokens.size() > 13) {
        boliCronice.clear();
        if (!tokens[13].empty()) {
            std::istringstream issB(tokens[13]);
            std::string b;
            while (std::getline(issB, b, ';'))
                if (!b.empty()) boliCronice.push_back(b);
        }
    }
    if (tokens.size() > 14) contactUrgenta = tokens[14];
    if (tokens.size() > 15) prioritateTriage = stringToPrioritate(tokens[15]);
    if (tokens.size() > 16) numarVizite = std::stoi(tokens[16]);
    if (tokens.size() > 17 && !tokens[17].empty()) {
        istoricMedical.clear();
        std::istringstream issI(tokens[17]);
        std::string intrare;
        while (std::getline(issI, intrare, '^')) {
            // separator "^^" => doua caractere '^'
            if (!intrare.empty())
                istoricMedical.push_back(IntrareIstoricMedical::deserializa(intrare));
        }
    }
}

Pacient Pacient::citesteDeLaConsola() {
    Pacient p;
    std::string id, nume, prenume, cnp, sex, dataNasterii, telefon, email, adresa;
    std::string grupaSanguina, contactUrgenta;
    int asiguratInt;

    std::cout << "  Prenume: "; std::cin >> std::ws; std::getline(std::cin, prenume);
    std::cout << "  Nume: ";    std::getline(std::cin, nume);
    std::cout << "  CNP (13 cifre): "; std::cin >> cnp;
    std::cout << "  Sex (M/F): "; std::cin >> sex;
    std::cout << "  Data nasterii (ZZ.LL.AAAA): "; std::cin >> dataNasterii;
    std::cin >> std::ws;
    std::cout << "  Telefon: "; std::getline(std::cin, telefon);
    std::cout << "  Email: ";   std::getline(std::cin, email);
    std::cout << "  Adresa: ";  std::getline(std::cin, adresa);
    std::cout << "  Grupa sanguina (ex: A+): "; std::cin >> grupaSanguina;
    std::cout << "  Asigurat CAS (1/0): "; std::cin >> asiguratInt;
    std::cin >> std::ws;
    std::cout << "  Contact urgenta: "; std::getline(std::cin, contactUrgenta);

    // Validam CNP
    if (!Persoana::validareCNP(cnp)) {
        throw CNPInvalidException(cnp);
    }

    static int idCounter = 0;
    id = "PAC" + std::to_string(++idCounter);

    p = Pacient(id, nume, prenume, cnp, sex, dataNasterii,
                telefon, email, adresa, (asiguratInt == 1), grupaSanguina, contactUrgenta);

    // Alergii optionale
    std::cout << "  Numar alergii: ";
    int nrAlergii; std::cin >> nrAlergii;
    std::cin >> std::ws;
    for (int i = 0; i < nrAlergii; i++) {
        std::cout << "  Alergie " << (i+1) << ": ";
        std::string al; std::getline(std::cin, al);
        p.adaugaAlergie(al);
    }

    // Boli cronice
    std::cout << "  Numar boli cronice: ";
    int nrBoli; std::cin >> nrBoli;
    std::cin >> std::ws;
    for (int i = 0; i < nrBoli; i++) {
        std::cout << "  Boala cronica " << (i+1) << ": ";
        std::string b; std::getline(std::cin, b);
        p.adaugaBoalaCronica(b);
    }

    return p;
}
