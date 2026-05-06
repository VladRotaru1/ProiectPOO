#include "PersonalAuxiliar.h"
#include <sstream>
#include <iostream>

std::string tipAuxToString(TipPersonalAuxiliar t) {
    switch (t) {
        case TipPersonalAuxiliar::INFIRMIER:    return "INFIRMIER";
        case TipPersonalAuxiliar::BRANCARDIER:  return "BRANCARDIER";
        case TipPersonalAuxiliar::RECEPTIE:     return "RECEPTIE";
        case TipPersonalAuxiliar::ADMINISTRATIV: return "ADMINISTRATIV";
    }
    return "ADMINISTRATIV";
}

TipPersonalAuxiliar stringToTipAux(const std::string& s) {
    if (s == "INFIRMIER")    return TipPersonalAuxiliar::INFIRMIER;
    if (s == "BRANCARDIER")  return TipPersonalAuxiliar::BRANCARDIER;
    if (s == "RECEPTIE")     return TipPersonalAuxiliar::RECEPTIE;
    return TipPersonalAuxiliar::ADMINISTRATIV;
}

PersonalAuxiliar::PersonalAuxiliar()
    : Angajat(), tip(TipPersonalAuxiliar::ADMINISTRATIV), departament("") {}

PersonalAuxiliar::PersonalAuxiliar(const std::string& id_, const std::string& nume_,
                                   const std::string& prenume_, const std::string& cnp_,
                                   const std::string& sex_, const std::string& dataNasterii_,
                                   const std::string& telefon_, const std::string& email_,
                                   const std::string& adresa_, const std::string& dataAngajarii_,
                                   double salariuBrut_, TipPersonalAuxiliar tip_,
                                   const std::string& departament_)
    : Angajat(id_, nume_, prenume_, cnp_, sex_, dataNasterii_,
              telefon_, email_, adresa_, dataAngajarii_, salariuBrut_),
      tip(tip_), departament(departament_) {}

void PersonalAuxiliar::afisare() const {
    std::cout << "[AUX] " << codAngajat << " | " << prenume << " " << nume
              << " | Tip: " << tipAuxToString(tip)
              << " | Departament: " << departament
              << " | Salariu net: " << calculeazaSalariuNet() << " RON\n";
}

std::string PersonalAuxiliar::serializa() const {
    return Angajat::serializa() + "|" + tipAuxToString(tip) + "|" + departament;
}

void PersonalAuxiliar::deserializa(const std::string& linie) {
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

    if (tokens.size() > 15) tip = stringToTipAux(tokens[15]);
    if (tokens.size() > 16) departament = tokens[16];
}

PersonalAuxiliar PersonalAuxiliar::citesteDeLaConsola() {
    std::string id, nume, prenume, cnp, sex, dataNasterii, telefon, email, adresa;
    std::string dataAngajarii, departament;
    double salariuBrut;
    int tipInt;

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
    std::cout << "  Tip (0=Infirmier, 1=Brancardier, 2=Receptie, 3=Administrativ): "; std::cin >> tipInt;
    std::cin >> std::ws;
    std::cout << "  Departament: "; std::getline(std::cin, departament);

    if (!Persoana::validareCNP(cnp)) throw CNPInvalidException(cnp);

    static int idC = 0;
    id = "AUX" + std::to_string(++idC);
    return PersonalAuxiliar(id, nume, prenume, cnp, sex, dataNasterii,
                            telefon, email, adresa, dataAngajarii, salariuBrut,
                            (TipPersonalAuxiliar)tipInt, departament);
}
