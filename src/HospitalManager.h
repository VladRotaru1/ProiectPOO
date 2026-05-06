#pragma once
#include "Pacient.h"
#include "Medic.h"
#include "Asistent.h"
#include "PersonalAuxiliar.h"
#include "Sectie.h"
#include "Programare.h"
#include "Internare.h"
#include "Consultatie.h"
#include "AnalizaLaborator.h"
#include "AparaturaMedicala.h"
#include "Inventar.h"
#include "AuthService.h"
#include "FinanciarManager.h"
#include "Exceptii.h"

#include <vector>
#include <string>
#include <memory>

// ============================================================
// Factory Pattern - pentru crearea dinamica a obiectelor
// ============================================================
class PersonFactory {
public:
    static std::shared_ptr<Persoana> creazaPersoana(const std::string& tip);
};

class ProductFactory {
public:
    static std::shared_ptr<Produs> creazaProdus(const std::string& tip);
};

// ============================================================
// HospitalManager - "creierul" aplicatiei, leaga toate modulele
// ============================================================
class HospitalManager {
private:
    std::vector<std::shared_ptr<Pacient>> pacienti;
    std::vector<std::shared_ptr<Medic>> medici;
    std::vector<std::shared_ptr<Asistent>> asistenti;
    std::vector<std::shared_ptr<PersonalAuxiliar>> personalAux;
    std::vector<std::shared_ptr<Sectie>> sectii;
    std::vector<std::shared_ptr<Programare>> programari;
    std::vector<std::shared_ptr<Internare>> internari;
    std::vector<std::shared_ptr<Consultatie>> consultatii;
    std::vector<std::shared_ptr<AnalizaLaborator>> analize;
    std::vector<std::shared_ptr<AparaturaMedicala>> aparatura;

    std::shared_ptr<Inventar<Produs>> inventar;
    std::shared_ptr<AuthService> authService;
    std::shared_ptr<FinanciarManager> financiarManager;

    HospitalManager();
    HospitalManager(const HospitalManager&) = delete;
    HospitalManager& operator=(const HospitalManager&) = delete;

public:
    static HospitalManager* getInstance();

    // Module
    std::shared_ptr<AuthService> getAuthService() { return authService; }
    std::shared_ptr<FinanciarManager> getFinanciarManager() { return financiarManager; }
    std::shared_ptr<Inventar<Produs>> getInventar() { return inventar; }

    // Init si persistenta
    void incarcaDate();
    void salveazaDate();
    void genereazaDateDemo();

    // ==========================================
    // PACIENTI
    // ==========================================
    void adaugaPacient(std::shared_ptr<Pacient> p);
    std::shared_ptr<Pacient> gasestePacient(const std::string& idOrCnp);
    void afisarePacienti() const;

    // ==========================================
    // PERSONAL
    // ==========================================
    void adaugaMedic(std::shared_ptr<Medic> m);
    std::shared_ptr<Medic> gasesteMedic(const std::string& id);
    void afisareMedici() const;
    void eliminaMedic(const std::string& id);

    void adaugaAsistent(std::shared_ptr<Asistent> a);
    std::shared_ptr<Asistent> gasesteAsistent(const std::string& id);
    void afisareAsistenti() const;
    void eliminaAsistent(const std::string& id);

    void adaugaPersonalAuxiliar(std::shared_ptr<PersonalAuxiliar> p);
    void afisarePersonalAuxiliar() const;
    void eliminaPersonalAuxiliar(const std::string& id);
    void calculSalariiTotale() const;

    const std::vector<std::shared_ptr<Pacient>>& getPacienti() const { return pacienti; }
    const std::vector<std::shared_ptr<Programare>>& getProgramari() const { return programari; }

    // ==========================================
    // SECTII, SALOANE, PATURI
    // ==========================================
    void adaugaSectie(std::shared_ptr<Sectie> s);
    std::shared_ptr<Sectie> gasesteSectie(const std::string& id);
    void afisareSectii() const;

    // ==========================================
    // PROGRAMARI SI CONSULTATII
    // ==========================================
    void adaugaProgramare(std::shared_ptr<Programare> p);
    void anulareProgramare(const std::string& id);
    std::shared_ptr<Programare> gasesteProgramare(const std::string& id);
    void afisareProgramariPentruMedic(const std::string& idMedic) const;
    int estimareTimpAsteptare(const std::string& idMedic, const std::string& data, const std::string& ora) const;

    void adaugaConsultatie(std::shared_ptr<Consultatie> c);

    // ==========================================
    // INTERNARI
    // ==========================================
    bool realizeazaInternare(const std::string& idPacient, const std::string& idMedic,
                             const std::string& diagnostic);
    void externeazaPacient(const std::string& idInternare, const std::string& diagExt);
    void transferaSectie(const std::string& idInternare, const std::string& idSectieNoua);
    void afisareInternariActive() const;
    std::shared_ptr<Internare> gasesteInternare(const std::string& id);

    // ==========================================
    // LABORATOR
    // ==========================================
    void adaugaAnaliza(std::shared_ptr<AnalizaLaborator> a);
    std::shared_ptr<AnalizaLaborator> gasesteAnaliza(const std::string& id);
    void afisareAnalizePacient(const std::string& idPacient) const;

    // ==========================================
    // APARATURA
    // ==========================================
    void adaugaAparatura(std::shared_ptr<AparaturaMedicala> a);
    std::shared_ptr<AparaturaMedicala> gasesteAparatura(const std::string& id);
    void afisareAparatura() const;

    // ==========================================
    // INVENTAR
    // ==========================================
    void adaugaProdusInventar(std::shared_ptr<Produs> p);
    void consumaProdus(const std::string& idProdus, int cantitate);

    // ==========================================
    // DASHBOARD & STATISTICI
    // ==========================================
    void afisareDashboardStatistic() const;
    void topMedici() const;
};
