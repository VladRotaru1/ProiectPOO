#include "Pat.h"
#include <sstream>
#include "Exceptii.h"
#include <vector>

Pat::Pat() : codPat(""), ocupat(false), idPacientCurent(""), igienizat(true) {}

Pat::Pat(const std::string& cod) : codPat(cod), ocupat(false), idPacientCurent(""), igienizat(true) {}

void Pat::ocupa(const std::string& idPacient) {
    if (ocupat)
        throw PatIndisponibilException(codPat);
    if (!igienizat)
        throw SpitalException("Patul " + codPat + " nu a fost igienizat");
    ocupat = true;
    idPacientCurent = idPacient;
    igienizat = false;  // dupa ocupare necesita igienizare la eliberare
}

void Pat::elibereaza() {
    ocupat = false;
    idPacientCurent = "";
    igienizat = false;  // necesita igienizare
}

void Pat::marcheazaIgienizat() {
    if (!ocupat) igienizat = true;
}

void Pat::marcheazaNecesitaIgienizare() {
    igienizat = false;
}

void Pat::afisare() const {
    std::cout << "  Pat " << codPat
              << " | " << (ocupat ? "OCUPAT (Pacient: " + idPacientCurent + ")" : "LIBER")
              << " | Igienizat: " << (igienizat ? "DA" : "NU") << "\n";
}

std::string Pat::serializa() const {
    return codPat + "~" + (ocupat ? "1" : "0") + "~" +
           idPacientCurent + "~" + (igienizat ? "1" : "0");
}

void Pat::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '~')) tokens.push_back(tok);
    if (tokens.size() >= 4) {
        codPat           = tokens[0];
        ocupat           = (tokens[1] == "1");
        idPacientCurent  = tokens[2];
        igienizat        = (tokens[3] == "1");
    }
}
