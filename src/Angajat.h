#pragma once
#include "Persoana.h"
#include <vector>
#include <string>

// ============================================================
// Angajat : Persoana - clasa de baza pentru toti angajatii
// ============================================================

struct Tura {
    std::string data;       // "AAAA-LL-ZZ"
    std::string tipTura;    // "zi" / "noapte"
    int oreDurata;

    std::string serializa() const;
    static Tura deserializa(const std::string& s);
};

class Angajat : public Persoana {
protected:
    std::string codAngajat;
    std::string dataAngajarii;   // "AAAA-LL-ZZ"
    double salariuBrut;
    double salariuNet;
    double sporuri;
    bool   activ;
    std::vector<Tura> ture;
    std::vector<std::string> certificari;

    static int contorAngajati;

public:
    Angajat();
    Angajat(const std::string& id, const std::string& nume, const std::string& prenume,
            const std::string& cnp, const std::string& sex, const std::string& dataNasterii,
            const std::string& telefon, const std::string& email, const std::string& adresa,
            const std::string& dataAngajarii, double salariuBrut);
    virtual ~Angajat() = default;

    // Getters
    std::string getCodAngajat()    const { return codAngajat; }
    std::string getDataAngajarii() const { return dataAngajarii; }
    double getSalariuBrut()        const { return salariuBrut; }
    double getSalariuNet()         const { return salariuNet; }
    double getSporuri()            const { return sporuri; }
    bool   isActiv()               const { return activ; }
    int    getAniExperienta()      const;
    const std::vector<Tura>& getTure() const { return ture; }
    const std::vector<std::string>& getCertificari() const { return certificari; }

    // Setters
    void setSalariuBrut(double s);
    void setActiv(bool a) { activ = a; }
    void setCodAngajat(const std::string& cod) { codAngajat = cod; }
    void setDataAngajarii(const std::string& d) { dataAngajarii = d; }

    // Calcule
    virtual double calculeazaSalariuNet() const;
    double calculeazaSporNoapte() const;
    void recalculeazaSalariu();

    // Ture & Certificari
    void adaugaTura(const Tura& t);
    void adaugaCertificare(const std::string& cert);

    virtual void afisare() const override;

    virtual std::string serializa() const override;
    virtual void deserializa(const std::string& linie) override;

    static Angajat citesteDeLaConsola();
};
