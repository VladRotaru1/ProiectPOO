#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>

// ============================================================
// Reteta - reteta electronica
// ============================================================
struct MedicamentReteta {
    std::string denumireMedicament;
    std::string dozaj;          // ex: "1 comprimat x 3/zi"
    int numarZile;
    std::string observatii;
};

class Reteta {
private:
    std::string idReteta;
    std::string idConsultatie;
    std::string idPacient;
    std::string codParafaMedic;
    std::string dataPrescrierii;
    std::vector<MedicamentReteta> medicamente;
    bool eliberata;

    static int contorRetete;

public:
    Reteta();
    Reteta(const std::string& id, const std::string& idConsultatie,
           const std::string& idPacient, const std::string& codParafa,
           const std::string& data);
    ~Reteta() = default;

    // Getters
    std::string getIdReteta()        const { return idReteta; }
    std::string getIdConsultatie()   const { return idConsultatie; }
    std::string getIdPacient()       const { return idPacient; }
    std::string getCodParafa()       const { return codParafaMedic; }
    std::string getDataPrescrierii() const { return dataPrescrierii; }
    bool        isEliberata()        const { return eliberata; }
    const std::vector<MedicamentReteta>& getMedicamente() const { return medicamente; }

    // Operatii
    void adaugaMedicament(const MedicamentReteta& med);
    void marcheazaEliberata() { eliberata = true; }

    // Generare fisier text
    void genereazaFisier(const std::string& numePacient, const std::string& numeMedic) const;

    void afisare() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
