#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Angajat {
protected:
    string nume;
    double salariu;

public:
    Angajat(string n="---", double s=0) : nume(n), salariu(s) {}
    
    Angajat(const Angajat& altul) : nume(altul.nume), salariu(altul.salariu) {}
    
    ~Angajat() {}
    string getNume() const { return nume; }
    double getSalariu() const { return salariu; }
    void setNume(string n) { 
        nume = n; 
    }
    void setSalariu(double s) { 
        salariu = s; 
    }
    friend istream& operator>>(istream& in, Angajat& a) {
        cout << "Nume: "; 
        in >> ws;
        getline(in, a.nume);
        cout << "Salariu: "; 
        in >> a.salariu;
        return in;
    }
    friend ostream& operator<<(ostream& out, Angajat& a){
        out<<"Nume: "<<a.nume<<" | Salariu: "<<a.salariu;
        return out;
    }
};

class Medic : public Angajat {
private:
    string specializare;
    double tarifConsultatie;

public:
    Medic(string n = "---", double s=0, string spec = "Nespecificat", double tarif=0) : Angajat(n, s), specializare(spec), tarifConsultatie(tarif) {}

    Medic(const Medic& altul) : Angajat(altul), specializare(altul.specializare), tarifConsultatie(altul.tarifConsultatie) {}

    ~Medic() {}
    string getSpecializare() const { return specializare; }
    double getTarif() const { return tarifConsultatie; }
    void setTarif(double t) { 
        tarifConsultatie = t; 
    }
    void setSpecializare(string s) {
        specializare = s; 
    }
    friend istream& operator>>(istream& in, Medic& m) {
        in >> (Angajat&)m;
        cout << "Specializare: "; 
        in >> ws;
        getline(in, m.specializare);
        cout << "Tarif consultatie: "; 
        in >> m.tarifConsultatie;
        return in;
    }
    friend ostream& operator<<(ostream& out, Medic& m) {
        out << "[Medic] " << (Angajat&)m << " | Spec: " << m.specializare << " | Tarif: " << m.tarifConsultatie;
        return out;
    }
};

class Asistent : public Angajat {
private:
    string sectie;

public:
    Asistent(string n, double s, string sec) : Angajat(n, s), sectie(sec) {}

    Asistent(const Asistent& altul) : Angajat(altul), sectie(altul.sectie) {}

    ~Asistent() {}
    string getSectie() const { return sectie; }
    void setSectie(string s) { 
        sectie = s; 
    }
    friend istream& operator>>(istream& in, Asistent& a) {
        in >> (Angajat&)a;
        cout << "Sectie: "; 
        in >> ws;
        getline(in, a.sectie);
        return in;
    }
    friend ostream& operator<<(ostream& out, Asistent& a) {
        out << "[Asistent] " << (Angajat&)a << " | Sectie: " << a.sectie;
        return out;
    }
};

class Pacient {
private:
    string nume;
    string diagnostic;
    int varsta;

public:
    Pacient(string n = "---", string d = "Nespecificat", int v=0) : nume(n), diagnostic(d), varsta(v) {}

    Pacient(const Pacient& altul) : nume(altul.nume), diagnostic(altul.diagnostic), varsta(altul.varsta) {}

    friend istream& operator>>(istream& in, Pacient& p) {
        cout << "Nume Pacient: "; 
        in >> ws;
        getline(in, p.nume);
        cout << "Diagnostic: "; 
        getline(in, p.diagnostic);
        cout << "Varsta: "; 
        in >> p.varsta;
        return in;
    }
    friend ostream& operator<<(ostream& out, Pacient& p) {
        out << "[Pacient] " << p.nume << " | Diag: " << p.diagnostic << " | " << p.varsta << " ani";
        return out;
    }
    ~Pacient() {}
    string getNume() { return nume; }
    string getDiagnostic() { return diagnostic; }
    int getVarsta() { return varsta; }
    void setNume(string numeNou) {
        this->nume = numeNou; 
    }
    void setDiagnostic(string diagNou) {
        diagnostic = diagNou;
    }
    void setVarsta(int varstaNoua) {
        this->varsta = varstaNoua;
    }
};

class Programare {
private:
    Pacient* pacient;
    Medic* medic;
    string data;

public:
    Programare(Pacient* p=nullptr, Medic* m=nullptr, string d="XX.XX.XXXX") : pacient(p), medic(m), data(d) {}

    Programare(const Programare& altul) : pacient(altul.pacient), medic(altul.medic), data(altul.data) {}

    ~Programare() {}

    string getData() const { return data; }
    void setData(string d) {
        data = d; 
    }
    friend istream& operator>>(istream& in, Programare& pr) {
        cout << "Introduceti data programarii (ZZ.LL.AAAA): ";
        in >> ws;
        getline(in, pr.data);
        cout << "--- Date Pacient ---" << endl;
        in >> *pr.pacient;
        cout << "--- Date Medic ---" << endl;
        in >> *pr.medic;

        return in;
    }
    friend ostream& operator<<(ostream& out, Programare& pr) {
        out << "========= PROGRAMARE =========" << endl;
        out << "Data: " << pr.data << endl;
        out << "Pacient: " << *pr.pacient << endl; 
        out << "Medic: " << *pr.medic << endl;
        out << "==============================";
        return out;
    }
};

class Factura {
private:
    static int contor;
    int id;
    string numeClient;
    double suma;

public:
    Factura(string nume="---", double s=0) : numeClient(nume), suma(s) {
        id = ++contor;
    }

    Factura(const Factura& altul) : id(altul.id), numeClient(altul.numeClient), suma(altul.suma) {}

    ~Factura() {}

    int getId() const { return id; }
    string getNumeClient() const { return numeClient; }
    double getSuma() const { return suma; }

    void setNumeClient(string nume) { 
        if (!nume.empty()) 
            numeClient = nume; 
    }
    void setSuma(double s) { 
        if (s >= 0) 
            suma = s; 
    }

    friend istream& operator>>(istream& in, Factura& f) {
        cout << "Nume Client Factura: ";
        in >> ws; 
        getline(in, f.numeClient);
        cout << "Suma totala: "; 
        in >> f.suma;
        return in;
    }
    friend ostream& operator<<(ostream& out, Factura& f) {
        out << "Factura #" << f.id << " | Client: " << f.numeClient << " | Total: " << f.suma << " RON";
        return out;
    }
};

int Factura::contor = 0;

int main() {
    cout << "=== TESTARE Medic + Asistent ===" << endl;
    
    Medic m1("Popescu Ion", 7500, "Cardiologie", 250);
    cout << "Medic creat: " << m1 << endl;

    Asistent a1("Ionescu Maria", 4000, "Pediatrie");
    cout << "Asistent creat: " << a1 << endl;

    m1.setSalariu(8200);
    cout << "Salariu nou medic " << m1.getNume() << ": " << m1.getSalariu() << " RON" << endl;

    cout << "\n=== TESTARE Pacient ===" << endl;
    
    Pacient p1;
    cout << "Introduceti date pentru un pacient nou (Ex: Popa_Andrei Gripa 25):" << endl;
    cin >> p1;
    cout << "Pacientul introdus: " << p1 << endl;
    cout << "Varsta pacientului este: " << p1.getVarsta() << endl;

    cout << "\n=== TESTARE Programare ===" << endl;
    
    Programare prog1(&p1, &m1, "12.10.2026");
    cout << prog1 << endl;

    Programare prog2 = prog1;
    prog2.setData("15.10.2026");
    cout << "Copie programare (data modificata): " << endl << prog2 << endl;

    cout << "\n=== TESTARE Factura ===" << endl;
    
    Factura f1(p1.getNume(), m1.getTarif());
    cout << "Factura 1 generata: " << f1 << endl;

    Factura f2;
    f2.setSuma(300);
    cout << "Factura 2 generata: " << f2 << endl;
    cout << "Observatie: ID-ul a crescut automat (static contor)." << endl;

    f2.setSuma(150.5);
    cout << "Suma modificata factura 2: " << f2.getSuma() << " RON" << endl;

    cout << "\n=== TESTARE FINALIZATA CU SUCCES ===" << endl;

    return 0;

}