#pragma once
#include <string>
#include <vector>
#include <iostream>

// ============================================================
// AnalizaLaborator
// ============================================================
struct ValoareReferinta {
    std::string parametru;
    std::string valoareNormala;
    std::string unitateMasura;
};

class AnalizaLaborator {
private:
    std::string idAnaliza;
    std::string idPacient;
    std::string idMedicResponsabil;
    std::string tipAnaliza;         // "hemoleucograma", "biochimie", etc.
    std::string dataRecoltare;
    std::string dataRezultat;
    std::vector<std::pair<std::string, std::string>> rezultate;  // parametru -> valoare
    std::vector<ValoareReferinta> valoriReferinta;
    std::string observatii;
    bool finalizata;

    static int contorAnalize;

public:
    AnalizaLaborator();
    AnalizaLaborator(const std::string& id, const std::string& idPacient,
                     const std::string& idMedic, const std::string& tip,
                     const std::string& dataRecoltare);
    ~AnalizaLaborator() = default;

    // Getters
    std::string getIdAnaliza()           const { return idAnaliza; }
    std::string getIdPacient()           const { return idPacient; }
    std::string getIdMedic()             const { return idMedicResponsabil; }
    std::string getTipAnaliza()          const { return tipAnaliza; }
    std::string getDataRecoltare()       const { return dataRecoltare; }
    std::string getDataRezultat()        const { return dataRezultat; }
    bool        isFinalizata()           const { return finalizata; }
    std::string getObservatii()          const { return observatii; }

    // Operatii
    void adaugaRezultat(const std::string& parametru, const std::string& valoare);
    void adaugaValoareReferinta(const ValoareReferinta& vr);
    void finalizeaza(const std::string& dataRezultat, const std::string& obs);
    void setObservatii(const std::string& o) { observatii = o; }

    void afisare() const;
    void afisareCompleta() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
