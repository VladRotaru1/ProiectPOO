// ============================================================
//  TuiLogin.cpp — Implementare ecran Login modal cu ncurses
// ============================================================
#include "TuiLogin.h"
#include <cstring>
#include <algorithm>

// ============================================================
//  Constructor / Destructor
// ============================================================
TuiLoginScreen::TuiLoginScreen(std::shared_ptr<AuthService> auth)
    : auth_(std::move(auth)),
      win_(nullptr),
      winRows_(WIN_H), winCols_(WIN_W),
      focus_(LoginFocus::FIELD_USER)
{
    // Centram fereastra
    int termRows, termCols;
    getmaxyx(stdscr, termRows, termCols);

    winY_ = (termRows - WIN_H) / 2;
    winX_ = (termCols - WIN_W) / 2;
    if (winY_ < 0) winY_ = 0;
    if (winX_ < 0) winX_ = 0;

    win_ = newwin(WIN_H, WIN_W, winY_, winX_);
    keypad(win_, TRUE);
}

TuiLoginScreen::~TuiLoginScreen() {
    if (win_) {
        delwin(win_);
    }
}

// ============================================================
//  Randare fereastra modala
// ============================================================
void TuiLoginScreen::render() {
    // Curata ecranul principal
    clear();
    refresh();

    // Curata fereastra
    werase(win_);

    renderBox();
    renderTitle();

    // Campul Username (row 5 in fereastra)
    renderField(5, "  Username", inputUser_, false, focus_ == LoginFocus::FIELD_USER);
    
    // Campul Password (row 9 in fereastra)
    renderField(9, "  Password", inputPass_, true, focus_ == LoginFocus::FIELD_PASS);

    renderButtons();
    renderError();

    wrefresh(win_);
}

// ---- Conturul ferestrei ----
void TuiLoginScreen::renderBox() {
    wattron(win_, COLOR_PAIR(1));
    box(win_, 0, 0);
    
    // Linie separatoare sub titlu
    mvwhline(win_, 3, 1, ACS_HLINE, WIN_W - 2);
    mvwaddch(win_, 3, 0, ACS_LTEE);
    mvwaddch(win_, 3, WIN_W - 1, ACS_RTEE);
    
    wattroff(win_, COLOR_PAIR(1));
}

// ---- Titlul ferestrei ----
void TuiLoginScreen::renderTitle() {
    wattron(win_, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win_, 1, (WIN_W - 27) / 2, "SPITALUL MUNICIPAL V1.0");
    wattroff(win_, COLOR_PAIR(1) | A_BOLD);

    wattron(win_, COLOR_PAIR(2));
    mvwprintw(win_, 2, (WIN_W - 26) / 2, "Hospital Information System");
    wattroff(win_, COLOR_PAIR(2));
}

// ---- Camp de input (username sau password) ----
void TuiLoginScreen::renderField(int row, const std::string& label, const std::string& value, bool masked, bool active) {
    wattron(win_, COLOR_PAIR(2));
    mvwprintw(win_, row, 1, "%s", label.c_str());
    wattroff(win_, COLOR_PAIR(2));

    if (active) {
        wattron(win_, COLOR_PAIR(4));
    } else {
        wattron(win_, COLOR_PAIR(2) | A_REVERSE);
    }

    std::string display;
    if (masked) {
        display = std::string(value.size(), '*');
    } else {
        display = value;
    }

    // Trunc la latimea campului
    int maxShow = FIELD_W - 4;
    if ((int)display.size() > maxShow) {
        display = display.substr(display.size() - maxShow);
    }

    std::string showStr = " " + display;
    if (active) showStr += "_";
    
    // Pad to field width
    while ((int)showStr.size() < FIELD_W - 2) showStr += " ";

    mvwprintw(win_, row + 1, 2, "%s", showStr.c_str());

    if (active) {
        wattroff(win_, COLOR_PAIR(4));
    } else {
        wattroff(win_, COLOR_PAIR(2) | A_REVERSE);
    }
}

// ---- Butoanele Login si Exit ----
void TuiLoginScreen::renderButtons() {
    const int btnRow = 14;

    // ---- Buton LOGIN ----
    if (focus_ == LoginFocus::BTN_LOGIN) {
        wattron(win_, COLOR_PAIR(4) | A_BOLD);
    } else {
        wattron(win_, COLOR_PAIR(2) | A_REVERSE);
    }
    mvwprintw(win_, btnRow, 6, "   LOGIN   ");
    if (focus_ == LoginFocus::BTN_LOGIN) wattroff(win_, COLOR_PAIR(4) | A_BOLD);
    else wattroff(win_, COLOR_PAIR(2) | A_REVERSE);

    // ---- Buton EXIT ----
    if (focus_ == LoginFocus::BTN_EXIT) {
        wattron(win_, COLOR_PAIR(3) | A_REVERSE | A_BOLD);
    } else {
        wattron(win_, COLOR_PAIR(2) | A_REVERSE);
    }
    mvwprintw(win_, btnRow, 32, "   EXIT    ");
    if (focus_ == LoginFocus::BTN_EXIT) wattroff(win_, COLOR_PAIR(3) | A_REVERSE | A_BOLD);
    else wattroff(win_, COLOR_PAIR(2) | A_REVERSE);

    // Hint navigare (row 16)
    wattron(win_, COLOR_PAIR(2) | A_DIM);
    mvwprintw(win_, 16, 2, " Tab/Arrows[UP/DOWN]: navigare  Enter: confirma ");
    wattroff(win_, COLOR_PAIR(2) | A_DIM);
}

// ---- Mesaj de eroare ----
void TuiLoginScreen::renderError() {
    if (!errMsg_.empty()) {
        wattron(win_, COLOR_PAIR(3) | A_BOLD);
        std::string msg = " " + errMsg_ + " ";
        int col = std::max(1, (WIN_W - (int)msg.size()) / 2);
        mvwprintw(win_, 12, col, "%s", msg.c_str());
        wattroff(win_, COLOR_PAIR(3) | A_BOLD);
    }
}

// ============================================================
//  Bucla principala de evenimente
// ============================================================
LoginResult TuiLoginScreen::run() {
    LoginResult result{false, false, ""};

    while (true) {
        render();

        int ch = wgetch(win_);
        errMsg_.clear(); // Sterge eroarea la orice apasare de tasta

        bool done = handleKey(ch, result);
        if (done) break;
    }
    return result;
}

// ============================================================
//  Gestionare taste
// ============================================================
bool TuiLoginScreen::handleKey(int ch, LoginResult& result) {
    // ESC
    if (ch == 27) {
        result.exitApp = true;
        return true;
    }

    // Tab (sau 9)
    if (ch == '\t' || ch == 9) {
        switch (focus_) {
            case LoginFocus::FIELD_USER: focus_ = LoginFocus::FIELD_PASS; break;
            case LoginFocus::FIELD_PASS: focus_ = LoginFocus::BTN_LOGIN;  break;
            case LoginFocus::BTN_LOGIN:  focus_ = LoginFocus::BTN_EXIT;   break;
            case LoginFocus::BTN_EXIT:   focus_ = LoginFocus::FIELD_USER; break;
        }
        return false;
    }

    // Navigare cu sageti
    if (ch == KEY_UP) {
        switch (focus_) {
            case LoginFocus::FIELD_PASS: focus_ = LoginFocus::FIELD_USER; break;
            case LoginFocus::BTN_LOGIN:
            case LoginFocus::BTN_EXIT:   focus_ = LoginFocus::FIELD_PASS; break;
            default: break;
        }
        return false;
    }
    if (ch == KEY_DOWN) {
        switch (focus_) {
            case LoginFocus::FIELD_USER: focus_ = LoginFocus::FIELD_PASS; break;
            case LoginFocus::FIELD_PASS: focus_ = LoginFocus::BTN_LOGIN;  break;
            default: break;
        }
        return false;
    }
    if (ch == KEY_LEFT && focus_ == LoginFocus::BTN_EXIT) {
        focus_ = LoginFocus::BTN_LOGIN;
        return false;
    }
    if (ch == KEY_RIGHT && focus_ == LoginFocus::BTN_LOGIN) {
        focus_ = LoginFocus::BTN_EXIT;
        return false;
    }

    // Enter
    if (ch == '\n' || ch == KEY_ENTER) {
        switch (focus_) {
            case LoginFocus::BTN_LOGIN:
            case LoginFocus::FIELD_USER:
            case LoginFocus::FIELD_PASS:
                return tryLogin(result);
            case LoginFocus::BTN_EXIT:
                result.exitApp = true;
                return true;
        }
    }

    // Backspace
    if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
        if (focus_ == LoginFocus::FIELD_USER && !inputUser_.empty())
            inputUser_.pop_back();
        else if (focus_ == LoginFocus::FIELD_PASS && !inputPass_.empty())
            inputPass_.pop_back();
        return false;
    }

    // Caractere printabile
    if (ch >= 32 && ch <= 126) {
        char c = (char)ch;
        if (focus_ == LoginFocus::FIELD_USER && inputUser_.size() < 32)
            inputUser_ += c;
        else if (focus_ == LoginFocus::FIELD_PASS && inputPass_.size() < 32)
            inputPass_ += c;
    }

    return false;
}

// ============================================================
//  Incercare autentificare
// ============================================================
bool TuiLoginScreen::tryLogin(LoginResult& result) {
    if (inputUser_.empty()) {
        errMsg_ = "Introduceti username-ul!";
        return false;
    }
    if (inputPass_.empty()) {
        errMsg_ = "Introduceti parola!";
        return false;
    }

    if (auth_->login(inputUser_, inputPass_)) {
        result.success  = true;
        result.username = inputUser_;
        result.exitApp  = false;
        return true;
    } else {
        errMsg_ = "Username sau parola incorecte!";
        inputPass_.clear();
        focus_ = LoginFocus::FIELD_PASS;
        return false;
    }
}
