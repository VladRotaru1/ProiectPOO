#pragma once
#include "Angajat.h"
#include <string>

// ============================================================
// Tipuri de personal auxiliar
// ============================================================
enum class TipPersonalAuxiliar {
    INFIRMIER,
    BRANCARDIER,
    RECEPTIE,
    ADMINISTRATIV
};

std::string tipAuxToString(TipPersonalAuxiliar t);
TipPersonalAuxiliar stringToTipAux(const std::string& s);

// ============================================================
// PersonalAuxiliar : Angajat
// ============================================================
class PersonalAuxiliar : public Angajat {
private:
    TipPersonalAuxiliar tip;
    std::string departament;

public:
    PersonalAuxiliar();
    PersonalAuxiliar(const std::string& id, const std::string& nume, const std::string& prenume,
                     const std::string& cnp, const std::string& sex, const std::string& dataNasterii,
                     const std::string& telefon, const std::string& email, const std::string& adresa,
                     const std::string& dataAngajarii, double salariuBrut,
                     TipPersonalAuxiliar tip, const std::string& departament);
    ~PersonalAuxiliar() override = default;

    TipPersonalAuxiliar getTip()       const { return tip; }
    std::string         getDepartament() const { return departament; }
    void setTip(TipPersonalAuxiliar t)       { tip = t; }
    void setDepartament(const std::string& d){ departament = d; }

    void afisare() const override;
    std::string serializa() const override;
    void deserializa(const std::string& linie) override;

    static PersonalAuxiliar citesteDeLaConsola();
};
