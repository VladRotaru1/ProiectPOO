#pragma once
#include "Programare.h"
#include "Reteta.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

// ============================================================
// Consultatie
// ============================================================
class Consultatie {
private:
    std::string idConsultatie;
    std::string idProgramare;
    std::string idPacient;
    std::string idMedic;
    std::string dataConsultatiei;
    std::string simptome;
    std::string diagnostic;
    std::string tratament;
    std::string recomandari;
    int zileConcediu;          // 0 daca nu este cazul
    std::shared_ptr<Reteta> reteta;

    static int contorConsultatii;

public:
    Consultatie();
    Consultatie(const std::string& id, const std::string& idProgramare,
                const std::string& idPacient, const std::string& idMedic,
                const std::string& data);
    ~Consultatie() = default;

    // Getters
    std::string getIdConsultatie()   const { return idConsultatie; }
    std::string getIdProgramare()    const { return idProgramare; }
    std::string getIdPacient()       const { return idPacient; }
    std::string getIdMedic()         const { return idMedic; }
    std::string getDataConsultatiei() const { return dataConsultatiei; }
    std::string getSimptome()        const { return simptome; }
    std::string getDiagnostic()      const { return diagnostic; }
    std::string getTratament()       const { return tratament; }
    std::string getRecomandari()     const { return recomandari; }
    int         getZileConcediu()    const { return zileConcediu; }
    std::shared_ptr<Reteta> getReteta() const { return reteta; }
    bool areReteta() const { return reteta != nullptr; }

    // Setters
    void setSimptome(const std::string& s)    { simptome = s; }
    void setDiagnostic(const std::string& d)  { diagnostic = d; }
    void setTratament(const std::string& t)   { tratament = t; }
    void setRecomandari(const std::string& r) { recomandari = r; }
    void setZileConcediu(int z)               { zileConcediu = z; }
    void setReteta(std::shared_ptr<Reteta> r) { reteta = r; }
    void setIdConsultatie(const std::string& id) { idConsultatie = id; }

    void afisare() const;
    void afisareCompleta() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
