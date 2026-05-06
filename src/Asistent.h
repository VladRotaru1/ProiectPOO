#pragma once
#include "Angajat.h"
#include <vector>
#include <string>

// ============================================================
// Asistent : Angajat
// ============================================================
class Asistent : public Angajat {
private:
    std::string sectie;
    std::string tipTura;   // "zi" / "noapte" / "rotatie"
    std::vector<std::string> calificari;
    std::vector<std::string> idPacientiAsistati;

    static int contorAsistenti;

public:
    Asistent();
    Asistent(const std::string& id, const std::string& nume, const std::string& prenume,
             const std::string& cnp, const std::string& sex, const std::string& dataNasterii,
             const std::string& telefon, const std::string& email, const std::string& adresa,
             const std::string& dataAngajarii, double salariuBrut,
             const std::string& sectie, const std::string& tipTura);
    ~Asistent() override = default;

    // Getters
    std::string getSectie()  const { return sectie; }
    std::string getTipTura() const { return tipTura; }
    const std::vector<std::string>& getCalificari()        const { return calificari; }
    const std::vector<std::string>& getPacientiAsistati()  const { return idPacientiAsistati; }

    // Setters
    void setSectie(const std::string& s)  { sectie = s; }
    void setTipTura(const std::string& t) { tipTura = t; }

    void adaugaCalificare(const std::string& c);
    void asigneazaPacient(const std::string& idPacient);
    void elibereazaPacient(const std::string& idPacient);

    void afisare() const override;
    std::string serializa() const override;
    void deserializa(const std::string& linie) override;

    static Asistent citesteDeLaConsola();
};
