#include "Angajat.h"
#include "Exceptii.h"
#include <sstream>
#include <iostream>
#include <ctime>

// ============================================================
// Tura
// ============================================================
std::string Tura::serializa() const {
    return data + "~" + tipTura + "~" + std::to_string(oreDurata);
}

Tura Tura::deserializa(const std::string& s) {
    Tura t;
    std::istringstream iss(s);
    std::string tok;
    std::vector<std::string> v;
    while (std::getline(iss, tok, '~')) v.push_back(tok);
    if (v.size() >= 3) {
        t.data     = v[0];
        t.tipTura  = v[1];
        t.oreDurata = std::stoi(v[2]);
    }
    return t;
}

// ============================================================
// Angajat
// ============================================================
int Angajat::contorAngajati = 0;

Angajat::Angajat()
    : Persoana(), codAngajat(""), dataAngajarii(""),
      salariuBrut(0), salariuNet(0), sporuri(0), activ(true) {}

Angajat::Angajat(const std::string& id_, const std::string& nume_,
                 const std::string& prenume_, const std::string& cnp_,
                 const std::string& sex_, const std::string& dataNasterii_,
                 const std::string& telefon_, const std::string& email_,
                 const std::string& adresa_, const std::string& dataAngajarii_,
                 double salariuBrut_)
    : Persoana(id_, nume_, prenume_, cnp_, sex_, dataNasterii_, telefon_, email_, adresa_),
      dataAngajarii(dataAngajarii_), salariuBrut(salariuBrut_),
      salariuNet(0), sporuri(0), activ(true)
{
    if (salariuBrut_ < 0)
        throw SalariuInvalidException(salariuBrut_);
    codAngajat = id_;
    recalculeazaSalariu();
}

int Angajat::getAniExperienta() const {
    if (dataAngajarii.size() < 4) return 0;
    int anAngajare = std::stoi(dataAngajarii.substr(0, 4));
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    int anCurent = 1900 + t->tm_year;
    int ani = anCurent - anAngajare;
    return (ani < 0) ? 0 : ani;
}

void Angajat::setSalariuBrut(double s) {
    if (s < 0) throw SalariuInvalidException(s);
    salariuBrut = s;
    recalculeazaSalariu();
}

double Angajat::calculeazaSalariuNet() const {
    // Contributii angajat: CAS 25%, CASS 10%, impozit 10% pe venit impozabil
    double contributiiAngajat = salariuBrut * 0.35;
    double venitImpozabil = salariuBrut - contributiiAngajat;
    double impozit = venitImpozabil * 0.10;
    return venitImpozabil - impozit + sporuri;
}

double Angajat::calculeazaSporNoapte() const {
    // Spor noapte 25% din salariu brut / ora, pentru fiecare tura de noapte
    double salariuOrar = salariuBrut / 168.0; // 168 ore/luna
    double spor = 0;
    for (const auto& t : ture) {
        if (t.tipTura == "noapte") {
            spor += salariuOrar * t.oreDurata * 0.25;
        }
    }
    return spor;
}

void Angajat::recalculeazaSalariu() {
    sporuri = calculeazaSporNoapte();
    salariuNet = calculeazaSalariuNet();
}

void Angajat::adaugaTura(const Tura& t) {
    ture.push_back(t);
    recalculeazaSalariu();
}

void Angajat::adaugaCertificare(const std::string& cert) {
    for (const auto& c : certificari)
        if (c == cert) return;
    certificari.push_back(cert);
}

void Angajat::afisare() const {
    std::cout << "[ANGAJAT] " << codAngajat << " | " << prenume << " " << nume
              << " | Salariu net: " << salariuNet << " RON"
              << " | Experienta: " << getAniExperienta() << " ani"
              << " | " << (activ ? "ACTIV" : "INACTIV") << "\n";
}

std::string Angajat::serializa() const {
    std::string baza = Persoana::serializa();
    // Ture
    std::string tureStr;
    for (size_t i = 0; i < ture.size(); i++) {
        tureStr += ture[i].serializa();
        if (i + 1 < ture.size()) tureStr += "^^";
    }
    // Certificari
    std::string certStr;
    for (size_t i = 0; i < certificari.size(); i++) {
        certStr += certificari[i];
        if (i + 1 < certificari.size()) certStr += ";";
    }
    return baza + "|" + codAngajat + "|" + dataAngajarii + "|" +
           std::to_string(salariuBrut) + "|" + (activ ? "1" : "0") + "|" +
           tureStr + "|" + certStr;
}

void Angajat::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|'))
        tokens.push_back(token);

    if (tokens.size() < 9) return;

    std::string bazaLinie = tokens[0] + "|" + tokens[1] + "|" + tokens[2] + "|" +
                            tokens[3] + "|" + tokens[4] + "|" + tokens[5] + "|" +
                            tokens[6] + "|" + tokens[7] + "|" + tokens[8];
    Persoana::deserializa(bazaLinie);

    if (tokens.size() > 9)  codAngajat    = tokens[9];
    if (tokens.size() > 10) dataAngajarii = tokens[10];
    if (tokens.size() > 11) salariuBrut   = std::stod(tokens[11]);
    if (tokens.size() > 12) activ = (tokens[12] == "1");
    if (tokens.size() > 13 && !tokens[13].empty()) {
        ture.clear();
        std::istringstream issT(tokens[13]);
        std::string tStr;
        while (std::getline(issT, tStr, '^'))
            if (!tStr.empty()) ture.push_back(Tura::deserializa(tStr));
    }
    if (tokens.size() > 14 && !tokens[14].empty()) {
        certificari.clear();
        std::istringstream issC(tokens[14]);
        std::string c;
        while (std::getline(issC, c, ';'))
            if (!c.empty()) certificari.push_back(c);
    }
    recalculeazaSalariu();
}

Angajat Angajat::citesteDeLaConsola() {
    std::string id, nume, prenume, cnp, sex, dataNasterii, telefon, email, adresa, dataAngajarii;
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

    if (!Persoana::validareCNP(cnp)) throw CNPInvalidException(cnp);

    static int idC = 0;
    id = "ANG" + std::to_string(++idC);

    return Angajat(id, nume, prenume, cnp, sex, dataNasterii,
                   telefon, email, adresa, dataAngajarii, salariuBrut);
}
