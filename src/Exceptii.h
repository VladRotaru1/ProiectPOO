#pragma once
#include <stdexcept>
#include <string>

// ============================================================
// Exceptii personalizate pentru sistemul spitalicesc
// ============================================================

class SpitalException : public std::runtime_error {
public:
    explicit SpitalException(const std::string& msg) : std::runtime_error(msg) {}
};

class CNPInvalidException : public SpitalException {
public:
    explicit CNPInvalidException(const std::string& cnp)
        : SpitalException("CNP invalid: " + cnp) {}
};

class ProgramareSuperupusaException : public SpitalException {
public:
    explicit ProgramareSuperupusaException(const std::string& data, const std::string& ora)
        : SpitalException("Programare suprapusa la data " + data + " ora " + ora) {}
};

class PatIndisponibilException : public SpitalException {
public:
    explicit PatIndisponibilException(const std::string& codPat)
        : SpitalException("Patul " + codPat + " este indisponibil") {}
};

class PacientInexistentException : public SpitalException {
public:
    explicit PacientInexistentException(const std::string& id)
        : SpitalException("Pacientul cu id/cod " + id + " nu exista in sistem") {}
};

class MedicInexistentException : public SpitalException {
public:
    explicit MedicInexistentException(const std::string& id)
        : SpitalException("Medicul cu id/cod " + id + " nu exista in sistem") {}
};

class SalariuInvalidException : public SpitalException {
public:
    explicit SalariuInvalidException(double val)
        : SpitalException("Salariu invalid: " + std::to_string(val) + " RON") {}
};

class StocInsuficientException : public SpitalException {
public:
    explicit StocInsuficientException(const std::string& produs, int disponibil, int cerut)
        : SpitalException("Stoc insuficient pentru " + produs +
                          " (disponibil: " + std::to_string(disponibil) +
                          ", cerut: " + std::to_string(cerut) + ")") {}
};

class AutentificareException : public SpitalException {
public:
    explicit AutentificareException(const std::string& msg)
        : SpitalException("Eroare autentificare: " + msg) {}
};

class PermisiuneException : public SpitalException {
public:
    explicit PermisiuneException(const std::string& actiune)
        : SpitalException("Nu aveti permisiunea pentru actiunea: " + actiune) {}
};

class SectieInexistentaException : public SpitalException {
public:
    explicit SectieInexistentaException(const std::string& numeSectie)
        : SpitalException("Sectia '" + numeSectie + "' nu exista in sistem") {}
};

class AparatDefectException : public SpitalException {
public:
    explicit AparatDefectException(const std::string& denumire)
        : SpitalException("Aparatul '" + denumire + "' este defect si nu poate fi folosit") {}
};
