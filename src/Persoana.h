#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "Exceptii.h"

// ============================================================
// Clasa abstracta de baza: Persoana
// ============================================================

class Persoana {
protected:
    std::string id;
    std::string nume;
    std::string prenume;
    std::string cnp;
    std::string sex;         // "M" sau "F"
    std::string dataNasterii;
    int varsta;
    std::string telefon;
    std::string email;
    std::string adresa;

    // Genereaza un ID unic bazat pe contor static
    static int contor;
    static std::string genereazaId(const std::string& prefix);

public:
    Persoana();
    Persoana(const std::string& id, const std::string& nume, const std::string& prenume,
             const std::string& cnp, const std::string& sex, const std::string& dataNasterii,
             const std::string& telefon, const std::string& email, const std::string& adresa);
    virtual ~Persoana() = default;

    // Getters
    std::string getId()           const { return id; }
    std::string getNume()         const { return nume; }
    std::string getPrenume()      const { return prenume; }
    std::string getCnp()          const { return cnp; }
    std::string getSex()          const { return sex; }
    std::string getDataNasterii() const { return dataNasterii; }
    int         getVarsta()       const { return varsta; }
    std::string getTelefon()      const { return telefon; }
    std::string getEmail()        const { return email; }
    std::string getAdresa()       const { return adresa; }
    std::string getNumeComplet()  const { return prenume + " " + nume; }

    // Setters
    void setNume(const std::string& n)          { nume = n; }
    void setPrenume(const std::string& p)       { prenume = p; }
    void setTelefon(const std::string& t)       { telefon = t; }
    void setEmail(const std::string& e)         { email = e; }
    void setAdresa(const std::string& a)        { adresa = a; }
    void setCnp(const std::string& c);          // valideaza CNP

    // Validare CNP romanesc (13 cifre, prima cifra 1-9)
    static bool validareCNP(const std::string& cnp);

    // Calcul varsta din CNP
    static int calculeazaVarsta(const std::string& cnp);

    // Criptare simpla CNP (XOR cu cheie)
    static std::string cripteazaCNP(const std::string& cnp);
    static std::string decripteazaCNP(const std::string& cnpCriptat);

    // Afisare virtuala (polimorfism)
    virtual void afisare() const = 0;

    // Serializare / Deserializare
    virtual std::string serializa() const;
    virtual void deserializa(const std::string& linie);

    // Operator de afisare
    friend std::ostream& operator<<(std::ostream& os, const Persoana& p);
};
