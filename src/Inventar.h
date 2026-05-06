#pragma once
#include "Produs.h"
#include "NotificationService.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// ============================================================
// Inventar<T> - template pentru gestiunea stocurilor
// Functioneaza cu orice tip derivat din Produs
// ============================================================
template<typename T>
class Inventar {
private:
    std::vector<std::shared_ptr<T>> produse;
    std::string numeFisier;
    int pragStocCritic;

public:
    explicit Inventar(const std::string& fisier = "inventar.txt", int prag = 5)
        : numeFisier(fisier), pragStocCritic(prag) {}

    // Adauga produs
    void adaugaProdus(std::shared_ptr<T> produs) {
        produse.push_back(produs);
        verificaAlerte();
    }

    // Elimina produs dupa id
    bool eliminaProdus(const std::string& idProdus) {
        auto it = std::find_if(produse.begin(), produse.end(),
            [&](const std::shared_ptr<T>& p) { return p->getIdProdus() == idProdus; });
        if (it != produse.end()) {
            produse.erase(it);
            return true;
        }
        return false;
    }

    // Gaseste produs dupa id
    std::shared_ptr<T> gasesteProdus(const std::string& idProdus) const {
        for (const auto& p : produse)
            if (p->getIdProdus() == idProdus) return p;
        return nullptr;
    }

    // Gaseste produs dupa denumire
    std::shared_ptr<T> gasesteDupaDenumire(const std::string& denumire) const {
        for (const auto& p : produse) {
            std::string den = p->getDenumire();
            std::string dn = denumire;
            std::transform(den.begin(), den.end(), den.begin(), ::tolower);
            std::transform(dn.begin(),  dn.end(),  dn.begin(),  ::tolower);
            if (den.find(dn) != std::string::npos) return p;
        }
        return nullptr;
    }

    // Verifica stoc
    bool areStocSuficient(const std::string& idProdus, int cantitate) const {
        auto p = gasesteProdus(idProdus);
        return p && p->getCantitate() >= cantitate;
    }

    // Scade stoc si notifica daca devine critic
    void scadeStoc(const std::string& idProdus, int cantitate) {
        auto p = gasesteProdus(idProdus);
        if (!p) throw SpitalException("Produs " + idProdus + " inexistent in inventar");
        p->scadeStoc(cantitate);
        if (p->stocCritic(pragStocCritic)) {
            // Notificare stoc critic
            NotificationService::getInstance()->notifica(
                "STOC_CRITIC",
                "Stoc critic pentru " + p->getDenumire() +
                " (ramase: " + std::to_string(p->getCantitate()) + ")"
            );
        }
    }

    // Adauga stoc
    void adaugaStoc(const std::string& idProdus, int cantitate) {
        auto p = gasesteProdus(idProdus);
        if (!p) throw SpitalException("Produs " + idProdus + " inexistent in inventar");
        p->adaugaStoc(cantitate);
    }

    // Returneaza produse cu stoc critic
    std::vector<std::shared_ptr<T>> getStocCritic() const {
        std::vector<std::shared_ptr<T>> rezultat;
        for (const auto& p : produse)
            if (p->stocCritic(pragStocCritic)) rezultat.push_back(p);
        return rezultat;
    }

    // Returneaza produse expirate
    std::vector<std::shared_ptr<T>> getProdusExpirate() const {
        std::vector<std::shared_ptr<T>> rezultat;
        for (const auto& p : produse)
            if (p->esteExpirat()) rezultat.push_back(p);
        return rezultat;
    }

    // Verifica si notifica alerte
    void verificaAlerte() {
        for (const auto& p : produse) {
            if (p->stocCritic(pragStocCritic)) {
                NotificationService::getInstance()->notifica(
                    "STOC_CRITIC",
                    "Stoc critic: " + p->getDenumire() +
                    " (" + std::to_string(p->getCantitate()) + " unitati)"
                );
            }
            if (p->esteExpirat()) {
                NotificationService::getInstance()->notifica(
                    "PRODUS_EXPIRAT",
                    "Produs expirat: " + p->getDenumire() +
                    " (exp: " + p->getTermenExpirare() + ")"
                );
            }
        }
    }

    // Afisare completa inventar
    void afisare() const {
        if (produse.empty()) {
            std::cout << "  Inventar gol.\n";
            return;
        }
        for (const auto& p : produse)
            p->afisare();
    }

    // Afisare stoc critic
    void afisareStocCritic() const {
        auto critic = getStocCritic();
        if (critic.empty()) {
            std::cout << "  Nu exista produse cu stoc critic.\n";
            return;
        }
        std::cout << "  --- PRODUSE STOC CRITIC ---\n";
        for (const auto& p : critic)
            p->afisare();
    }

    // Afisare expirate
    void afisareExpirate() const {
        auto expirate = getProdusExpirate();
        if (expirate.empty()) {
            std::cout << "  Nu exista produse expirate.\n";
            return;
        }
        std::cout << "  --- PRODUSE EXPIRATE ---\n";
        for (const auto& p : expirate)
            p->afisare();
    }

    const std::vector<std::shared_ptr<T>>& getProduse() const { return produse; }
    int size() const { return (int)produse.size(); }

    // Salvare in fisier
    void salveaza() const {
        std::ofstream ofs(numeFisier);
        if (!ofs.is_open()) return;
        for (const auto& p : produse)
            ofs << p->serializa() << "\n";
    }

    // Incarcare din fisier - necesita factory extern
    // (implementat in HospitalManager)
};
