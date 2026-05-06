#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

// ============================================================
// Status programare
// ============================================================
enum class StatusProgramare {
    ACTIVA,
    ANULATA,
    EFECTUATA,
    NEPREZENTAT
};

std::string statusProgToString(StatusProgramare s);
StatusProgramare stringToStatusProg(const std::string& s);

// ============================================================
// Programare
// ============================================================
class Programare {
private:
    std::string idProgramare;
    std::string idPacient;
    std::string idMedic;
    std::string data;        // "AAAA-LL-ZZ"
    std::string ora;         // "HH:MM"
    std::string cabinet;
    StatusProgramare status;
    std::string descriere;

    static int contorProgramari;

public:
    Programare();
    Programare(const std::string& id, const std::string& idPacient,
               const std::string& idMedic, const std::string& data,
               const std::string& ora, const std::string& cabinet,
               const std::string& descriere = "");
    ~Programare() = default;

    // Getters
    std::string      getIdProgramare() const { return idProgramare; }
    std::string      getIdPacient()    const { return idPacient; }
    std::string      getIdMedic()      const { return idMedic; }
    std::string      getData()         const { return data; }
    std::string      getOra()          const { return ora; }
    std::string      getCabinet()      const { return cabinet; }
    StatusProgramare getStatus()       const { return status; }
    std::string      getDescriere()    const { return descriere; }

    // Setters
    void setStatus(StatusProgramare s)       { status = s; }
    void setCabinet(const std::string& c)    { cabinet = c; }
    void setDescriere(const std::string& d)  { descriere = d; }
    void setIdProgramare(const std::string& id) { idProgramare = id; }

    // Operatii
    bool suprapuneCu(const Programare& alta) const;
    int estimareTimpAsteptare(const std::vector<std::shared_ptr<Programare>>& listaProgramari,
                               const std::string& idMedicCautat) const;

    void anuleaza() { status = StatusProgramare::ANULATA; }
    void marcheazaEfectuata() { status = StatusProgramare::EFECTUATA; }
    void marcheazaNeprezentat() { status = StatusProgramare::NEPREZENTAT; }

    void afisare() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
