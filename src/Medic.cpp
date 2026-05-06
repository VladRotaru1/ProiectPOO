#include "Medic.h"
#include <sstream>
#include <iostream>
#include <algorithm>

// ============================================================
// Helper functions GradProfesional
// ============================================================
std::string gradToString(GradProfesional g) {
    switch (g) {
        case GradProfesional::REZIDENT:        return "REZIDENT";
        case GradProfesional::SPECIALIST:      return "SPECIALIST";
        case GradProfesional::PRIMAR:          return "PRIMAR";
        case GradProfesional::PROFESOR_DOCTOR: return "PROFESOR_DOCTOR";
    }
    return "REZIDENT";
}

GradProfesional stringToGrad(const std::string& s) {
    if (s == "SPECIALIST")      return GradProfesional::SPECIALIST;
    if (s == "PRIMAR")          return GradProfesional::PRIMAR;
    if (s == "PROFESOR_DOCTOR") return GradProfesional::PROFESOR_DOCTOR;
    return GradProfesional::REZIDENT;
}

// ============================================================
// Medic
// ============================================================
int Medic::contorMedici = 0;

Medic::Medic()
    : Angajat(), specializare(""), grad(GradProfesional::REZIDENT),
      codParafa(""), program(""), rating(5.0),
      numarConsultatii(0), numarInterventii(0) {}

Medic::Medic(const std::string& id_, const std::string& nume_,
             const std::string& prenume_, const std::string& cnp_,
             const std::string& sex_, const std::string& dataNasterii_,
             const std::string& telefon_, const std::string& email_,
             const std::string& adresa_, const std::string& dataAngajarii_,
             double salariuBrut_, const std::string& specializare_,
             GradProfesional grad_, const std::string& codParafa_)
    : Angajat(id_, nume_, prenume_, cnp_, sex_, dataNasterii_,
              telefon_, email_, adresa_, dataAngajarii_, salariuBrut_),
      specializare(specializare_), grad(grad_), codParafa(codParafa_),
      program("Luni-Vineri 08:00-16:00"), rating(5.0),
      numarConsultatii(0), numarInterventii(0)
{
    recalculeazaSalariu();
}

double Medic::calculeazaSalariuNet() const {
    double net = Angajat::calculeazaSalariuNet();
    // Spor de grad: Rezident 0%, Specialist 10%, Primar 20%, Prof. Dr. 30%
    double sporGrad = 0;
    switch (grad) {
        case GradProfesional::SPECIALIST:      sporGrad = salariuBrut * 0.10; break;
        case GradProfesional::PRIMAR:          sporGrad = salariuBrut * 0.20; break;
        case GradProfesional::PROFESOR_DOCTOR: sporGrad = salariuBrut * 0.30; break;
        default: break;
    }
    return net + sporGrad;
}

void Medic::asigneazaPacient(const std::string& idPacient) {
    if (!arePacientAsignat(idPacient))
        idPacientiAsignati.push_back(idPacient);
}

void Medic::dezasigneazaPacient(const std::string& idPacient) {
    idPacientiAsignati.erase(
        std::remove(idPacientiAsignati.begin(), idPacientiAsignati.end(), idPacient),
        idPacientiAsignati.end());
}

bool Medic::arePacientAsignat(const std::string& idPacient) const {
    return std::find(idPacientiAsignati.begin(), idPacientiAsignati.end(), idPacient)
           != idPacientiAsignati.end();
}

void Medic::afisare() const {
    std::cout << "[MEDIC] " << codAngajat << " | " << prenume << " " << nume
              << " | Spec: " << specializare
              << " | Grad: " << gradToString(grad)
              << " | Parafa: " << codParafa
              << " | Rating: " << rating
              << " | Consultatii: " << numarConsultatii
              << " | Salariu net: " << calculeazaSalariuNet() << " RON\n";
}

std::string Medic::serializa() const {
    std::string baza = Angajat::serializa();
    // Pacienti asignati
    std::string pacientiStr;
    for (size_t i = 0; i < idPacientiAsignati.size(); i++) {
        pacientiStr += idPacientiAsignati[i];
        if (i + 1 < idPacientiAsignati.size()) pacientiStr += ";";
    }
    return baza + "|" + specializare + "|" + gradToString(grad) + "|" +
           codParafa + "|" + program + "|" + std::to_string(rating) + "|" +
           std::to_string(numarConsultatii) + "|" +
           std::to_string(numarInterventii) + "|" + pacientiStr;
}

void Medic::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|'))
        tokens.push_back(token);

    // Angajat::deserializa foloseste primele 15 tokenuri
    // Reconstituim linia angajat (primele 15 tokenuri)
    if (tokens.size() < 16) return;

    std::string angajatLinie;
    // Angajat serializa: 9 baza + 6 angajat = 15 campuri separate prin '|'
    for (int i = 0; i < 15 && i < (int)tokens.size(); i++) {
        if (i > 0) angajatLinie += "|";
        angajatLinie += tokens[i];
    }
    Angajat::deserializa(angajatLinie);

    if (tokens.size() > 15) specializare = tokens[15];
    if (tokens.size() > 16) grad = stringToGrad(tokens[16]);
    if (tokens.size() > 17) codParafa = tokens[17];
    if (tokens.size() > 18) program = tokens[18];
    if (tokens.size() > 19) rating = std::stod(tokens[19]);
    if (tokens.size() > 20) numarConsultatii = std::stoi(tokens[20]);
    if (tokens.size() > 21) numarInterventii = std::stoi(tokens[21]);
    if (tokens.size() > 22 && !tokens[22].empty()) {
        idPacientiAsignati.clear();
        std::istringstream issP(tokens[22]);
        std::string p;
        while (std::getline(issP, p, ';'))
            if (!p.empty()) idPacientiAsignati.push_back(p);
    }
    recalculeazaSalariu();
}

Medic Medic::citesteDeLaConsola() {
    std::string id, nume, prenume, cnp, sex, dataNasterii, telefon, email, adresa;
    std::string dataAngajarii, specializare, codParafa;
    double salariuBrut;
    int gradInt;

    std::cout << "  Prenume: "; std::cin >> std::ws; std::getline(std::cin, prenume);
    std::cout << "  Nume: ";    std::getline(std::cin, nume);
    std::cout << "  CNP (13 cifre): "; std::cin >> cnp;
    std::cout << "  Sex (M/F): "; std::cin >> sex;
    std::cout << "  Data nasterii (ZZ.LL.AAAA): "; std::cin >> dataNasterii;
    std::cout << "  Data angajarii (AAAA-LL-ZZ): "; std::cin >> dataAngajarii;
    std::cout << "  Salariu brut (RON): "; std::cin >> salariuBrut;
    std::cin >> std::ws;
    std::cout << "  Telefon: "; std::getline(std::cin, telefon);
    std::cout << "  Email: ";   std::getline(std::cin, email);
    std::cout << "  Adresa: ";  std::getline(std::cin, adresa);
    std::cout << "  Specializare: "; std::getline(std::cin, specializare);
    std::cout << "  Grad (0=Rezident, 1=Specialist, 2=Primar, 3=Prof.Dr.): "; std::cin >> gradInt;
    std::cout << "  Cod parafa: "; std::cin >> codParafa;

    if (!Persoana::validareCNP(cnp)) throw CNPInvalidException(cnp);

    static int idC = 0;
    id = "MED" + std::to_string(++idC);

    GradProfesional g = (GradProfesional)gradInt;
    return Medic(id, nume, prenume, cnp, sex, dataNasterii,
                 telefon, email, adresa, dataAngajarii, salariuBrut,
                 specializare, g, codParafa);
}
