#pragma once
#include "Pat.h"
#include <vector>
#include <string>

// ============================================================
// Tipuri salon
// ============================================================
enum class TipSalon {
    STANDARD,
    ATI,
    IZOLARE
};

std::string tipSalonToString(TipSalon t);
TipSalon stringToTipSalon(const std::string& s);

// ============================================================
// Salon
// ============================================================
class Salon {
private:
    std::string numarSalon;
    TipSalon    tip;
    std::vector<Pat> paturi;
    int capacitate;

public:
    Salon();
    Salon(const std::string& numar, TipSalon tip, int capacitate);
    ~Salon() = default;

    // Getters
    std::string getNumarSalon()  const { return numarSalon; }
    TipSalon    getTip()         const { return tip; }
    int         getCapacitate()  const { return capacitate; }
    int         getPaturiLibere() const;
    int         getPaturiOcupate() const;
    const std::vector<Pat>& getPaturi() const { return paturi; }
    std::vector<Pat>&       getPaturiRef()    { return paturi; }

    // Setters
    void setNumarSalon(const std::string& n) { numarSalon = n; }
    void setTip(TipSalon t)                  { tip = t; }

    // Operatii
    Pat* primulPatLiber();                              // nullptr daca nu exista
    bool ocupa(const std::string& idPacient, std::string& codPatOcupat);
    bool elibereazaPat(const std::string& codPat);
    bool igienizeazaPat(const std::string& codPat);
    Pat* getPat(const std::string& codPat);

    void afisare() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
