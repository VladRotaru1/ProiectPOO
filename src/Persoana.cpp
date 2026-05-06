#include "Persoana.h"
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <vector>

int Persoana::contor = 0;

std::string Persoana::genereazaId(const std::string& prefix) {
    return prefix + std::to_string(++contor);
}

Persoana::Persoana()
    : id(""), nume(""), prenume(""), cnp(""), sex(""),
      dataNasterii(""), varsta(0), telefon(""), email(""), adresa("") {}

Persoana::Persoana(const std::string& id_, const std::string& nume_,
                   const std::string& prenume_, const std::string& cnp_,
                   const std::string& sex_, const std::string& dataNasterii_,
                   const std::string& telefon_, const std::string& email_,
                   const std::string& adresa_)
    : id(id_), nume(nume_), prenume(prenume_), cnp(cnp_), sex(sex_),
      dataNasterii(dataNasterii_), varsta(0),
      telefon(telefon_), email(email_), adresa(adresa_)
{
    if (!cnp_.empty()) {
        if (!validareCNP(cnp_))
            throw CNPInvalidException(cnp_);
        varsta = calculeazaVarsta(cnp_);
    }
}

void Persoana::setCnp(const std::string& c) {
    if (!validareCNP(c))
        throw CNPInvalidException(c);
    cnp = c;
    varsta = calculeazaVarsta(c);
}

bool Persoana::validareCNP(const std::string& cnp) {
    if (cnp.size() != 13) return false;
    for (char c : cnp)
        if (c < '0' || c > '9') return false;
    if (cnp[0] < '1' || cnp[0] > '9') return false;
    // Verificare luna
    int luna = (cnp[3] - '0') * 10 + (cnp[4] - '0');
    if (luna < 1 || luna > 12) return false;
    // Verificare zi
    int zi = (cnp[5] - '0') * 10 + (cnp[6] - '0');
    if (zi < 1 || zi > 31) return false;
    // Suma de control
    const int coeficienti[] = {2, 7, 9, 1, 4, 6, 3, 5, 8, 2, 7, 9};
    int suma = 0;
    for (int i = 0; i < 12; i++)
        suma += (cnp[i] - '0') * coeficienti[i];
    int rest = suma % 11;
    int cifraControl = (rest == 10) ? 1 : rest;
    return cifraControl == (cnp[12] - '0');
}

int Persoana::calculeazaVarsta(const std::string& cnp) {
    if (cnp.size() < 7) return 0;
    int secol = 0;
    char prima = cnp[0];
    if (prima == '1' || prima == '2') secol = 1900;
    else if (prima == '3' || prima == '4') secol = 1800;
    else if (prima == '5' || prima == '6') secol = 2000;
    else secol = 1900; // rezidenti straini / alte cazuri

    int an = secol + (cnp[1] - '0') * 10 + (cnp[2] - '0');
    int luna = (cnp[3] - '0') * 10 + (cnp[4] - '0');
    int zi = (cnp[5] - '0') * 10 + (cnp[6] - '0');

    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    int anCurent = 1900 + t->tm_year;
    int lunaCurenta = 1 + t->tm_mon;
    int ziCurenta = t->tm_mday;

    int varsta = anCurent - an;
    if (lunaCurenta < luna || (lunaCurenta == luna && ziCurenta < zi))
        varsta--;
    return (varsta < 0) ? 0 : varsta;
}

std::string Persoana::cripteazaCNP(const std::string& cnp) {
    // XOR simplu cu cheia 0x5A
    std::string rezultat = cnp;
    for (char& c : rezultat)
        c = c ^ 0x5A;
    // Convertim in hex pentru stocare
    std::ostringstream oss;
    for (unsigned char c : rezultat)
        oss << std::hex << (int)c << ":";
    return oss.str();
}

std::string Persoana::decripteazaCNP(const std::string& cnpCriptat) {
    std::string rezultat;
    std::istringstream iss(cnpCriptat);
    std::string token;
    while (std::getline(iss, token, ':')) {
        if (!token.empty()) {
            int val = std::stoi(token, nullptr, 16);
            rezultat += (char)(val ^ 0x5A);
        }
    }
    return rezultat;
}

std::string Persoana::serializa() const {
    return id + "|" + nume + "|" + prenume + "|" +
           cripteazaCNP(cnp) + "|" + sex + "|" + dataNasterii + "|" +
           telefon + "|" + email + "|" + adresa;
}

void Persoana::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|'))
        tokens.push_back(token);
    if (tokens.size() >= 9) {
        id = tokens[0];
        nume = tokens[1];
        prenume = tokens[2];
        cnp = decripteazaCNP(tokens[3]);
        sex = tokens[4];
        dataNasterii = tokens[5];
        telefon = tokens[6];
        email = tokens[7];
        adresa = tokens[8];
        if (!cnp.empty() && validareCNP(cnp))
            varsta = calculeazaVarsta(cnp);
    }
}

std::ostream& operator<<(std::ostream& os, const Persoana& p) {
    os << p.prenume << " " << p.nume
       << " | CNP: " << p.cnp
       << " | Sex: " << p.sex
       << " | Varsta: " << p.varsta << " ani"
       << " | Tel: " << p.telefon;
    return os;
}
