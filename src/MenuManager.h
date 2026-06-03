#pragma once

// ============================================================
//  MenuManager — Gestioneaza interfata utilizator
//  - start()    : interfata CLI clasica (stdin/stdout)
//  - startTUI() : interfata TUI moderna cu Notcurses
// ============================================================
class MenuManager {
private:
    // ---- Meniuri CLI (pastreaza compatibilitatea existenta) ----
    void meniuAdmin();
    void meniuMedic();
    void meniuAsistent();
    void meniuReceptie();

public:
    MenuManager() = default;

    // Interfata CLI clasica — functioneaza fara Notcurses
    void start();

    // Interfata TUI cu Notcurses — ecran Login + Dashboard
    // Necesita compilare cu -DUSE_TUI=ON in CMakeLists.txt
    void startTUI();
};
