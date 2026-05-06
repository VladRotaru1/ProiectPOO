#pragma once
#include "Salon.h"
#include "AparaturaMedicala.h"
#include <vector>
#include <string>
#include <memory>

// Forward declarations
class Medic;
class Asistent;
class AparaturaMedicala;

// ============================================================
// Sectie
// ============================================================
class Sectie {
private:
    std::string idSectie;
    std::string numeSectie;
    int etaj;
    std::vector<std::string> idMedici;
    std::vector<std::string> idAsistenti;
    std::vector<Salon> saloane;
    std::vector<std::string> idAparatura;

    static int contorSectii;

public:
    Sectie();
    Sectie(const std::string& id, const std::string& nume, int etaj);
    ~Sectie() = default;

    // Getters
    std::string getIdSectie()    const { return idSectie; }
    std::string getNumeSectie()  const { return numeSectie; }
    int         getEtaj()        const { return etaj; }
    const std::vector<std::string>& getIdMedici()    const { return idMedici; }
    const std::vector<std::string>& getIdAsistenti() const { return idAsistenti; }
    const std::vector<Salon>&       getSaloane()     const { return saloane; }
    std::vector<Salon>&             getSaloaneRef()        { return saloane; }
    const std::vector<std::string>& getIdAparatura() const { return idAparatura; }

    // Setters
    void setIdSectie(const std::string& id)   { idSectie = id; }
    void setNumeSectie(const std::string& n)  { numeSectie = n; }
    void setEtaj(int e)                       { etaj = e; }

    // Gestionare personal
    void adaugaMedic(const std::string& idMedic);
    void eliminaMedic(const std::string& idMedic);
    void adaugaAsistent(const std::string& idAsistent);
    void eliminaAsistent(const std::string& idAsistent);

    // Gestionare saloane
    void adaugaSalon(const Salon& salon);
    Salon* getSalon(const std::string& numarSalon);
    int getPaturiLibereTotal() const;
    int getPaturiOcupateTotal() const;

    // Gasire pat liber in sectie
    // returneaza true si completeaza numarSalon + codPat
    bool gastestePatLiber(std::string& numarSalon, std::string& codPat);
    bool ocupa(const std::string& idPacient, std::string& numarSalon, std::string& codPat);
    bool elibereazaPat(const std::string& codPat);

    // Aparatura
    void adaugaAparatura(const std::string& idApar);

    void afisare() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
