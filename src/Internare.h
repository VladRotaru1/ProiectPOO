#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

// ============================================================
// Internare - eveniment separat (nu mosteneste din Pacient)
// ============================================================
enum class StareInternare {
    ACTIVA,
    EXTERNATA,
    TRANSFERATA,
    DECEDAT
};

std::string stareInternToString(StareInternare s);
StareInternare stringToStareIntern(const std::string& s);

class Internare {
private:
    std::string idInternare;
    std::string idPacient;
    std::string idMedicResponsabil;
    std::string idSectie;
    std::string numarSalon;
    std::string codPat;
    std::string dataInternarii;
    std::string dataExternarii;
    std::string tratamentActiv;
    StareInternare stare;
    std::string observatii;
    std::string diagnosticInternare;
    std::string diagnosticExternare;
    double costTotal;

    // Parametri vitali monitorizati (timestamp -> valori)
    std::vector<std::pair<std::string, std::string>> parametriVitali;

    static int contorInternari;

public:
    Internare();
    Internare(const std::string& id, const std::string& idPacient,
              const std::string& idMedic, const std::string& idSectie,
              const std::string& numarSalon, const std::string& codPat,
              const std::string& dataInternarii, const std::string& diagnosticInternare);
    ~Internare() = default;

    // Getters
    std::string    getIdInternare()          const { return idInternare; }
    std::string    getIdPacient()            const { return idPacient; }
    std::string    getIdMedic()              const { return idMedicResponsabil; }
    std::string    getIdSectie()             const { return idSectie; }
    std::string    getNumarSalon()           const { return numarSalon; }
    std::string    getCodPat()               const { return codPat; }
    std::string    getDataInternarii()       const { return dataInternarii; }
    std::string    getDataExternarii()       const { return dataExternarii; }
    std::string    getTratamentActiv()       const { return tratamentActiv; }
    StareInternare getStare()                const { return stare; }
    std::string    getObservatii()           const { return observatii; }
    std::string    getDiagnosticInternare()  const { return diagnosticInternare; }
    std::string    getDiagnosticExternare()  const { return diagnosticExternare; }
    double         getCostTotal()            const { return costTotal; }
    bool           isActiva()               const { return stare == StareInternare::ACTIVA; }

    // Setters
    void setTratamentActiv(const std::string& t)  { tratamentActiv = t; }
    void setObservatii(const std::string& o)      { observatii = o; }
    void setIdInternare(const std::string& id)    { idInternare = id; }
    void setCostTotal(double c)                    { costTotal = c; }

    // Operatii
    void externeaza(const std::string& data, const std::string& diagnosticExt);
    void transfera(const std::string& nouaSectie, const std::string& noulSalon,
                   const std::string& noulPat);
    void marcheazaDecedat();
    void adaugaParametriVitali(const std::string& timestamp, const std::string& valori);

    void afisare() const;
    void afisareCompleta() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
