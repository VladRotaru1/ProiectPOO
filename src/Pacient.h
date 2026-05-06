#pragma once
#include "Persoana.h"
#include <vector>
#include <string>

// ============================================================
// Prioritate triage
// ============================================================
enum class PrioritateTriage {
    ROSU = 1,    // Critic - imediat
    PORTOCALIU,  // Urgent
    GALBEN,      // Semi-urgent
    VERDE        // Non-urgent
};

std::string prioritateToString(PrioritateTriage p);
PrioritateTriage stringToPrioritate(const std::string& s);

// ============================================================
// IntrareIstoricMedical - inregistrare in istoricul pacientului
// ============================================================
struct IntrareIstoricMedical {
    std::string data;
    std::string diagnostic;
    std::string tratament;
    std::string medicResponsabil;
    std::string observatii;

    std::string serializa() const;
    static IntrareIstoricMedical deserializa(const std::string& linie);
};

// ============================================================
// Pacient : Persoana
// ============================================================
class Pacient : public Persoana {
private:
    std::string codPacient;
    bool asiguratCAS;
    std::string grupaSanguina;
    std::vector<std::string> alergii;
    std::vector<std::string> boliCronice;
    std::string contactUrgenta;
    PrioritateTriage prioritateTriage;
    std::vector<IntrareIstoricMedical> istoricMedical;
    int numarVizite;

    static int contorPacienti;

public:
    Pacient();
    Pacient(const std::string& id, const std::string& nume, const std::string& prenume,
            const std::string& cnp, const std::string& sex, const std::string& dataNasterii,
            const std::string& telefon, const std::string& email, const std::string& adresa,
            bool asiguratCAS, const std::string& grupaSanguina,
            const std::string& contactUrgenta);

    ~Pacient() override = default;

    // Getters
    std::string getCodPacient()       const { return codPacient; }
    bool        isAsiguratCAS()       const { return asiguratCAS; }
    std::string getGrupaSanguina()    const { return grupaSanguina; }
    const std::vector<std::string>& getAllergii()    const { return alergii; }
    const std::vector<std::string>& getBoliCronice() const { return boliCronice; }
    std::string getContactUrgenta()   const { return contactUrgenta; }
    PrioritateTriage getPrioritate()  const { return prioritateTriage; }
    int getNumarVizite()              const { return numarVizite; }
    const std::vector<IntrareIstoricMedical>& getIstoricMedical() const { return istoricMedical; }

    // Setters
    void setAsiguratCAS(bool val)                  { asiguratCAS = val; }
    void setGrupaSanguina(const std::string& g)    { grupaSanguina = g; }
    void setContactUrgenta(const std::string& c)   { contactUrgenta = c; }
    void setPrioritate(PrioritateTriage p)          { prioritateTriage = p; }
    void setCodPacient(const std::string& cod)      { codPacient = cod; }

    // Functionalitati
    void adaugaAlergie(const std::string& alergie);
    void adaugaBoalaCronica(const std::string& boala);
    void adaugaIntrareIstoricMedical(const IntrareIstoricMedical& intrare);
    bool areAlergieLa(const std::string& substanta) const;
    void incrementeazaVizite() { numarVizite++; }

    void afisareFisaMedicala() const;
    void afisare() const override;

    std::string serializa() const override;
    void deserializa(const std::string& linie) override;

    // Citire date de la consola
    static Pacient citesteDeLaConsola();
};
