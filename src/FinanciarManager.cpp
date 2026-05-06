#include "FinanciarManager.h"
#include <sstream>

int Factura::contor = 0;

Factura::Factura()
    : idFactura(""), idPacient(""), idInternare(""), sumaTotala(0),
      sumaCAS(0), sumaPacient(0), dataEmiterii(""), platita(false), descriere("") {}

Factura::Factura(const std::string& idPacient_, const std::string& idInternare_,
                 double suma_, bool asiguratCAS_, const std::string& data_)
    : idPacient(idPacient_), idInternare(idInternare_), sumaTotala(suma_),
      dataEmiterii(data_), platita(false), descriere("")
{
    idFactura = "FAC" + std::to_string(++contor);
    if (asiguratCAS_) {
        // Presupunem decontare 80% din suma pentru internari, etc.
        sumaCAS = sumaTotala * 0.8;
        sumaPacient = sumaTotala - sumaCAS;
    } else {
        sumaCAS = 0;
        sumaPacient = sumaTotala;
    }
}

std::string Factura::serializa() const {
    return idFactura + "|" + idPacient + "|" + idInternare + "|" +
           std::to_string(sumaTotala) + "|" + std::to_string(sumaCAS) + "|" +
           std::to_string(sumaPacient) + "|" + dataEmiterii + "|" +
           (platita ? "1" : "0") + "|" + descriere;
}

void Factura::deserializa(const std::string& linie) {
    std::istringstream iss(linie);
    std::string tok;
    std::vector<std::string> tokens;
    while (std::getline(iss, tok, '|')) tokens.push_back(tok);
    if (tokens.size() < 8) return;
    idFactura   = tokens[0];
    idPacient   = tokens[1];
    idInternare = tokens[2];
    sumaTotala  = std::stod(tokens[3]);
    sumaCAS     = std::stod(tokens[4]);
    sumaPacient = std::stod(tokens[5]);
    dataEmiterii= tokens[6];
    platita     = (tokens[7] == "1");
    if (tokens.size() > 8) descriere = tokens[8];
}

void Factura::afisare() const {
    std::cout << "[FACTURA] " << idFactura
              << " | Pacient: " << idPacient
              << " | Internare: " << (idInternare.empty() ? "-" : idInternare)
              << " | Total: " << sumaTotala << " RON"
              << " | CAS: " << sumaCAS << " RON"
              << " | De plata: " << sumaPacient << " RON"
              << " | Stare: " << (platita ? "PLATITA" : "NEPLATITA") << "\n";
}

// ============================================================
// FinanciarManager
// ============================================================
FinanciarManager::FinanciarManager(const std::string& fisier)
    : fisierFacturi(fisier), totalVenituri(0), totalCheltuieli(0) {
    incarca();
}

Factura FinanciarManager::emiteFactura(const std::string& idPacient, const std::string& idInternare,
                                       double suma, bool asiguratCAS, const std::string& data,
                                       const std::string& descriere) {
    Factura f(idPacient, idInternare, suma, asiguratCAS, data);
    f.descriere = descriere;
    facturi.push_back(f);
    salveaza();
    return f;
}

bool FinanciarManager::marcheazaPlatita(const std::string& idFactura) {
    Factura* f = gaseste(idFactura);
    if (f && !f->platita) {
        f->platita = true;
        totalVenituri += f->sumaPacient;
        salveaza();
        return true;
    }
    return false;
}

Factura* FinanciarManager::gaseste(const std::string& idFactura) {
    for (auto& f : facturi)
        if (f.idFactura == idFactura) return &f;
    return nullptr;
}

std::vector<Factura> FinanciarManager::getFacturiPacient(const std::string& idPacient) const {
    std::vector<Factura> rez;
    for (const auto& f : facturi)
        if (f.idPacient == idPacient) rez.push_back(f);
    return rez;
}

std::vector<Factura> FinanciarManager::getFacturiNePlatite() const {
    std::vector<Factura> rez;
    for (const auto& f : facturi)
        if (!f.platita) rez.push_back(f);
    return rez;
}

double FinanciarManager::getTotalVenituri() const {
    return totalVenituri;
}

double FinanciarManager::getTotalDecontatCAS() const {
    double totalCAS = 0;
    for (const auto& f : facturi)
        totalCAS += f.sumaCAS;
    return totalCAS;
}

double FinanciarManager::getTotalIncasat() const {
    double incasat = 0;
    for (const auto& f : facturi)
        if (f.platita) incasat += f.sumaPacient;
    return incasat;
}

void FinanciarManager::adaugaCheltuiala(double suma, const std::string& descriere) {
    totalCheltuieli += suma;
    // Nu persistam momentan cheltuielile detaliate, doar totalul,
    // dar se poate adauga logica in viitor
}

void FinanciarManager::afisareRaport() const {
    std::cout << "\n========== RAPORT FINANCIAR ==========\n";
    std::cout << "Total facturat (inclusiv neplatite): " << getTotalDecontatCAS() + getTotalIncasat() << " RON\n";
    std::cout << "Total CAS decontabil: " << getTotalDecontatCAS() << " RON\n";
    std::cout << "Total incasat pacienti: " << getTotalIncasat() << " RON\n";
    std::cout << "Facturi emise: " << facturi.size() << "\n";
    std::cout << "Cheltuieli estimate: " << totalCheltuieli << " RON\n";
    std::cout << "PROFIT ESTIMAT: " << (getTotalIncasat() + getTotalDecontatCAS() - totalCheltuieli) << " RON\n";
    std::cout << "======================================\n";
}

void FinanciarManager::afisareFacturi() const {
    for (const auto& f : facturi)
        f.afisare();
}

void FinanciarManager::afisareFacturiNePlatite() const {
    for (const auto& f : facturi)
        if (!f.platita) f.afisare();
}

void FinanciarManager::salveaza() const {
    std::ofstream ofs(fisierFacturi);
    if (!ofs.is_open()) return;
    for (const auto& f : facturi)
        ofs << f.serializa() << "\n";
}

void FinanciarManager::incarca() {
    std::ifstream ifs(fisierFacturi);
    if (!ifs.is_open()) return;
    facturi.clear();
    std::string linie;
    totalVenituri = 0;
    while (std::getline(ifs, linie)) {
        if (linie.empty()) continue;
        Factura f;
        f.deserializa(linie);
        facturi.push_back(f);
        if (f.platita)
            totalVenituri += f.sumaPacient;
    }
}
