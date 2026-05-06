#pragma once
#include <string>
#include <iostream>

// ============================================================
// Pat - unitatea de baza a unui salon
// ============================================================
class Pat {
private:
    std::string codPat;
    bool ocupat;
    std::string idPacientCurent;  // "" daca liber
    bool igienizat;

public:
    Pat();
    Pat(const std::string& cod);
    ~Pat() = default;

    // Getters
    std::string getCodPat()          const { return codPat; }
    bool        isOcupat()           const { return ocupat; }
    bool        isIgienizat()        const { return igienizat; }
    std::string getIdPacientCurent() const { return idPacientCurent; }
    bool        isDisponibil()       const { return !ocupat && igienizat; }

    // Setters
    void setCodPat(const std::string& c) { codPat = c; }

    // Operatii
    void ocupa(const std::string& idPacient);
    void elibereaza();
    void marcheazaIgienizat();
    void marcheazaNecesitaIgienizare();

    void afisare() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
