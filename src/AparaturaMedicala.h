#pragma once
#include <string>
#include <vector>
#include <iostream>

// ============================================================
// AparaturaMedicala
// ============================================================
enum class StareAparatura {
    FUNCTIONALA,
    DEFECTA,
    IN_MENTENANTA,
    REZERVATA
};

std::string stareToString(StareAparatura s);
StareAparatura stringToStare(const std::string& s);

struct InregistrareMentenanta {
    std::string data;
    std::string descriere;
    std::string tehnician;
    double cost;

    std::string serializa() const;
    static InregistrareMentenanta deserializa(const std::string& s);
};

class AparaturaMedicala {
private:
    std::string idAparatura;
    std::string denumire;
    std::string serie;
    std::string producator;
    StareAparatura stare;
    std::string idSectie;
    std::string ultimaMentenanta;
    bool disponibila;
    std::vector<InregistrareMentenanta> jurnalMentenanta;

    static int contorAparatura;

public:
    AparaturaMedicala();
    AparaturaMedicala(const std::string& id, const std::string& denumire,
                      const std::string& serie, const std::string& producator,
                      const std::string& idSectie);
    ~AparaturaMedicala() = default;

    // Getters
    std::string    getIdAparatura()       const { return idAparatura; }
    std::string    getDenumire()          const { return denumire; }
    std::string    getSerie()             const { return serie; }
    std::string    getProducator()        const { return producator; }
    StareAparatura getStare()             const { return stare; }
    std::string    getIdSectie()          const { return idSectie; }
    std::string    getUltimaMentenanta()  const { return ultimaMentenanta; }
    bool           isDisponibila()        const { return disponibila && stare == StareAparatura::FUNCTIONALA; }
    const std::vector<InregistrareMentenanta>& getJurnalMentenanta() const { return jurnalMentenanta; }

    // Setters
    void setIdSectie(const std::string& s) { idSectie = s; }
    void setDenumire(const std::string& d) { denumire = d; }

    // Operatii
    void marcheazaDefecta(const std::string& motivul);
    void trimiteLaMentenanta(const std::string& data, const std::string& descriere,
                              const std::string& tehnician, double cost);
    void marcheazaFunctionala();
    void blocheazaFolosire() { disponibila = false; }
    void deblocheazaFolosire();

    void afisare() const;
    void afisareJurnal() const;
    std::string serializa() const;
    void deserializa(const std::string& linie);
};
