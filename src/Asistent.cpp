#include "Asistent.h"
#include <sstream>
#include <iostream>
#include <algorithm>

int Asistent::contorAsistenti = 0;

Asistent::Asistent()
    : Angajat(), sectie(""), tipTura("zi") {}

Asistent::Asistent(const std::string& id_, const std::string& nume_,
                   const std::string& prenume_, const std::string& cnp_,
                   const std::string& sex_, const std::string& dataNasterii_,
                   const std::string& telefon_, const std::string& email_,
                   const std::string& adresa_, const std::string& dataAngajarii_,
                   double salariuBrut_, const std::string& sectie_, const std::string& tipTura_)
    : Angajat(id_, nume_, prenume_, cnp_, sex_, dataNasterii_,
              telefon_, email_, adresa_, dataAngajarii_, salariuBrut_),
      sectie(sectie_), tipTura(tipTura_) {}

void Asistent::adaugaCalificare(const std::string& c) {
    for (const auto& cal : calificari)
        if (cal == c) return;
    calificari.push_back(c);
}

void Asistent::asigneazaPacient(const std::string& idPacient) {
    if (std::find(idPacientiAsistati.begin(), idPacientiAsistati.end(), idPacient)
        == idPacientiAsistati.end())
        idPacientiAsistati.push_back(idPacient);
}

void Asistent::elibereazaPacient(const std::string& idPacient) {
    idPacientiAsistati.erase(
        std::remove(idPacientiAsistati.begin(), idPacientiAsistati.end(), idPacient),
        idPacientiAsistati.end());
}

void Asistent::afisare() const {
    std::cout << "[ASISTENT] " << codAngajat << " | " << prenume << " " << nume
              << " | Sectie: " << sectie
              << " | Tura: " << tipTura
              << " | Salariu net: " << calculeazaSalariuNet() << " RON\n";
}

std::string Asistent::serializa() const {
    std::string baza = Angajat::serializa();
    std::string calStr;
    for (size_t i = 0; i < calificari.size(); i++) {
        calStr += calificari[i];
        if (i + 1 < calificari.size()) calStr += ";";
    }
    std::string pacientiStr;
    for (size_t i = 0; i < idPacientiAsistati.size(); i++) {
        pacientiStr += idPacientiAsistati[i];
        if (i + 1 < idPacientiAsistati.size()) pacientiStr += ";";
    }
    return baza + "|" + sectie + "|" + tipTura + "|" + calStr + "|" + pacientiStr;
}

void Asistent::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|'))
        tokens.push_back(token);

    if (tokens.size() < 15) return;

    std::string angajatLinie;
    for (int i = 0; i < 15 && i < (int)tokens.size(); i++) {
        if (i > 0) angajatLinie += "|";
        angajatLinie += tokens[i];
    }
    Angajat::deserializa(angajatLinie);

    if (tokens.size() > 15) sectie  = tokens[15];
    if (tokens.size() > 16) tipTura = tokens[16];
    if (tokens.size() > 17 && !tokens[17].empty()) {
        calificari.clear();
        std::istringstream issC(tokens[17]);
        std::string c;
        while (std::getline(issC, c, ';'))
            if (!c.empty()) calificari.push_back(c);
    }
    if (tokens.size() > 18 && !tokens[18].empty()) {
        idPacientiAsistati.clear();
        std::istringstream issP(tokens[18]);
        std::string p;
        while (std::getline(issP, p, ';'))
            if (!p.empty()) idPacientiAsistati.push_back(p);
    }
}

Asistent Asistent::citesteDeLaConsola() {
    std::string id, nume, prenume, cnp, sex, dataNasterii, telefon, email, adresa;
    std::string dataAngajarii, sectie, tipTura;
    double salariuBrut;

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
    std::cout << "  Sectie: ";  std::getline(std::cin, sectie);
    std::cout << "  Tip tura (zi/noapte/rotatie): "; std::cin >> tipTura;

    if (!Persoana::validareCNP(cnp)) throw CNPInvalidException(cnp);

    static int idC = 0;
    id = "ASI" + std::to_string(++idC);
    return Asistent(id, nume, prenume, cnp, sex, dataNasterii,
                    telefon, email, adresa, dataAngajarii, salariuBrut, sectie, tipTura);
}
