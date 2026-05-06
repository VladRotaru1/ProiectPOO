#include "HospitalManager.h"
#include "AuditLog.h"
#include "NotificationService.h"
#include <iostream>
#include <fstream>
#include <algorithm>

// ============================================================
// Factory Implementation
// ============================================================
std::shared_ptr<Persoana> PersonFactory::creazaPersoana(const std::string& tip) {
    if (tip == "PACIENT") return std::make_shared<Pacient>();
    if (tip == "MEDIC") return std::make_shared<Medic>();
    if (tip == "ASISTENT") return std::make_shared<Asistent>();
    if (tip == "AUXILIAR") return std::make_shared<PersonalAuxiliar>();
    return nullptr;
}

std::shared_ptr<Produs> ProductFactory::creazaProdus(const std::string& tip) {
    if (tip == "MEDICAMENT") return std::make_shared<Medicament>();
    if (tip == "CONSUMABIL") return std::make_shared<Consumabil>();
    if (tip == "ECHIPAMENT") return std::make_shared<Echipament>();
    return nullptr;
}

// ============================================================
// HospitalManager Implementation
// ============================================================
// HospitalManager Implementation

HospitalManager* HospitalManager::getInstance() {
    static HospitalManager instance;
    return &instance;
}

HospitalManager::HospitalManager() {
    authService = std::make_shared<AuthService>("users.txt");
    financiarManager = std::make_shared<FinanciarManager>("facturi.txt");
    inventar = std::make_shared<Inventar<Produs>>("inventar.txt", 10);

    // Initializam observatori
    auto consoleObs = std::make_shared<ConsoleObserver>("MainConsole");
    auto fileObs = std::make_shared<FileObserver>("alerte_sistem.log");
    NotificationService::getInstance()->inregistreazaObserver(consoleObs);
    NotificationService::getInstance()->inregistreazaObserver(fileObs);

    incarcaDate();
}

// ============================================================
// Persistenta (incarcare/salvare din fisiere text)
// ============================================================
void HospitalManager::incarcaDate() {
    // Incarcare Pacienti
    std::ifstream fPacienti("pacienti.txt");
    if (fPacienti.is_open()) {
        std::string linie;
        while (std::getline(fPacienti, linie)) {
            if (linie.empty()) continue;
            auto p = std::make_shared<Pacient>();
            p->deserializa(linie);
            pacienti.push_back(p);
        }
    }

    // Incarcare Medici
    std::ifstream fMedici("medici.txt");
    if (fMedici.is_open()) {
        std::string linie;
        while (std::getline(fMedici, linie)) {
            if (linie.empty()) continue;
            auto m = std::make_shared<Medic>();
            m->deserializa(linie);
            medici.push_back(m);
        }
    }

    // Incarcare Asistenti
    std::ifstream fAsistenti("asistenti.txt");
    if (fAsistenti.is_open()) {
        std::string linie;
        while (std::getline(fAsistenti, linie)) {
            if (linie.empty()) continue;
            auto a = std::make_shared<Asistent>();
            a->deserializa(linie);
            asistenti.push_back(a);
        }
    }

    // Incarcare Sectii
    std::ifstream fSectii("sectii.txt");
    if (fSectii.is_open()) {
        std::string linie;
        while (std::getline(fSectii, linie)) {
            if (linie.empty()) continue;
            auto s = std::make_shared<Sectie>();
            s->deserializa(linie);
            sectii.push_back(s);
        }
    }

    // Incarcare Inventar
    std::ifstream fInventar("inventar.txt");
    if (fInventar.is_open()) {
        std::string linie;
        while (std::getline(fInventar, linie)) {
            if (linie.empty()) continue;
            auto pos = linie.find('|');
            if (pos != std::string::npos) {
                std::string tip = linie.substr(0, pos);
                auto prod = ProductFactory::creazaProdus(tip);
                if (prod) {
                    prod->deserializa(linie);
                    inventar->adaugaProdus(prod);
                }
            }
        }
    }

    // Incarcare Programari
    std::ifstream fProgramari("programari.txt");
    if (fProgramari.is_open()) {
        std::string linie;
        while (std::getline(fProgramari, linie)) {
            if (linie.empty()) continue;
            auto p = std::make_shared<Programare>();
            p->deserializa(linie);
            programari.push_back(p);
        }
    }

    // Incarcare Internari
    std::ifstream fInternari("internari.txt");
    if (fInternari.is_open()) {
        std::string linie;
        while (std::getline(fInternari, linie)) {
            if (linie.empty()) continue;
            auto i = std::make_shared<Internare>();
            i->deserializa(linie);
            internari.push_back(i);
        }
    }
    
    // Incarcare Consultatii
    std::ifstream fConsultatii("consultatii.txt");
    if (fConsultatii.is_open()) {
        std::string linie;
        while (std::getline(fConsultatii, linie)) {
            if (linie.empty()) continue;
            auto c = std::make_shared<Consultatie>();
            c->deserializa(linie);
            consultatii.push_back(c);
        }
    }

    // Incarcare Analize
    std::ifstream fAnalize("analize.txt");
    if (fAnalize.is_open()) {
        std::string linie;
        while (std::getline(fAnalize, linie)) {
            if (linie.empty()) continue;
            auto a = std::make_shared<AnalizaLaborator>();
            a->deserializa(linie);
            analize.push_back(a);
        }
    }
}

void HospitalManager::salveazaDate() {
    {
        std::ofstream f("pacienti.txt");
        for (const auto& p : pacienti) f << p->serializa() << "\n";
    }
    {
        std::ofstream f("medici.txt");
        for (const auto& m : medici) f << m->serializa() << "\n";
    }
    {
        std::ofstream f("asistenti.txt");
        for (const auto& a : asistenti) f << a->serializa() << "\n";
    }
    {
        std::ofstream f("sectii.txt");
        for (const auto& s : sectii) f << s->serializa() << "\n";
    }
    {
        std::ofstream f("programari.txt");
        for (const auto& p : programari) f << p->serializa() << "\n";
    }
    {
        std::ofstream f("internari.txt");
        for (const auto& i : internari) f << i->serializa() << "\n";
    }
    {
        std::ofstream f("consultatii.txt");
        for (const auto& c : consultatii) f << c->serializa() << "\n";
    }
    {
        std::ofstream f("analize.txt");
        for (const auto& a : analize) f << a->serializa() << "\n";
    }
    inventar->salveaza();
    authService->salveaza();
    financiarManager->salveaza();
}

void HospitalManager::genereazaDateDemo() {
    if (!medici.empty()) return; // avem deja date

    std::cout << "Generare date DEMO...\n";

    // Sectii
    auto s1 = std::make_shared<Sectie>("SEC1", "Cardiologie", 1);
    s1->adaugaSalon(Salon("101", TipSalon::STANDARD, 4));
    s1->adaugaSalon(Salon("102", TipSalon::ATI, 2));
    
    auto s2 = std::make_shared<Sectie>("SEC2", "Chirurgie", 2);
    s2->adaugaSalon(Salon("201", TipSalon::STANDARD, 5));
    s2->adaugaSalon(Salon("202", TipSalon::IZOLARE, 1));

    adaugaSectie(s1);
    adaugaSectie(s2);

    // Medici
    auto m1 = std::make_shared<Medic>("MED1", "Popescu", "Ion", "1800101000000", "M", "01.01.1980",
                                      "0700000000", "ion@spital.ro", "Bucuresti", "2010-05-10", 12000,
                                      "Cardiologie", GradProfesional::PRIMAR, "P123");
    auto m2 = std::make_shared<Medic>("MED2", "Ionescu", "Maria", "2850101000000", "F", "01.01.1985",
                                      "0700000001", "maria@spital.ro", "Bucuresti", "2015-02-01", 10000,
                                      "Chirurgie", GradProfesional::SPECIALIST, "P456");
    adaugaMedic(m1);
    adaugaMedic(m2);
    s1->adaugaMedic(m1->getCodAngajat());
    s2->adaugaMedic(m2->getCodAngajat());

    // Asistenti
    auto a1 = std::make_shared<Asistent>("ASI1", "Vasile", "Elena", "2900101000000", "F", "01.01.1990",
                                         "0700000002", "elena@spital.ro", "Bucuresti", "2018-03-15", 5000,
                                         "Cardiologie", "rotatie");
    adaugaAsistent(a1);
    s1->adaugaAsistent(a1->getCodAngajat());

    // Pacienti
    auto p1 = std::make_shared<Pacient>("PAC1", "Radu", "Andrei", "1950101000000", "M", "01.01.1995",
                                        "0700000003", "andrei@mail.com", "Cluj", true, "A2+", "0711111111");
    p1->adaugaAlergie("Penicilina");
    p1->adaugaBoalaCronica("Hipertensiune");
    adaugaPacient(p1);

    // Inventar
    auto med1 = std::make_shared<Medicament>("PRD1", "Paracetamol", 100, 15.5, "2028-12-31",
                                             "Paracetamol 500mg", "comprimate", false);
    auto cons1 = std::make_shared<Consumabil>("PRD2", "Seringi 5ml", 500, 0.5, "2030-01-01",
                                              "Materiale", "bucati");
    adaugaProdusInventar(med1);
    adaugaProdusInventar(cons1);

    salveazaDate();
}

// ==========================================
// PACIENTI
// ==========================================
void HospitalManager::adaugaPacient(std::shared_ptr<Pacient> p) {
    pacienti.push_back(p);
    AuditLog::getInstance()->log("SYSTEM", "ADAUGARE_PACIENT: " + p->getCodPacient());
}

std::shared_ptr<Pacient> HospitalManager::gasestePacient(const std::string& idOrCnp) {
    for (auto& p : pacienti)
        if (p->getCodPacient() == idOrCnp || p->getCnp() == idOrCnp) return p;
    return nullptr;
}

void HospitalManager::afisarePacienti() const {
    std::cout << "\n========== LISTA PACIENTI (" << pacienti.size() << ") ==========\n";
    for (const auto& p : pacienti) p->afisare();
    std::cout << "==============================================\n";
}

// ==========================================
// MEDICI & PERSONAL
// ==========================================
void HospitalManager::adaugaMedic(std::shared_ptr<Medic> m) {
    medici.push_back(m);
    AuditLog::getInstance()->log("SYSTEM", "ADAUGARE_MEDIC: " + m->getCodAngajat());
}

std::shared_ptr<Medic> HospitalManager::gasesteMedic(const std::string& id) {
    for (auto& m : medici)
        if (m->getCodAngajat() == id) return m;
    return nullptr;
}

void HospitalManager::afisareMedici() const {
    std::cout << "\n========== LISTA MEDICI (" << medici.size() << ") ==========\n";
    for (const auto& m : medici) m->afisare();
    std::cout << "==============================================\n";
}

void HospitalManager::adaugaAsistent(std::shared_ptr<Asistent> a) {
    asistenti.push_back(a);
    AuditLog::getInstance()->log("SYSTEM", "ADAUGARE_ASISTENT: " + a->getCodAngajat());
}

std::shared_ptr<Asistent> HospitalManager::gasesteAsistent(const std::string& id) {
    for (auto& a : asistenti)
        if (a->getCodAngajat() == id) return a;
    return nullptr;
}

void HospitalManager::afisareAsistenti() const {
    std::cout << "\n========== LISTA ASISTENTI (" << asistenti.size() << ") ==========\n";
    for (const auto& a : asistenti) a->afisare();
    std::cout << "=================================================\n";
}

void HospitalManager::adaugaPersonalAuxiliar(std::shared_ptr<PersonalAuxiliar> p) {
    personalAux.push_back(p);
    AuditLog::getInstance()->log("SYSTEM", "ADAUGARE_AUXILIAR: " + p->getCodAngajat());
}

void HospitalManager::afisarePersonalAuxiliar() const {
    std::cout << "\n========== LISTA PERSONAL AUXILIAR (" << personalAux.size() << ") ==========\n";
    for (const auto& p : personalAux) p->afisare();
    std::cout << "========================================================\n";
}

void HospitalManager::eliminaMedic(const std::string& id) {
    auto it = std::remove_if(medici.begin(), medici.end(),
        [&](const std::shared_ptr<Medic>& m) { return m->getCodAngajat() == id; });
    if (it != medici.end()) {
        medici.erase(it, medici.end());
        // Eliminare din sectii
        for (auto& s : sectii) s->eliminaMedic(id);
        AuditLog::getInstance()->log("SYSTEM", "ELIMINARE_MEDIC: " + id);
        salveazaDate();
        std::cout << "  Medicul " << id << " a fost eliminat din sistem.\n";
    } else {
        std::cout << "  Medicul " << id << " nu a fost gasit.\n";
    }
}

void HospitalManager::eliminaAsistent(const std::string& id) {
    auto it = std::remove_if(asistenti.begin(), asistenti.end(),
        [&](const std::shared_ptr<Asistent>& a) { return a->getCodAngajat() == id; });
    if (it != asistenti.end()) {
        asistenti.erase(it, asistenti.end());
        // Eliminare din sectii
        for (auto& s : sectii) s->eliminaAsistent(id);
        AuditLog::getInstance()->log("SYSTEM", "ELIMINARE_ASISTENT: " + id);
        salveazaDate();
        std::cout << "  Asistentul " << id << " a fost eliminat din sistem.\n";
    } else {
        std::cout << "  Asistentul " << id << " nu a fost gasit.\n";
    }
}

void HospitalManager::eliminaPersonalAuxiliar(const std::string& id) {
    auto it = std::remove_if(personalAux.begin(), personalAux.end(),
        [&](const std::shared_ptr<PersonalAuxiliar>& p) { return p->getCodAngajat() == id; });
    if (it != personalAux.end()) {
        personalAux.erase(it, personalAux.end());
        AuditLog::getInstance()->log("SYSTEM", "ELIMINARE_AUXILIAR: " + id);
        salveazaDate();
        std::cout << "  Personalul auxiliar " << id << " a fost eliminat din sistem.\n";
    } else {
        std::cout << "  Personalul auxiliar " << id << " nu a fost gasit.\n";
    }
}

void HospitalManager::calculSalariiTotale() const {
    double total = 0;
    std::cout << "\n========== CALCUL SALARII ==========\n";
    for (const auto& m : medici) {
        std::cout << "  Medic " << m->getNumeComplet() << ": " << m->calculeazaSalariuNet() << " RON\n";
        total += m->calculeazaSalariuNet();
    }
    for (const auto& a : asistenti) {
        std::cout << "  Asistent " << a->getNumeComplet() << ": " << a->calculeazaSalariuNet() << " RON\n";
        total += a->calculeazaSalariuNet();
    }
    for (const auto& p : personalAux) {
        std::cout << "  Auxiliar " << p->getNumeComplet() << ": " << p->calculeazaSalariuNet() << " RON\n";
        total += p->calculeazaSalariuNet();
    }
    std::cout << "------------------------------------\n";
    std::cout << "TOTAL SALARII: " << total << " RON\n";
    std::cout << "====================================\n";
}

// ==========================================
// SECTII
// ==========================================
void HospitalManager::adaugaSectie(std::shared_ptr<Sectie> s) {
    sectii.push_back(s);
}

std::shared_ptr<Sectie> HospitalManager::gasesteSectie(const std::string& id) {
    for (auto& s : sectii)
        if (s->getIdSectie() == id || s->getNumeSectie() == id) return s;
    return nullptr;
}

void HospitalManager::afisareSectii() const {
    std::cout << "\n========== SECTII SPITAL (" << sectii.size() << ") ==========\n";
    for (const auto& s : sectii) s->afisare();
    std::cout << "==============================================\n";
}

// ==========================================
// PROGRAMARI & CONSULTATII
// ==========================================
void HospitalManager::adaugaProgramare(std::shared_ptr<Programare> p) {
    // Verificare suprapunere
    for (const auto& prog : programari) {
        if (prog->suprapuneCu(*p))
            throw ProgramareSuperupusaException(p->getData(), p->getOra());
    }
    programari.push_back(p);
    AuditLog::getInstance()->log("SYSTEM", "PROGRAMARE_NOUA: " + p->getIdProgramare());
}

void HospitalManager::anulareProgramare(const std::string& id) {
    auto p = gasesteProgramare(id);
    if (p) {
        p->anuleaza();
        AuditLog::getInstance()->log("SYSTEM", "ANULARE_PROGRAMARE: " + id);
    }
}

std::shared_ptr<Programare> HospitalManager::gasesteProgramare(const std::string& id) {
    for (auto& p : programari)
        if (p->getIdProgramare() == id) return p;
    return nullptr;
}

void HospitalManager::afisareProgramariPentruMedic(const std::string& idMedic) const {
    std::cout << "\n--- Programari pentru Medicul " << idMedic << " ---\n";
    bool gasit = false;
    for (const auto& p : programari) {
        if (p->getIdMedic() == idMedic && p->getStatus() == StatusProgramare::ACTIVA) {
            p->afisare();
            gasit = true;
        }
    }
    if (!gasit) std::cout << "Nu exista programari active.\n";
}

int HospitalManager::estimareTimpAsteptare(const std::string& idMedic, const std::string& data, const std::string& ora) const {
    // Folosim o instanta dummy pentru a apela metoda din Programare
    Programare dummy("", "", idMedic, data, ora, "");
    return dummy.estimareTimpAsteptare(programari, idMedic);
}

void HospitalManager::adaugaConsultatie(std::shared_ptr<Consultatie> c) {
    consultatii.push_back(c);
    auto pac = gasestePacient(c->getIdPacient());
    if (pac) {
        IntrareIstoricMedical istoric;
        istoric.data = c->getDataConsultatiei();
        istoric.diagnostic = c->getDiagnostic();
        istoric.tratament = c->getTratament();
        istoric.medicResponsabil = c->getIdMedic();
        istoric.observatii = c->getRecomandari();
        pac->adaugaIntrareIstoricMedical(istoric);
        pac->incrementeazaVizite();
    }
    auto med = gasesteMedic(c->getIdMedic());
    if (med) {
        med->incrementeazaConsultatii();
    }
    AuditLog::getInstance()->log("SYSTEM", "CONSULTATIE_ADAUGARE: " + c->getIdConsultatie());
}

// ==========================================
// INTERNARI
// ==========================================
bool HospitalManager::realizeazaInternare(const std::string& idPacient, const std::string& idMedic,
                                          const std::string& diagnostic) {
    auto pac = gasestePacient(idPacient);
    if (!pac) throw PacientInexistentException(idPacient);
    
    // Cautare pat liber in toate sectiile
    std::string nSectie, nSalon, cPat;
    bool gasit = false;
    for (auto& sect : sectii) {
        if (sect->ocupa(idPacient, nSalon, cPat)) {
            nSectie = sect->getIdSectie();
            gasit = true;
            break;
        }
    }
    if (!gasit) {
        NotificationService::getInstance()->notifica("INTERNARE_ESUATA", "Nu exista paturi libere pentru pacientul " + idPacient);
        return false;
    }

    // Creare inregistrare internare
    std::string idIntern = "INT" + std::to_string(internari.size() + 1);
    
    // Data curenta
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    
    auto intObj = std::make_shared<Internare>(idIntern, idPacient, idMedic, nSectie, nSalon, cPat, buf, diagnostic);
    internari.push_back(intObj);

    NotificationService::getInstance()->notifica("INTERNARE_NOUA", "Pacient " + idPacient + " internat in " + nSectie + " / Pat: " + cPat);
    AuditLog::getInstance()->log("SYSTEM", "INTERNARE_REALIZATA: " + idIntern);
    salveazaDate();
    return true;
}

void HospitalManager::externeazaPacient(const std::string& idInternare, const std::string& diagExt) {
    auto in = gasesteInternare(idInternare);
    if (!in || !in->isActiva()) return;

    // Data curenta
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);

    in->externeaza(buf, diagExt);

    // Eliberare pat
    auto sect = gasesteSectie(in->getIdSectie());
    if (sect) {
        sect->elibereazaPat(in->getCodPat());
    }

    // Facturare (simulare cost)
    auto pac = gasestePacient(in->getIdPacient());
    double cost = 1500.0; // baza
    in->setCostTotal(cost);
    if (pac) {
        financiarManager->emiteFactura(pac->getCodPacient(), idInternare, cost, pac->isAsiguratCAS(), buf, "Cost internare " + idInternare);
    }

    NotificationService::getInstance()->notifica("EXTERNARE", "Pacientul aferent internarii " + idInternare + " a fost externat.");
    AuditLog::getInstance()->log("SYSTEM", "EXTERNARE_REALIZATA: " + idInternare);
    salveazaDate();
}

std::shared_ptr<Internare> HospitalManager::gasesteInternare(const std::string& id) {
    for (auto& i : internari)
        if (i->getIdInternare() == id) return i;
    return nullptr;
}

void HospitalManager::afisareInternariActive() const {
    std::cout << "\n========== INTERNARI ACTIVE ==========\n";
    int count = 0;
    for (const auto& i : internari) {
        if (i->isActiva()) {
            i->afisare();
            count++;
        }
    }
    if (count == 0) std::cout << "Nu exista internari active.\n";
    std::cout << "======================================\n";
}

// ==========================================
// INVENTAR
// ==========================================
void HospitalManager::adaugaProdusInventar(std::shared_ptr<Produs> p) {
    inventar->adaugaProdus(p);
}

void HospitalManager::consumaProdus(const std::string& idProdus, int cantitate) {
    inventar->scadeStoc(idProdus, cantitate);
}

// ==========================================
// DASHBOARD & STATISTICI
// ==========================================
void HospitalManager::afisareDashboardStatistic() const {
    std::cout << "\n============= DASHBOARD STATISTIC =============\n";
    std::cout << "Total Pacienti:   " << pacienti.size() << "\n";
    std::cout << "Total Medici:     " << medici.size() << "\n";
    std::cout << "Total Asistenti:  " << asistenti.size() << "\n";
    
    int paturiTotale = 0, paturiOcupate = 0;
    for (const auto& s : sectii) {
        paturiTotale += s->getPaturiLibereTotal() + s->getPaturiOcupateTotal();
        paturiOcupate += s->getPaturiOcupateTotal();
    }
    double gradOcupare = (paturiTotale > 0) ? (double)paturiOcupate / paturiTotale * 100 : 0;
    
    std::cout << "Paturi (Ocupate/Total): " << paturiOcupate << "/" << paturiTotale 
              << " (" << gradOcupare << "%)\n";
              
    int progA = 0;
    for (const auto& p : programari) if (p->getStatus() == StatusProgramare::ACTIVA) progA++;
    std::cout << "Programari active: " << progA << "\n";
    
    int intA = 0;
    for (const auto& i : internari) if (i->isActiva()) intA++;
    std::cout << "Internari active:  " << intA << "\n";
    
    std::cout << "Produse Inventar:  " << inventar->size() << "\n";
    std::cout << "===============================================\n";
}

void HospitalManager::topMedici() const {
    std::vector<std::shared_ptr<Medic>> sortati = medici;
    std::sort(sortati.begin(), sortati.end(), [](const std::shared_ptr<Medic>& a, const std::shared_ptr<Medic>& b) {
        return a->getNumarConsultatii() > b->getNumarConsultatii(); // Sortare descrescatoare
    });
    
    std::cout << "\n========== TOP MEDICI (dupa consultatii) ==========\n";
    int limita = std::min(5, (int)sortati.size());
    for (int i = 0; i < limita; i++) {
        std::cout << (i+1) << ". " << sortati[i]->getNumeComplet() 
                  << " (Spec: " << sortati[i]->getSpecializare() << ")"
                  << " - Consultatii: " << sortati[i]->getNumarConsultatii() << "\n";
    }
    std::cout << "=====================================================\n";
}

// ==========================================
// APARATURA
// ==========================================
void HospitalManager::adaugaAparatura(std::shared_ptr<AparaturaMedicala> a) {
    aparatura.push_back(a);
}

std::shared_ptr<AparaturaMedicala> HospitalManager::gasesteAparatura(const std::string& id) {
    for (auto& a : aparatura)
        if (a->getIdAparatura() == id) return a;
    return nullptr;
}

void HospitalManager::afisareAparatura() const {
    std::cout << "\n========== APARATURA MEDICALA (" << aparatura.size() << ") ==========\n";
    for (const auto& a : aparatura) a->afisare();
    std::cout << "=================================================\n";
}

// ==========================================
// LABORATOR
// ==========================================
void HospitalManager::adaugaAnaliza(std::shared_ptr<AnalizaLaborator> a) {
    analize.push_back(a);
}

std::shared_ptr<AnalizaLaborator> HospitalManager::gasesteAnaliza(const std::string& id) {
    for (auto& a : analize)
        if (a->getIdAnaliza() == id) return a;
    return nullptr;
}

void HospitalManager::afisareAnalizePacient(const std::string& idPacient) const {
    std::cout << "\n--- Analize pentru pacientul " << idPacient << " ---\n";
    bool gasit = false;
    for (const auto& a : analize) {
        if (a->getIdPacient() == idPacient) {
            a->afisareCompleta();
            gasit = true;
        }
    }
    if (!gasit) std::cout << "Nu exista analize inregistrate.\n";
}
