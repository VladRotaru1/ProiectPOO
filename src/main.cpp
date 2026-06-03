// ============================================================
//  main.cpp — Punct de intrare SpitalHIS
//  Compila cu TUI:  cmake -DUSE_TUI=ON ..
//  Compila fara TUI: cmake -DUSE_TUI=OFF ..  (sau implicit)
// ============================================================
#include "MenuManager.h"

#ifdef USE_TUI
#include <locale.h>
#endif

int main() {
    MenuManager menu;

#ifdef USE_TUI
    // Interfata TUI moderna cu Notcurses (Login + Dashboard)
    menu.startTUI();
#else
    // Interfata CLI clasica (stdin/stdout)
    menu.start();
#endif

    return 0;
}