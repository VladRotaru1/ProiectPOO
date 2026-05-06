#pragma once
#include "Angajat.h"
#include <vector>
#include <string>

// ============================================================
// Grad profesional medic
// ============================================================
enum class GradProfesional {
    REZIDENT,
    SPECIALIST,
    PRIMAR,
    PROFESOR_DOCTOR
};

std::string gradToString(GradProfesional g);
GradProfesional stringToGrad(const std::string& s);

// ============================================================
// Medic : Angajat
// ============================================================
class Medic : public Angajat {
private:
    std::string specializare;
    GradProfesional grad;
    std::string codParafa;
    std::string program;         // ex: "Luni-Vineri 08:00-16:00"
    double rating;               // 1.0 - 5.0
    std::vector<std::string> idPacientiAsignati;
    int numarConsultatii;
    int numarInterventii;

    static int contorMedici;

public:
    Medic();
    Medic(const std::string& id, const std::string& nume, const std::string& prenume,
          const std::string& cnp, const std::string& sex, const std::string& dataNasterii,
          const std::string& telefon, const std::string& email, const std::string& adresa,
          const std::string& dataAngajarii, double salariuBrut,
          const std::string& specializare, GradProfesional grad, const std::string& codParafa);

    ~Medic() override = default;

    // Getters
    std::string     getSpecializare()      const { return specializare; }
    GradProfesional getGrad()              const { return grad; }
    std::string     getCodParafa()         const { return codParafa; }
    std::string     getProgram()           const { return program; }
    double          getRating()            const { return rating; }
    int             getNumarConsultatii()  const { return numarConsultatii; }
    int             getNumarInterventii()  const { return numarInterventii; }
    const std::vector<std::string>& getPacientiAsignati() const { return idPacientiAsignati; }

    // Setters
    void setSpecializare(const std::string& s) { specializare = s; }
    void setGrad(GradProfesional g)            { grad = g; }
    void setCodParafa(const std::string& c)    { codParafa = c; }
    void setProgram(const std::string& p)      { program = p; }
    void setRating(double r)                   { if (r >= 1.0 && r <= 5.0) rating = r; }

    // Functionalitati
    void asigneazaPacient(const std::string& idPacient);
    void dezasigneazaPacient(const std::string& idPacient);
    bool arePacientAsignat(const std::string& idPacient) const;
    void incrementeazaConsultatii() { numarConsultatii++; }
    void incrementeazaInterventii() { numarInterventii++; }

    // Calcul salariu (medici au spor de grad)
    double calculeazaSalariuNet() const override;

    void afisare() const override;
    std::string serializa() const override;
    void deserializa(const std::string& linie) override;

    static Medic citesteDeLaConsola();
};
