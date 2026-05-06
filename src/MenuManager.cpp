#include "MenuManager.h"
#include "HospitalManager.h"
#include "AuthService.h"
#include "AuditLog.h"
#include "Exceptii.h"
#include <iostream>
#include <string>

void MenuManager::start() {
    auto hospital = HospitalManager::getInstance();
    auto auth = hospital->getAuthService();

    hospital->genereazaDateDemo();

    while (true) {
        if (!auth->esteAutentificat()) {
            std::cout << "\n========== AUTENTIFICARE ==========\n";
            std::string user, pass;
            std::cout << "Username (0 pt exit): ";
            std::cin >> user;
            if (user == "0") break;
            std::cout << "Parola: ";
            std::cin >> pass;

            if (auth->login(user, pass)) {
                std::cout << "  Login reusit! Bun venit, " << user << "!\n";
            } else {
                std::cout << "  Autentificare esuata. Incercati din nou.\n";
            }
        } else {
            Rol rol = auth->getUtilizatorCurent()->getRol();
            try {
                switch (rol) {
                    case Rol::ADMIN:    meniuAdmin();    break;
                    case Rol::MEDIC:    meniuMedic();    break;
                    case Rol::ASISTENT: meniuAsistent(); break;
                    case Rol::RECEPTIE: meniuReceptie(); break;
                }
            } catch (const std::exception& e) {
                std::cout << "\n  [EROARE] " << e.what() << "\n";
            }
        }
    }
}

void MenuManager::meniuAdmin() {
    auto hm = HospitalManager::getInstance();
    auto auth = hm->getAuthService();
    int opt;
    do {
        std::cout << "\n========== MENIU ADMIN ==========\n";
        std::cout << "1. Adaugare pacient\n";
        std::cout << "2. Afisare pacienti\n";
        std::cout << "3. Internare pacient\n";
        std::cout << "4. Externare pacient\n";
        std::cout << "5. Transfer sectie\n";
        std::cout << "6. Gestionare medici (Afisare)\n";
        std::cout << "7. Gestionare asistenti (Afisare)\n";
        std::cout << "8. Gestionare personal auxiliar (Afisare)\n";
        std::cout << "9. Gestionare inventar (Afisare complet)\n";
        std::cout << "10. Gestionare aparatura (Afisare)\n";
        std::cout << "11. Dashboard statistic\n";
        std::cout << "12. Audit log\n";
        std::cout << "13. Backup automat (Salvare date)\n";
        std::cout << "14. Gestionare utilizatori (Afisare)\n";
        std::cout << "15. Calcul salarii\n";
        std::cout << "16. Rapoarte financiare\n";
        std::cout << "17. Top medici\n";
        std::cout << "18. Produse expirate\n";
        std::cout << "19. Alertare stoc critic\n";
        std::cout << "0. Logout\n";
        std::cout << "Optiune: ";
        std::cin >> opt;
        
        try {
            switch(opt) {
                case 1: {
                    auto p = std::make_shared<Pacient>(Pacient::citesteDeLaConsola());
                    hm->adaugaPacient(p);
                    break;
                }
                case 2: hm->afisarePacienti(); break;
                case 3: {
                    std::string idPac, idMed, diag;
                    std::cout << "ID Pacient: "; std::cin >> idPac;
                    std::cout << "ID Medic responsabil: "; std::cin >> idMed;
                    std::cin >> std::ws;
                    std::cout << "Diagnostic internare: "; std::getline(std::cin, diag);
                    hm->realizeazaInternare(idPac, idMed, diag);
                    break;
                }
                case 4: {
                    std::string idInt, diagExt;
                    std::cout << "ID Internare: "; std::cin >> idInt;
                    std::cin >> std::ws;
                    std::cout << "Diagnostic externare: "; std::getline(std::cin, diagExt);
                    hm->externeazaPacient(idInt, diagExt);
                    break;
                }
                case 6: {
                    int sopt;
                    std::cout << "\n--- GESTIONARE MEDICI ---\n";
                    std::cout << "1. Afisare medici\n2. Angajare medic nou\n3. Concediere medic\n0. Inapoi\nOptiune: ";
                    std::cin >> sopt;
                    if (sopt == 1) hm->afisareMedici();
                    else if (sopt == 2) {
                        auto m = std::make_shared<Medic>(Medic::citesteDeLaConsola());
                        hm->adaugaMedic(m);
                        hm->salveazaDate();
                    } else if (sopt == 3) {
                        std::string id;
                        std::cout << "ID Medic de concediat: "; std::cin >> id;
                        hm->eliminaMedic(id);
                    }
                    break;
                }
                case 7: {
                    int sopt;
                    std::cout << "\n--- GESTIONARE ASISTENTI ---\n";
                    std::cout << "1. Afisare asistenti\n2. Angajare asistent nou\n3. Concediere asistent\n0. Inapoi\nOptiune: ";
                    std::cin >> sopt;
                    if (sopt == 1) hm->afisareAsistenti();
                    else if (sopt == 2) {
                        auto a = std::make_shared<Asistent>(Asistent::citesteDeLaConsola());
                        hm->adaugaAsistent(a);
                        hm->salveazaDate();
                    } else if (sopt == 3) {
                        std::string id;
                        std::cout << "ID Asistent de concediat: "; std::cin >> id;
                        hm->eliminaAsistent(id);
                    }
                    break;
                }
                case 8: {
                    int sopt;
                    std::cout << "\n--- GESTIONARE PERSONAL AUXILIAR ---\n";
                    std::cout << "1. Afisare personal auxiliar\n2. Angajare personal auxiliar\n3. Concediere personal auxiliar\n0. Inapoi\nOptiune: ";
                    std::cin >> sopt;
                    if (sopt == 1) hm->afisarePersonalAuxiliar();
                    else if (sopt == 2) {
                        auto p = std::make_shared<PersonalAuxiliar>(PersonalAuxiliar::citesteDeLaConsola());
                        hm->adaugaPersonalAuxiliar(p);
                        hm->salveazaDate();
                    } else if (sopt == 3) {
                        std::string id;
                        std::cout << "ID Personal auxiliar de concediat: "; std::cin >> id;
                        hm->eliminaPersonalAuxiliar(id);
                    }
                    break;
                }
                case 9: hm->getInventar()->afisare(); break;
                case 10: hm->afisareAparatura(); break;
                case 11: hm->afisareDashboardStatistic(); break;
                case 12: AuditLog::getInstance()->afisareLog(); break;
                case 13: hm->salveazaDate(); std::cout << "Datele au fost salvate cu succes.\n"; break;
                case 14: {
                    int sopt;
                    std::cout << "\n--- GESTIONARE UTILIZATORI ---\n";
                    std::cout << "1. Afisare utilizatori\n2. Creare utilizator nou\n3. Stergere utilizator\n0. Inapoi\nOptiune: ";
                    std::cin >> sopt;
                    if (sopt == 1) auth->afisareUtilizatori();
                    else if (sopt == 2) {
                        std::string u, p, pId;
                        int r;
                        std::cout << "Username: "; std::cin >> u;
                        std::cout << "Parola: "; std::cin >> p;
                        std::cout << "Rol (0-Admin, 1-Medic, 2-Asistent, 3-Receptie): "; std::cin >> r;
                        std::cout << "ID Persoana asociata (optional, '-' pt niciunul): "; std::cin >> pId;
                        User newUser(u, p, (Rol)r, (pId == "-" ? "" : pId));
                        if (auth->adaugaUtilizator(newUser)) std::cout << "  User creat cu succes.\n";
                        else std::cout << "  Eroare: Username-ul exista deja.\n";
                    } else if (sopt == 3) {
                        std::string u;
                        std::cout << "Username de sters: "; std::cin >> u;
                        if (auth->eliminaUtilizator(u)) std::cout << "  User sters.\n";
                        else std::cout << "  Eroare: User negasit.\n";
                    }
                    break;
                }
                case 15: hm->calculSalariiTotale(); break;
                case 16: hm->getFinanciarManager()->afisareRaport(); break;
                case 17: hm->topMedici(); break;
                case 18: hm->getInventar()->afisareExpirate(); break;
                case 19: hm->getInventar()->afisareStocCritic(); break;
                case 0: auth->logout(); break;
                default: if (opt != 0) std::cout << "Optiune invalida sau in constructie.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Eroare operatie: " << e.what() << "\n";
        }
    } while (opt != 0 && auth->esteAutentificat());
}

void MenuManager::meniuMedic() {
    auto hm = HospitalManager::getInstance();
    auto auth = hm->getAuthService();
    auto uid = auth->getUtilizatorCurent()->getIdPersoanaAsociata();
    int opt;
    do {
        std::cout << "\n========== MENIU MEDIC (" << uid << ") ==========\n";
        std::cout << "1. Vizualizare pacienti\n";
        std::cout << "2. Adaugare diagnostic (in consultatie)\n";
        std::cout << "3. Prescriere tratament (in consultatie)\n";
        std::cout << "4. Emitere reteta\n";
        std::cout << "5. Programari\n";
        std::cout << "6. Consultatii (Noua)\n";
        std::cout << "7. Laborator (Afisare analize pacient)\n";
        std::cout << "8. Rezultate analize (Vizualizare)\n";
        std::cout << "9. Istoric medical pacient\n";
        std::cout << "10. Monitorizare pacienti (Internari active)\n";
        std::cout << "11. Pacienti prioritari (Triage)\n";
        std::cout << "12. Dashboard medical\n";
        std::cout << "0. Logout\n";
        std::cout << "Optiune: ";
        std::cin >> opt;

        try {
            switch(opt) {
                case 1: hm->afisarePacienti(); break;
                case 4: {
                    std::string idPacient;
                    std::cout << "ID Pacient: "; std::cin >> idPacient;
                    Reteta r("RET100", "CONS100", idPacient, uid, "2026-05-05");
                    MedicamentReteta m; m.denumireMedicament="Paracetamol"; m.dozaj="1/zi"; m.numarZile=5;
                    r.adaugaMedicament(m);
                    r.genereazaFisier("Pacient_Demo", "Dr. " + uid);
                    break;
                }
                case 5: hm->afisareProgramariPentruMedic(uid); break;
                case 6: {
                    std::string idPacient;
                    std::cout << "ID Pacient: "; std::cin >> idPacient;
                    auto c = std::make_shared<Consultatie>("C101", "PRG1", idPacient, uid, "2026-05-05");
                    c->setDiagnostic("Raceala");
                    c->setRecomandari("Odihna");
                    hm->adaugaConsultatie(c);
                    std::cout << "Consultatie salvata.\n";
                    break;
                }
                case 2: {
                    std::string idInt, diag;
                    std::cout << "ID Internare pt adaugare diagnostic: "; std::cin >> idInt;
                    std::cin >> std::ws;
                    std::cout << "Diagnostic nou/suplimentar: "; std::getline(std::cin, diag);
                    auto in = hm->gasesteInternare(idInt);
                    if (in && in->isActiva()) {
                        in->setObservatii(in->getObservatii() + " | Diag: " + diag);
                        std::cout << "Diagnostic adaugat.\n";
                        hm->salveazaDate();
                    } else std::cout << "Internare inactiva sau negasita.\n";
                    break;
                }
                case 3: {
                    std::string idInt, trat;
                    std::cout << "ID Internare pt prescriere tratament: "; std::cin >> idInt;
                    std::cin >> std::ws;
                    std::cout << "Tratament prescris: "; std::getline(std::cin, trat);
                    auto in = hm->gasesteInternare(idInt);
                    if (in && in->isActiva()) {
                        in->setTratamentActiv(trat);
                        std::cout << "Tratament prescris cu succes.\n";
                        hm->salveazaDate();
                    } else std::cout << "Internare inactiva sau negasita.\n";
                    break;
                }
                case 7:
                case 8: {
                    std::string idPac;
                    std::cout << "ID Pacient: "; std::cin >> idPac;
                    hm->afisareAnalizePacient(idPac);
                    break;
                }
                case 9: {
                    std::string idPac;
                    std::cout << "ID Pacient: "; std::cin >> idPac;
                    auto p = hm->gasestePacient(idPac);
                    if(p) p->afisareFisaMedicala();
                    else std::cout << "Pacient negasit.\n";
                    break;
                }
                case 10: hm->afisareInternariActive(); break;
                case 11: {
                    std::cout << "\n--- Pacienti cu prioritate (Triage ROSU/GALBEN) ---\n";
                    for(const auto& p : hm->getPacienti()) {
                        if(p->getPrioritate() == PrioritateTriage::ROSU || p->getPrioritate() == PrioritateTriage::GALBEN) {
                            p->afisare();
                        }
                    }
                    break;
                }
                case 12: hm->afisareDashboardStatistic(); break;
                case 0: auth->logout(); break;
                default: if(opt != 0) std::cout << "Optiune invalida sau in constructie.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Eroare operatie: " << e.what() << "\n";
        }
    } while (opt != 0 && auth->esteAutentificat());
}

void MenuManager::meniuAsistent() {
    auto hm = HospitalManager::getInstance();
    auto auth = hm->getAuthService();
    int opt;
    do {
        std::cout << "\n========== MENIU ASISTENT ==========\n";
        std::cout << "1. Internari pacienti (Afisare active)\n";
        std::cout << "2. Externari pacienti\n";
        std::cout << "3. Monitorizare pacienti (Vitali)\n";
        std::cout << "4. Gestionare consumabile (Inventar)\n";
        std::cout << "5. Verificare stoc\n";
        std::cout << "6. Gestionare saloane (Afisare sectii)\n";
        std::cout << "7. Gestionare paturi (Afisare sectii)\n";
        std::cout << "8. Monitorizare aparatura\n";
        std::cout << "9. Raport tura\n";
        std::cout << "10. Dashboard\n";
        std::cout << "0. Logout\n";
        std::cout << "Optiune: ";
        std::cin >> opt;

        try {
            switch(opt) {
                case 1: hm->afisareInternariActive(); break;
                case 2: {
                    std::string idInt, diagExt;
                    std::cout << "ID Internare: "; std::cin >> idInt;
                    std::cin >> std::ws;
                    std::cout << "Diagnostic externare: "; std::getline(std::cin, diagExt);
                    hm->externeazaPacient(idInt, diagExt);
                    break;
                }
                case 3: {
                    std::string idInt, ritm;
                    std::cout << "ID Internare: "; std::cin >> idInt;
                    std::cout << "Ritm cardiac / Tensiune: "; 
                    std::cin >> std::ws; std::getline(std::cin, ritm);
                    auto in = hm->gasesteInternare(idInt);
                    if (in && in->isActiva()) {
                        in->adaugaParametriVitali("Acum", ritm);
                        std::cout << "Parametri salvati.\n";
                        hm->salveazaDate();
                    } else std::cout << "Internare inactiva sau invalida.\n";
                    break;
                }
                case 4: hm->getInventar()->afisare(); break;
                case 5: hm->getInventar()->afisareStocCritic(); break;
                case 6: 
                case 7: hm->afisareSectii(); break;
                case 8: hm->afisareAparatura(); break;
                case 9: {
                    std::cout << "\n========== RAPORT TURA ==========\n";
                    std::cout << "Internari active in sistem:\n";
                    hm->afisareInternariActive();
                    std::cout << "\nSectii si Paturi:\n";
                    hm->afisareSectii();
                    std::cout << "=================================\n";
                    break;
                }
                case 10: hm->afisareDashboardStatistic(); break;
                case 0: auth->logout(); break;
                default: if(opt != 0) std::cout << "Optiune invalida sau in constructie.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Eroare operatie: " << e.what() << "\n";
        }
    } while (opt != 0 && auth->esteAutentificat());
}

void MenuManager::meniuReceptie() {
    auto hm = HospitalManager::getInstance();
    auto auth = hm->getAuthService();
    int opt;
    do {
        std::cout << "\n========== MENIU RECEPTIE ==========\n";
        std::cout << "1. Adaugare pacient\n";
        std::cout << "2. Programare consultatie\n";
        std::cout << "3. Anulare programare\n";
        std::cout << "4. Verificare disponibilitate medic\n";
        std::cout << "5. Afisare programari\n";
        std::cout << "6. Facturare pacient\n";
        std::cout << "7. Pacienti neasigurati\n";
        std::cout << "8. Estimare timp asteptare\n";
        std::cout << "9. Cautare pacient\n";
        std::cout << "10. Dashboard receptie\n";
        std::cout << "0. Logout\n";
        std::cout << "Optiune: ";
        std::cin >> opt;

        try {
            switch(opt) {
                case 1: {
                    auto p = std::make_shared<Pacient>(Pacient::citesteDeLaConsola());
                    hm->adaugaPacient(p);
                    hm->salveazaDate();
                    break;
                }
                case 2: {
                    std::string idPac, idMed, data, ora;
                    std::cout << "ID Pacient: "; std::cin >> idPac;
                    std::cout << "ID Medic: "; std::cin >> idMed;
                    std::cout << "Data (AAAA-LL-ZZ): "; std::cin >> data;
                    std::cout << "Ora (HH:MM): "; std::cin >> ora;
                    auto prog = std::make_shared<Programare>("PRG_NOU", idPac, idMed, data, ora, "CAB1");
                    hm->adaugaProgramare(prog);
                    std::cout << "Programare inregistrata cu succes.\n";
                    hm->salveazaDate();
                    break;
                }
                case 3: {
                    std::string idProg;
                    std::cout << "ID Programare: "; std::cin >> idProg;
                    hm->anulareProgramare(idProg);
                    hm->salveazaDate();
                    break;
                }
                case 4: {
                    std::cout << "Vizualizare programari pentru a verifica disponibilitatea...\n";
                    [[fallthrough]];
                }
                case 5: {
                    std::string idMed;
                    std::cout << "ID Medic (sau 0 pt toate): "; std::cin >> idMed;
                    if(idMed == "0") {
                        for(const auto& p : hm->getProgramari()) p->afisare();
                    } else {
                        hm->afisareProgramariPentruMedic(idMed);
                    }
                    break;
                }
                case 6: {
                    std::cout << "Facturile pot fi vazute in raportul financiar, sau aici (demo)\n";
                    hm->getFinanciarManager()->afisareFacturiNePlatite();
                    std::string idF;
                    std::cout << "ID Factura pentru a marca platita (sau 0): ";
                    std::cin >> idF;
                    if(idF != "0") hm->getFinanciarManager()->marcheazaPlatita(idF);
                    break;
                }
                case 7: {
                    std::cout << "\n--- Pacienti neasigurati (fara CAS) ---\n";
                    for(const auto& p : hm->getPacienti()) {
                        if(!p->isAsiguratCAS()) p->afisare();
                    }
                    break;
                }
                case 8: {
                    std::string idMed, data, ora;
                    std::cout << "ID Medic: "; std::cin >> idMed;
                    std::cout << "Data (AAAA-LL-ZZ): "; std::cin >> data;
                    std::cout << "Ora (HH:MM): "; std::cin >> ora;
                    int est = hm->estimareTimpAsteptare(idMed, data, ora);
                    std::cout << "Timp estimat de asteptare: " << est << " minute.\n";
                    break;
                }
                case 9: {
                    std::string cnp;
                    std::cout << "Introduceti ID / CNP: "; std::cin >> cnp;
                    auto p = hm->gasestePacient(cnp);
                    if (p) p->afisare();
                    else std::cout << "Nu a fost gasit.\n";
                    break;
                }
                case 10: hm->afisareDashboardStatistic(); break;
                case 0: auth->logout(); break;
                default: if(opt != 0) std::cout << "Optiune invalida sau in constructie.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Eroare operatie: " << e.what() << "\n";
        }
    } while (opt != 0 && auth->esteAutentificat());
}
