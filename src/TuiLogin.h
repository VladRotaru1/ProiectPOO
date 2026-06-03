#pragma once
// ============================================================
//  TuiLogin.h — Ecran de Login modal cu ncurses
// ============================================================
#include <ncurses.h>
#include <string>
#include <memory>
#include "AuthService.h"

// ---- Rezultatul sesiunii de login ----------------------------
struct LoginResult {
    bool exitApp;       // true = userul a apasat "Exit" => inchide aplicatia
    bool success;       // true = autentificare reusita
    std::string username;
};

// ---- Starea focusului in fereastra de login ------------------
enum class LoginFocus {
    FIELD_USER,     // campul Username activ
    FIELD_PASS,     // campul Password activ
    BTN_LOGIN,      // butonul Login selectat
    BTN_EXIT        // butonul Exit selectat
};

// ============================================================
//  TuiLoginScreen
//  Randeaza o fereastra modala centrata, captureaza input de
//  la tastatura si returneaza un LoginResult.
// ============================================================
class TuiLoginScreen {
public:
    explicit TuiLoginScreen(std::shared_ptr<AuthService> auth);
    ~TuiLoginScreen();

    // Blocheaza pana la login reusit sau Exit.
    LoginResult run();

private:
    // ---- Referinte externe ----
    std::shared_ptr<AuthService> auth_;

    // ---- Planuri (Ferestre) ncurses ----
    WINDOW* win_;       // fereastra modala

    // ---- Dimensiuni fereastra ----
    int winRows_, winCols_;     // dimensiunile ferestrei modale
    int winY_, winX_;           // pozitia ferestrei

    // ---- Stare UI ----
    std::string  inputUser_;
    std::string  inputPass_;
    LoginFocus   focus_;
    std::string  errMsg_;

    // ---- Constante layout ----
    static constexpr int WIN_H       = 18;
    static constexpr int WIN_W       = 52;
    static constexpr int FIELD_W     = 36;

    // ---- Metode private ----
    void render();
    void renderBox();
    void renderTitle();
    void renderField(int row, const std::string& label, const std::string& value, bool masked, bool active);
    void renderButtons();
    void renderError();

    // Procesare tastatura
    bool handleKey(int ch, LoginResult& result);
    bool tryLogin(LoginResult& result);
};
