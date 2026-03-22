#include <iostream>
#include <string>
#include <vector>

class Angajat {
protected:
    std::string nume;
    double salariu;

public:
    Angajat(std::string n, double s) : nume(n), salariu(s) {}
    
    Angajat(const Angajat& altul) : nume(altul.nume), salariu(altul.salariu) {}
    
    ~Angajat() {}
};

class Medic : public Angajat {
private:
    std::string specializare;
    double tarifConsultatie;

public:
    Medic(std::string n, double s, std::string spec, double tarif) 
        : Angajat(n, s), specializare(spec), tarifConsultatie(tarif) {}

    Medic(const Medic& altul) 
        : Angajat(altul), specializare(altul.specializare), tarifConsultatie(altul.tarifConsultatie) {}

    ~Medic() {}
};

class Asistent : public Angajat {
private:
    std::string sectie;

public:
    Asistent(std::string n, double s, std::string sec) 
        : Angajat(n, s), sectie(sec) {}

    Asistent(const Asistent& altul) 
        : Angajat(altul), sectie(altul.sectie) {}

    ~Asistent() {}
};

class Pacient {
private:
    std::string nume;
    std::string diagnostic;

public:
    Pacient(std::string n, std::string d) : nume(n), diagnostic(d) {}

    Pacient(const Pacient& altul) : nume(altul.nume), diagnostic(altul.diagnostic) {}

    ~Pacient() {}
};

class Programare {
private:
    Pacient* pacient;
    Medic* medic;
    std::string data;

public:
    Programare(Pacient* p, Medic* m, std::string d) : pacient(p), medic(m), data(d) {}

    Programare(const Programare& altul) 
        : pacient(altul.pacient), medic(altul.medic), data(altul.data) {}

    ~Programare() {}
};

class Factura {
private:
    static int contor;
    int id;
    std::string numeClient;
    double suma;

public:
    Factura(std::string nume, double s) : numeClient(nume), suma(s) {
        id = ++contor;
    }

    Factura(const Factura& altul) : id(altul.id), numeClient(altul.numeClient), suma(altul.suma) {}

    ~Factura() {}
};

int Factura::contor = 0;

int main() {
    return 0;
}