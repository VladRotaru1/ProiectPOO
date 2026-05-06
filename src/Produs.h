#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// ============================================================
// Clasa abstracta Produs
// ============================================================
class Produs {
protected:
    std::string idProdus;
    std::string denumire;
    int cantitate;
    double pret;
    std::string termenExpirare;  // "AAAA-LL-ZZ"

    static int contorProduse;

public:
    Produs();
    Produs(const std::string& id, const std::string& denumire,
           int cantitate, double pret, const std::string& termenExpirare);
    virtual ~Produs() = default;

    // Getters
    std::string getIdProdus()        const { return idProdus; }
    std::string getDenumire()        const { return denumire; }
    int         getCantitate()       const { return cantitate; }
    double      getPret()            const { return pret; }
    std::string getTermenExpirare()  const { return termenExpirare; }

    // Setters
    void setDenumire(const std::string& d) { denumire = d; }
    void setPret(double p)                 { pret = p; }
    void setTermenExpirare(const std::string& t) { termenExpirare = t; }
    void setIdProdus(const std::string& id) { idProdus = id; }

    // Operatii stoc
    void adaugaStoc(int qty);
    void scadeStoc(int qty);   // throws StocInsuficientException
    bool esteExpirat() const;  // comparare cu data curenta
    bool stocCritic(int prag = 5) const;

    // Afisare virtuala
    virtual void afisare() const = 0;
    virtual std::string getTip() const = 0;

    virtual std::string serializa() const;
    virtual void deserializa(const std::string& linie);
};

// ============================================================
// Medicament : Produs
// ============================================================
class Medicament : public Produs {
private:
    std::string substActiva;
    std::string forma;          // "comprimate", "fiole", etc.
    std::string contraindicatii;
    bool necesitaReteta;
    std::string producator;

public:
    Medicament();
    Medicament(const std::string& id, const std::string& denumire,
               int cantitate, double pret, const std::string& termenExpirare,
               const std::string& substActiva, const std::string& forma,
               bool necesitaReteta, const std::string& producator = "");
    ~Medicament() override = default;

    std::string getSubstActiva()      const { return substActiva; }
    std::string getForma()            const { return forma; }
    std::string getContraindicatii()  const { return contraindicatii; }
    bool        getNecesitaReteta()   const { return necesitaReteta; }
    std::string getProducator()       const { return producator; }
    void setContraindicatii(const std::string& c) { contraindicatii = c; }

    void afisare() const override;
    std::string getTip() const override { return "MEDICAMENT"; }
    std::string serializa() const override;
    void deserializa(const std::string& linie) override;
};

// ============================================================
// Consumabil : Produs
// ============================================================
class Consumabil : public Produs {
private:
    std::string categorie;   // "seringi", "manusi", etc.
    std::string unitate;     // "bucati", "cutii", etc.

public:
    Consumabil();
    Consumabil(const std::string& id, const std::string& denumire,
               int cantitate, double pret, const std::string& termenExpirare,
               const std::string& categorie, const std::string& unitate);
    ~Consumabil() override = default;

    std::string getCategorie() const { return categorie; }
    std::string getUnitate()   const { return unitate; }

    void afisare() const override;
    std::string getTip() const override { return "CONSUMABIL"; }
    std::string serializa() const override;
    void deserializa(const std::string& linie) override;
};

// ============================================================
// Echipament : Produs
// ============================================================
class Echipament : public Produs {
private:
    std::string tip;
    std::string stare;    // "nou", "bun", "uzat"

public:
    Echipament();
    Echipament(const std::string& id, const std::string& denumire,
               int cantitate, double pret, const std::string& termenExpirare,
               const std::string& tip, const std::string& stare);
    ~Echipament() override = default;

    std::string getTipEch() const { return tip; }
    std::string getStare()  const { return stare; }
    void setStare(const std::string& s) { stare = s; }

    void afisare() const override;
    std::string getTip() const override { return "ECHIPAMENT"; }
    std::string serializa() const override;
    void deserializa(const std::string& linie) override;
};
