#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// ============================================================
// Factura pacient
// ============================================================
struct Factura {
    std::string idFactura;
    std::string idPacient;
    std::string idInternare;
    double sumaTotala;
    double sumaCAS;       // decontata de CAS
    double sumaPacient;   // de platit de pacient
    std::string dataEmiterii;
    bool platita;
    std::string descriere;

    static int contor;

    Factura();
    Factura(const std::string& idPacient, const std::string& idInternare,
            double suma, bool asiguratCAS, const std::string& data);
    std::string serializa() const;
    void deserializa(const std::string& linie);
    void afisare() const;
};

// ============================================================
// FinanciarManager
// ============================================================
class FinanciarManager {
private:
    std::vector<Factura> facturi;
    std::string fisierFacturi;
    double totalVenituri;
    double totalCheltuieli;

public:
    explicit FinanciarManager(const std::string& fisier = "facturi.txt");
    ~FinanciarManager() = default;

    // Emitere factura
    Factura emiteFactura(const std::string& idPacient, const std::string& idInternare,
                         double suma, bool asiguratCAS, const std::string& data,
                         const std::string& descriere = "");

    // Marcare platita
    bool marcheazaPlatita(const std::string& idFactura);

    // Getters
    Factura* gaseste(const std::string& idFactura);
    std::vector<Factura> getFacturiPacient(const std::string& idPacient) const;
    std::vector<Factura> getFacturiNePlatite() const;

    // Rapoarte financiare
    double getTotalVenituri() const;
    double getTotalDecontatCAS() const;
    double getTotalIncasat() const;
    void adaugaCheltuiala(double suma, const std::string& descriere);
    double getTotalCheltuieli() const { return totalCheltuieli; }
    double getProfit() const { return getTotalVenituri() - totalCheltuieli; }

    void afisareRaport() const;
    void afisareFacturi() const;
    void afisareFacturiNePlatite() const;

    // Persistenta
    void salveaza() const;
    void incarca();
};
