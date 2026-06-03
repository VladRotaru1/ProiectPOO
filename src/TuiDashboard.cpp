#include "TuiDashboard.h"
#include "AuditLog.h"
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>

// ============================================================
//  Constructor / Destructor
// ============================================================
TuiAdminDashboard::TuiAdminDashboard(HospitalManager* hm, AuthService* auth)
    : hm_(hm), auth_(auth),
      rol_(Rol::ADMIN),
      headerWin_(nullptr), sidebarWin_(nullptr),
      workspaceWin_(nullptr), footerWin_(nullptr),
      sidebarCursor_(0),
      sidebarFocused_(true),
      exitDash_(false),
      wsMode_(WsMode::SUB_MENU),
      wsActionCursor_(0),
      wsScroll_(0),
      wsDataTotalLines_(0),
      messageColor_(0),
      inputCursor_(0),
      pendingActionId_(0),
      lastTimeRefresh_(std::chrono::steady_clock::now())
{
    // Determina rolul din utilizatorul autentificat
    User* u = auth_->getUtilizatorCurent();
    if (u) rol_ = u->getRol();

    getmaxyx(stdscr, termRows_, termCols_);
    initMenu();
    initPlanes();

    // Seteaza sectiunea activa pe prima din meniu
    if (!menuItems_.empty()) {
        activeSection_ = menuItems_[0].section;
    }
    loadActionsForSection(activeSection_);
}

TuiAdminDashboard::~TuiAdminDashboard() {
    destroyPlanes();
}

// ============================================================
//  Sidebar menu items — personalizat per ROL
// ============================================================
void TuiAdminDashboard::initMenu() {
    menuItems_.clear();

    switch (rol_) {
        case Rol::ADMIN:
            menuItems_ = {
                {DashSection::ADMIN_CONTURI,       " U ", "Gestionare conturi"},
                {DashSection::ADMIN_ROLURI,        " R ", "Roluri & permisiuni"},
                {DashSection::ADMIN_NOMENCLATOARE,  " N ", "Config. nomenclatoare"},
                {DashSection::ADMIN_RAPOARTE,      " D ", "Rapoarte & statistici"},
                {DashSection::ADMIN_AUDIT,         " A ", "Audit & jurnale"},
                {DashSection::ADMIN_PATURI,        " P ", "Paturi & saloane"},
                {DashSection::IESIRE,              " X ", "Logout / Iesire"}
            };
            break;

        case Rol::MEDIC:
            menuItems_ = {
                {DashSection::MEDIC_CONSULTATII,   " C ", "Consultatii & diag."},
                {DashSection::MEDIC_FOAIE_OBS,     " F ", "Foaie de observatie"},
                {DashSection::MEDIC_PRESCRIPTII,   " Rx", "Prescriptii medicale"},
                {DashSection::MEDIC_INVESTIGATII,  " I ", "Solicitare invest."},
                {DashSection::MEDIC_INTERNARE,     " H ", "Internare & externare"},
                {DashSection::MEDIC_CNAS,          " $ ", "Raportare CNAS"},
                {DashSection::MEDIC_TRIMITERI,     " T ", "Trimiteri inter-sectii"},
                {DashSection::MEDIC_REZULTATE,     " R ", "Vizualizare rezultate"},
                {DashSection::MEDIC_INDICATII,     " M ", "Indicatii tratament"},
                {DashSection::IESIRE,              " X ", "Logout / Iesire"}
            };
            break;

        case Rol::ASISTENT:
            menuItems_ = {
                {DashSection::ASIST_TRATAMENT,     " T ", "Administrare tratam."},
                {DashSection::ASIST_SEMNE_VITALE,  " V ", "Semne vitale"},
                {DashSection::ASIST_RECOLTARE,     " R ", "Recoltare probe"},
                {DashSection::ASIST_FISA_NURSING,  " N ", "Fisa nursing"},
                {DashSection::ASIST_INCIDENTE,     " ! ", "Raportare incidente"},
                {DashSection::ASIST_PREGATIRE,     " P ", "Pregatire pacient"},
                {DashSection::IESIRE,              " X ", "Logout / Iesire"}
            };
            break;

        case Rol::RECEPTIE:
            menuItems_ = {
                {DashSection::RECEP_INREGISTRARE,  " + ", "Inregistrare pacienti"},
                {DashSection::RECEP_ASIGURARE,     " A ", "Verificare asigurare"},
                {DashSection::RECEP_PROGRAMARI,    " C ", "Programari"},
                {DashSection::RECEP_DIRECTIONARE,  " D ", "Direction. admin."},
                {DashSection::RECEP_DOCUMENTE,     " E ", "Emitere documente"},
                {DashSection::RECEP_LISTE_ASTEPTARE," L ", "Liste de asteptare"},
                {DashSection::RECEP_INFORMARE,     " i ", "Informare pacienti"},
                {DashSection::RECEP_SERVICII,      " S ", "Servicii facturabile"},
                {DashSection::RECEP_FACTURA,       " F ", "Factura & chitanta"},
                {DashSection::IESIRE,              " X ", "Logout / Iesire"}
            };
            break;
    }
}

// ============================================================
//  Load sub-menu actions for each section
// ============================================================
void TuiAdminDashboard::loadActionsForSection(DashSection sec) {
    currentActions_.clear();
    wsActionCursor_ = 0;
    wsScroll_ = 0;
    wsMode_ = WsMode::SUB_MENU;

    switch (sec) {
        // ============================================================
        //  ADMIN sections
        // ============================================================
        case DashSection::ADMIN_CONTURI:
            currentActions_ = {
                {"Afisare utilizatori",              1000},
                {"Creare utilizator nou",            1001},
                {"Editare utilizator",               1002},
                {"Dezactivare utilizator",           1003},
                {"Resetare parola utilizator",       1004}
            };
            break;

        case DashSection::ADMIN_ROLURI:
            currentActions_ = {
                {"Afisare roluri & permisiuni",      1010}
            };
            break;

        case DashSection::ADMIN_NOMENCLATOARE:
            currentActions_ = {
                {"Afisare sectii",                   1020},
                {"Afisare coduri diagnostic ICD-10", 1021},
                {"Afisare proceduri medicale",       1022}
            };
            break;

        case DashSection::ADMIN_RAPOARTE:
            currentActions_ = {
                {"Dashboard statistic",              1030},
                {"Raport financiar",                 1031},
                {"Top medici",                       1032},
                {"Calcul salarii",                   1033},
                {"Gestiune inventar complet",        1034},
                {"Alerte stoc critic",               1035},
                {"Produse expirate",                 1036}
            };
            break;

        case DashSection::ADMIN_AUDIT:
            currentActions_ = {
                {"Audit log (ultimele 50)",          1040},
                {"Backup automat (Salvare date)",    1041}
            };
            break;

        case DashSection::ADMIN_PATURI:
            currentActions_ = {
                {"Vizualizare sectii & paturi",      1050},
                {"Vizualizare ocupare saloane",       1051}
            };
            break;

        // ============================================================
        //  MEDIC sections
        // ============================================================
        case DashSection::MEDIC_CONSULTATII:
            currentActions_ = {
                {"Consultatie noua",                  2000},
                {"Adaugare diagnostic (ICD-10)",      2001},
                {"Vizualizare pacienti",              2002}
            };
            break;

        case DashSection::MEDIC_FOAIE_OBS:
            currentActions_ = {
                {"Completare foaie de observatie",    2010},
                {"Semnare digitala foaie",            2011}
            };
            break;

        case DashSection::MEDIC_PRESCRIPTII:
            currentActions_ = {
                {"Prescriere tratament (DCI)",        2020},
                {"Emitere reteta compensata",         2021}
            };
            break;

        case DashSection::MEDIC_INVESTIGATII:
            currentActions_ = {
                {"Trimitere analize laborator",       2030},
                {"Trimitere imagistica",              2031},
                {"Consult interdisciplinar",          2032}
            };
            break;

        case DashSection::MEDIC_INTERNARE:
            currentActions_ = {
                {"Internare pacient",                 2040},
                {"Externare pacient",                 2041},
                {"Internari active",                  2042},
                {"Generare scrisoare medicala",       2043}
            };
            break;

        case DashSection::MEDIC_CNAS:
            currentActions_ = {
                {"Validare cazuri DRG",               2050},
                {"Raport decontare CNAS",             2051}
            };
            break;

        case DashSection::MEDIC_TRIMITERI:
            currentActions_ = {
                {"Trimitere consult specialitate",    2060},
                {"Vizualizare trimiteri emise",       2061}
            };
            break;

        case DashSection::MEDIC_REZULTATE:
            currentActions_ = {
                {"Rezultate analize pacient",         2070},
                {"Vizualizare imagistica (DICOM)",    2071}
            };
            break;

        case DashSection::MEDIC_INDICATII:
            currentActions_ = {
                {"Transmitere indicatii asistent",    2080},
                {"Vizualizare indicatii transmise",   2081}
            };
            break;

        // ============================================================
        //  ASISTENT sections
        // ============================================================
        case DashSection::ASIST_TRATAMENT:
            currentActions_ = {
                {"Vizualizare indicatii primite",     3000},
                {"Confirmare administrare tratament", 3001}
            };
            break;

        case DashSection::ASIST_SEMNE_VITALE:
            currentActions_ = {
                {"Inregistrare parametri vitali",     3010},
                {"Vizualizare istoric parametri",     3011}
            };
            break;

        case DashSection::ASIST_RECOLTARE:
            currentActions_ = {
                {"Inregistrare proba biologica",      3020},
                {"Vizualizare probe recoltate",       3021}
            };
            break;

        case DashSection::ASIST_FISA_NURSING:
            currentActions_ = {
                {"Completare ingrijiri zilnice",       3030},
                {"Vizualizare observatii",            3031}
            };
            break;

        case DashSection::ASIST_INCIDENTE:
            currentActions_ = {
                {"Raportare eveniment advers",        3040},
                {"Alerte active",                     3041}
            };
            break;

        case DashSection::ASIST_PREGATIRE:
            currentActions_ = {
                {"Checklist pregatire interventie",   3050},
                {"Checklist pregatire investigatie",  3051}
            };
            break;

        // ============================================================
        //  RECEPTIE sections
        // ============================================================
        case DashSection::RECEP_INREGISTRARE:
            currentActions_ = {
                {"Adaugare pacient nou",              4000},
                {"Cautare pacient (ID/CNP)",          4001}
            };
            break;

        case DashSection::RECEP_ASIGURARE:
            currentActions_ = {
                {"Verificare asigurare CNAS",         4010},
                {"Verificare bilet de trimitere",     4011}
            };
            break;

        case DashSection::RECEP_PROGRAMARI:
            currentActions_ = {
                {"Creare programare",                 4020},
                {"Afisare programari (calendar)",     4021},
                {"Anulare programare",                4022}
            };
            break;

        case DashSection::RECEP_DIRECTIONARE:
            currentActions_ = {
                {"Alocare medic & sectie",            4030},
                {"Vizualizare medici disponibili",    4031}
            };
            break;

        case DashSection::RECEP_DOCUMENTE:
            currentActions_ = {
                {"Generare bilet de externare",       4040},
                {"Generare adeverinta medicala",      4041}
            };
            break;

        case DashSection::RECEP_LISTE_ASTEPTARE:
            currentActions_ = {
                {"Afisare lista de asteptare",        4050},
                {"Prioritizare pacient",              4051}
            };
            break;

        case DashSection::RECEP_INFORMARE:
            currentActions_ = {
                {"Program sectii",                    4060},
                {"Localizare sectii & cabinete",      4061}
            };
            break;

        case DashSection::RECEP_SERVICII:
            currentActions_ = {
                {"Inregistrare serviciu neasigurat",  4070},
                {"Inregistrare coplata",              4071},
                {"Servicii la cerere",                4072}
            };
            break;

        case DashSection::RECEP_FACTURA:
            currentActions_ = {
                {"Emitere factura",                   4080},
                {"Afisare facturi neplatite",         4081},
                {"Emitere chitanta",                  4082}
            };
            break;

        default:
            break;
    }
}

// ============================================================
//  Window creation / destruction
// ============================================================
void TuiAdminDashboard::initPlanes() {
    int workspaceH = termRows_ - HEADER_H - FOOTER_H;
    int workspaceW = termCols_ - SIDEBAR_W;
    if (workspaceH < 1) workspaceH = 1;
    if (workspaceW < 1) workspaceW = 1;

    headerWin_    = newwin(HEADER_H, termCols_, 0, 0);
    sidebarWin_   = newwin(workspaceH, SIDEBAR_W, HEADER_H, 0);
    workspaceWin_ = newwin(workspaceH, workspaceW, HEADER_H, SIDEBAR_W);
    footerWin_    = newwin(FOOTER_H, termCols_, termRows_ - FOOTER_H, 0);

    keypad(sidebarWin_, TRUE);
    keypad(workspaceWin_, TRUE);
}

void TuiAdminDashboard::destroyPlanes() {
    if (headerWin_)    delwin(headerWin_);
    if (sidebarWin_)   delwin(sidebarWin_);
    if (workspaceWin_) delwin(workspaceWin_);
    if (footerWin_)    delwin(footerWin_);
    headerWin_ = sidebarWin_ = workspaceWin_ = footerWin_ = nullptr;
}

// ============================================================
//  Main loop
// ============================================================
void TuiAdminDashboard::run() {
    while (!exitDash_) {
        // Resize handling
        int newR, newC;
        getmaxyx(stdscr, newR, newC);
        if (newR != termRows_ || newC != termCols_) {
            termRows_ = newR;
            termCols_ = newC;
            destroyPlanes();
            initPlanes();
            clear();
            refresh();
        }

        renderAll();

        WINDOW* inputWin = sidebarFocused_ ? sidebarWin_ : workspaceWin_;
        wtimeout(inputWin, 500);
        int ch = wgetch(inputWin);

        if (ch != ERR) {
            handleKey(ch);
        }
    }
}

// ============================================================
//  Rendering
// ============================================================
void TuiAdminDashboard::renderAll() {
    renderHeader();
    renderSidebar();
    renderWorkspace();
    renderFooter();
}

std::string TuiAdminDashboard::getCurrentTime() const {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

int TuiAdminDashboard::wsVisibleLines() const {
    int h = termRows_ - HEADER_H - FOOTER_H;
    return h > 4 ? h - 4 : 1;  // leave room for title + border
}

// ============================================================
//  Section title helper
// ============================================================
std::string TuiAdminDashboard::getSectionTitle(DashSection sec) const {
    switch (sec) {
        // ADMIN
        case DashSection::ADMIN_CONTURI:       return "GESTIONARE CONTURI";
        case DashSection::ADMIN_ROLURI:        return "ROLURI & PERMISIUNI";
        case DashSection::ADMIN_NOMENCLATOARE:  return "CONFIGURARE NOMENCLATOARE";
        case DashSection::ADMIN_RAPOARTE:      return "RAPOARTE & STATISTICI";
        case DashSection::ADMIN_AUDIT:         return "AUDIT & JURNALE";
        case DashSection::ADMIN_PATURI:        return "GESTIONARE PATURI & SALOANE";
        // MEDIC
        case DashSection::MEDIC_CONSULTATII:   return "CONSULTATII & DIAGNOSTIC";
        case DashSection::MEDIC_FOAIE_OBS:     return "FOAIE DE OBSERVATIE";
        case DashSection::MEDIC_PRESCRIPTII:   return "PRESCRIPTII MEDICALE";
        case DashSection::MEDIC_INVESTIGATII:  return "SOLICITARE INVESTIGATII";
        case DashSection::MEDIC_INTERNARE:     return "INTERNARE & EXTERNARE";
        case DashSection::MEDIC_CNAS:          return "RAPORTARE CNAS";
        case DashSection::MEDIC_TRIMITERI:     return "TRIMITERI INTER-SECTII";
        case DashSection::MEDIC_REZULTATE:     return "VIZUALIZARE REZULTATE";
        case DashSection::MEDIC_INDICATII:     return "INDICATII DE TRATAMENT";
        // ASISTENT
        case DashSection::ASIST_TRATAMENT:     return "ADMINISTRARE TRATAMENT";
        case DashSection::ASIST_SEMNE_VITALE:  return "MONITORIZARE SEMNE VITALE";
        case DashSection::ASIST_RECOLTARE:     return "RECOLTARE PROBE BIOLOGICE";
        case DashSection::ASIST_FISA_NURSING:  return "FISA NURSING";
        case DashSection::ASIST_INCIDENTE:     return "RAPORTARE INCIDENTE";
        case DashSection::ASIST_PREGATIRE:     return "PREGATIRE PACIENT";
        // RECEPTIE
        case DashSection::RECEP_INREGISTRARE:  return "INREGISTRARE PACIENTI";
        case DashSection::RECEP_ASIGURARE:     return "VERIFICARE ASIGURARE";
        case DashSection::RECEP_PROGRAMARI:    return "PROGRAMARI";
        case DashSection::RECEP_DIRECTIONARE:  return "DIRECTIONARE ADMINISTRATIVA";
        case DashSection::RECEP_DOCUMENTE:     return "EMITERE DOCUMENTE";
        case DashSection::RECEP_LISTE_ASTEPTARE: return "LISTE DE ASTEPTARE";
        case DashSection::RECEP_INFORMARE:     return "INFORMARE PACIENTI";
        case DashSection::RECEP_SERVICII:      return "SERVICII FACTURABILE";
        case DashSection::RECEP_FACTURA:       return "FACTURA & CHITANTA";
        default: return "SELECTATI ACTIUNEA";
    }
}

void TuiAdminDashboard::renderHeader() {
    wbkgd(headerWin_, COLOR_PAIR(4));
    werase(headerWin_);

    wattron(headerWin_, A_BOLD);
    // Titlu personalizat per rol
    switch (rol_) {
        case Rol::ADMIN:
            mvwprintw(headerWin_, 1, 2, " SPITALUL MUNICIPAL - ADMINISTRARE ");
            break;
        case Rol::MEDIC:
            mvwprintw(headerWin_, 1, 2, " SPITALUL MUNICIPAL - CABINET MEDICAL ");
            break;
        case Rol::ASISTENT:
            mvwprintw(headerWin_, 1, 2, " SPITALUL MUNICIPAL - POST NURSING ");
            break;
        case Rol::RECEPTIE:
            mvwprintw(headerWin_, 1, 2, " SPITALUL MUNICIPAL - RECEPTIE ");
            break;
    }
    wattroff(headerWin_, A_BOLD);

    // Show current user role
    User* u = auth_->getUtilizatorCurent();
    if (u) {
        std::string roleStr = rolToString(u->getRol());
        int infoPos = 42;
        if (infoPos < termCols_ - 30) {
            mvwprintw(headerWin_, 1, infoPos, "[%s: %s]", roleStr.c_str(), u->getUsername().c_str());
        }
    }

    std::string timeStr = getCurrentTime();
    int timePos = termCols_ - (int)timeStr.size() - 2;
    if (timePos > 0) {
        mvwprintw(headerWin_, 1, timePos, "%s", timeStr.c_str());
    }

    wrefresh(headerWin_);
}

void TuiAdminDashboard::renderSidebar() {
    wbkgd(sidebarWin_, COLOR_PAIR(1));
    werase(sidebarWin_);
    box(sidebarWin_, 0, 0);

    // Titlu sidebar per rol
    wattron(sidebarWin_, A_BOLD);
    switch (rol_) {
        case Rol::ADMIN:    mvwprintw(sidebarWin_, 1, 2, "MENIU ADMINISTRATOR"); break;
        case Rol::MEDIC:    mvwprintw(sidebarWin_, 1, 2, "MENIU MEDIC");         break;
        case Rol::ASISTENT: mvwprintw(sidebarWin_, 1, 2, "MENIU ASISTENT");      break;
        case Rol::RECEPTIE: mvwprintw(sidebarWin_, 1, 2, "MENIU RECEPTIE");      break;
    }
    wattroff(sidebarWin_, A_BOLD);
    mvwhline(sidebarWin_, 2, 1, ACS_HLINE, SIDEBAR_W - 2);

    int startY = 3;
    int sidebarH = termRows_ - HEADER_H - FOOTER_H;

    for (size_t i = 0; i < menuItems_.size(); ++i) {
        int row = startY + (int)i * 2;
        if (row >= sidebarH - 1) break;  // Nu depasi fereastra

        bool isSelected = ((int)i == sidebarCursor_);

        if (isSelected && sidebarFocused_) {
            wattron(sidebarWin_, COLOR_PAIR(4) | A_BOLD);
        } else if (isSelected && !sidebarFocused_) {
            wattron(sidebarWin_, COLOR_PAIR(2) | A_BOLD);
        } else {
            wattron(sidebarWin_, COLOR_PAIR(1));
        }

        std::string line = menuItems_[i].icon + " " + menuItems_[i].label;
        // Pad to sidebar width
        while ((int)line.size() < SIDEBAR_W - 4) line += " ";

        mvwprintw(sidebarWin_, row, 1, " %s", line.c_str());

        if (isSelected && sidebarFocused_) {
            wattroff(sidebarWin_, COLOR_PAIR(4) | A_BOLD);
        } else if (isSelected && !sidebarFocused_) {
            wattroff(sidebarWin_, COLOR_PAIR(2) | A_BOLD);
        } else {
            wattroff(sidebarWin_, COLOR_PAIR(1));
        }
    }

    wrefresh(sidebarWin_);
}

void TuiAdminDashboard::clearWorkspace() {
    wbkgd(workspaceWin_, COLOR_PAIR(2));
    werase(workspaceWin_);
    box(workspaceWin_, 0, 0);
}

void TuiAdminDashboard::renderWorkspace() {
    clearWorkspace();

    switch (wsMode_) {
        case WsMode::SUB_MENU:   renderWsSubMenu();   break;
        case WsMode::DATA_VIEW:  renderWsDataView();  break;
        case WsMode::INPUT_FORM: renderWsInputForm();  break;
        case WsMode::MESSAGE:    renderWsMessage();    break;
    }

    wrefresh(workspaceWin_);
}

void TuiAdminDashboard::renderFooter() {
    wbkgd(footerWin_, COLOR_PAIR(4));
    werase(footerWin_);

    User* u = auth_->getUtilizatorCurent();
    std::string userStr = u ? u->getUsername() : "?";

    mvwprintw(footerWin_, 0, 2, " User: %s ", userStr.c_str());

    // Context-sensitive help
    std::string help;
    if (wsMode_ == WsMode::SUB_MENU) {
        help = "[Tab] Focus  [Enter] Selecteaza  [Q] Iesire";
    } else if (wsMode_ == WsMode::DATA_VIEW) {
        help = "[Esc] Inapoi  [Up/Down] Scroll  [Q] Iesire";
    } else if (wsMode_ == WsMode::INPUT_FORM) {
        help = "[Tab/Down] Camp urmator  [Enter] Trimite  [Esc] Anuleaza";
    } else {
        help = "[Esc/Enter] Inapoi";
    }

    int helpPos = termCols_ - (int)help.size() - 2;
    if (helpPos > 20) {
        mvwprintw(footerWin_, 0, helpPos, "%s", help.c_str());
    }

    wrefresh(footerWin_);
}

// ============================================================
//  Workspace: Sub-Menu Rendering
// ============================================================
void TuiAdminDashboard::renderWsSubMenu() {
    // Title from section
    wattron(workspaceWin_, COLOR_PAIR(1) | A_BOLD);
    std::string title = getSectionTitle(activeSection_) + " - Selectati actiunea";
    mvwprintw(workspaceWin_, 1, 2, "%s", title.c_str());
    wattroff(workspaceWin_, COLOR_PAIR(1) | A_BOLD);
    mvwhline(workspaceWin_, 2, 1, ACS_HLINE, termCols_ - SIDEBAR_W - 2);

    int row = 3;
    for (size_t i = 0; i < currentActions_.size(); ++i) {
        bool selected = ((int)i == wsActionCursor_) && !sidebarFocused_;

        if (selected) {
            wattron(workspaceWin_, COLOR_PAIR(4) | A_BOLD);
            mvwprintw(workspaceWin_, row, 2, " > %s", currentActions_[i].label.c_str());
            wattroff(workspaceWin_, COLOR_PAIR(4) | A_BOLD);
        } else {
            mvwprintw(workspaceWin_, row, 2, "   %s", currentActions_[i].label.c_str());
        }
        row += 2;
    }
}

// ============================================================
//  Workspace: Data View Rendering (scrollable)
// ============================================================
void TuiAdminDashboard::renderWsDataView() {
    if (dataLines_.empty()) {
        mvwprintw(workspaceWin_, 2, 2, "Nu exista date de afisat.");
        mvwprintw(workspaceWin_, 4, 2, "[Esc] Inapoi");
        return;
    }

    int visible = wsVisibleLines();
    wsDataTotalLines_ = (int)dataLines_.size();

    // Clamp scroll
    if (wsScroll_ > wsDataTotalLines_ - visible) {
        wsScroll_ = std::max(0, wsDataTotalLines_ - visible);
    }
    if (wsScroll_ < 0) wsScroll_ = 0;

    int wsWidth = termCols_ - SIDEBAR_W - 4;

    for (int i = 0; i < visible && (wsScroll_ + i) < wsDataTotalLines_; ++i) {
        const DataLine& dl = dataLines_[wsScroll_ + i];

        if (dl.colorPair > 0) wattron(workspaceWin_, COLOR_PAIR(dl.colorPair));
        if (dl.bold) wattron(workspaceWin_, A_BOLD);

        // Truncate line if too long
        std::string text = dl.text;
        if ((int)text.size() > wsWidth) text = text.substr(0, wsWidth);

        mvwprintw(workspaceWin_, 1 + i, 2, "%s", text.c_str());

        if (dl.bold) wattroff(workspaceWin_, A_BOLD);
        if (dl.colorPair > 0) wattroff(workspaceWin_, COLOR_PAIR(dl.colorPair));
    }

    // Scroll indicator
    if (wsDataTotalLines_ > visible) {
        int pctTop = (wsScroll_ * 100) / std::max(1, wsDataTotalLines_ - visible);
        mvwprintw(workspaceWin_, termRows_ - HEADER_H - FOOTER_H - 2, 2,
                  "[Scroll: %d%% | Linii: %d/%d]", pctTop, wsScroll_ + 1, wsDataTotalLines_);
    }
}

// ============================================================
//  Workspace: Input Form Rendering
// ============================================================
void TuiAdminDashboard::renderWsInputForm() {
    wattron(workspaceWin_, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(workspaceWin_, 1, 2, "COMPLETATI FORMULARUL:");
    wattroff(workspaceWin_, COLOR_PAIR(1) | A_BOLD);
    mvwhline(workspaceWin_, 2, 1, ACS_HLINE, termCols_ - SIDEBAR_W - 2);

    int row = 4;
    for (size_t i = 0; i < inputFields_.size(); ++i) {
        bool active = ((int)i == inputCursor_);

        mvwprintw(workspaceWin_, row, 3, "%s:", inputFields_[i].label.c_str());

        if (active) {
            wattron(workspaceWin_, COLOR_PAIR(4) | A_UNDERLINE);
        }

        // Draw input box
        mvwprintw(workspaceWin_, row + 1, 3, "[");
        int boxW = 30;
        std::string val = inputFields_[i].value;
        if ((int)val.size() > boxW) val = val.substr(val.size() - boxW);
        wprintw(workspaceWin_, "%-*s", boxW, val.c_str());
        wprintw(workspaceWin_, "]");

        if (active) {
            wattroff(workspaceWin_, COLOR_PAIR(4) | A_UNDERLINE);
        }

        row += 3;
    }

    // Submit button
    row += 1;
    bool onSubmit = (inputCursor_ == (int)inputFields_.size());
    if (onSubmit) wattron(workspaceWin_, COLOR_PAIR(5) | A_BOLD | A_REVERSE);
    mvwprintw(workspaceWin_, row, 5, "  [ TRIMITE ]  ");
    if (onSubmit) wattroff(workspaceWin_, COLOR_PAIR(5) | A_BOLD | A_REVERSE);

    // Cancel hint
    mvwprintw(workspaceWin_, row + 2, 5, "[Esc] Anuleaza");
}

// ============================================================
//  Workspace: Message Rendering
// ============================================================
void TuiAdminDashboard::renderWsMessage() {
    if (messageColor_ > 0) wattron(workspaceWin_, COLOR_PAIR(messageColor_) | A_BOLD);
    mvwprintw(workspaceWin_, 3, 4, "%s", messageText_.c_str());
    if (messageColor_ > 0) wattroff(workspaceWin_, COLOR_PAIR(messageColor_) | A_BOLD);

    mvwprintw(workspaceWin_, 6, 4, "[Esc] sau [Enter] -> Inapoi la meniu");
}

// ============================================================
//  Helper: show message
// ============================================================
void TuiAdminDashboard::showMessage(const std::string& msg, int color) {
    messageText_ = msg;
    messageColor_ = color;
    wsMode_ = WsMode::MESSAGE;
}

// ============================================================
//  Helper: start input form
// ============================================================
void TuiAdminDashboard::startInputForm(int actionId, const std::vector<InputField>& fields) {
    inputFields_ = fields;
    inputCursor_ = 0;
    pendingActionId_ = actionId;
    wsMode_ = WsMode::INPUT_FORM;
}

// ============================================================
//  Execute action based on actionId
// ============================================================
void TuiAdminDashboard::executeAction(int actionId) {
    switch (actionId) {
        // ============================================================
        //  ADMIN actions (1000-1099)
        // ============================================================
        case 1000: // Afisare utilizatori
            loadUtilizatoriData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1001: // Creare utilizator nou
            startInputForm(1001, {
                {"Username", "", 20},
                {"Parola", "", 20},
                {"Rol (0=Admin,1=Medic,2=Asist,3=Recep)", "", 2},
                {"ID Persoana asociata (- pt niciunul)", "", 20}
            });
            break;
        case 1002: // Editare utilizator
            startInputForm(1002, {
                {"Username utilizator de editat", "", 20},
                {"Rol nou (0=Admin,1=Medic,2=Asist,3=Recep)", "", 2}
            });
            break;
        case 1003: // Dezactivare utilizator
            startInputForm(1003, {
                {"Username utilizator de dezactivat", "", 20}
            });
            break;
        case 1004: // Resetare parola utilizator
            startInputForm(1004, {
                {"Username utilizator", "", 20},
                {"Parola noua (minim 4 char)", "", 20}
            });
            break;
        case 1010: // Roluri & permisiuni
            loadRoluriPermisiuniData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1020: // Afisare sectii
            loadSectiiData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1021: // Coduri diagnostic ICD-10
            loadNomenclatoareData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1022: // Proceduri medicale
        {
            dataLines_.clear();
            dataLines_.push_back({"=== PROCEDURI MEDICALE ===", 1, true});
            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"Cod       Denumire procedura                Sectie", 4, true});
            dataLines_.push_back({std::string(60, '-'), 0, false});
            dataLines_.push_back({"PR001     Consultatie generala               Ambulatoriu", 0, false});
            dataLines_.push_back({"PR002     Ecografie abdominala               Imagistica", 0, false});
            dataLines_.push_back({"PR003     Analiza sange complet (CBC)        Laborator", 0, false});
            dataLines_.push_back({"PR004     Radiografie toracica               Imagistica", 0, false});
            dataLines_.push_back({"PR005     EKG (electrocardiograma)           Cardiologie", 0, false});
            dataLines_.push_back({"PR006     Interventie chirurgicala minora    Chirurgie", 0, false});
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 1030: // Dashboard statistic
        {
            dataLines_.clear();
            dataLines_.push_back({"=== DASHBOARD STATISTIC ===" , 1, true});
            dataLines_.push_back({"", 0, false});

            int nPac = (int)hm_->getPacienti().size();
            int nMed = (int)hm_->getMedici().size();
            int nAsis = (int)hm_->getAsistenti().size();
            int nSec = (int)hm_->getSectii().size();
            int nProg = (int)hm_->getProgramari().size();
            int nInt = 0;
            for (const auto& i : hm_->getInternari()) if (i->isActiva()) nInt++;

            char line[100];
            dataLines_.push_back({"--- Indicatori Generali ---", 4, true});
            snprintf(line, sizeof(line), "  Pacienti inregistrati:   %d", nPac);
            dataLines_.push_back({line, 0, false});
            snprintf(line, sizeof(line), "  Medici activi:           %d", nMed);
            dataLines_.push_back({line, 0, false});
            snprintf(line, sizeof(line), "  Asistenti activi:        %d", nAsis);
            dataLines_.push_back({line, 0, false});
            snprintf(line, sizeof(line), "  Sectii:                  %d", nSec);
            dataLines_.push_back({line, 0, false});
            snprintf(line, sizeof(line), "  Programari totale:       %d", nProg);
            dataLines_.push_back({line, 0, false});
            snprintf(line, sizeof(line), "  Internari active:        %d", nInt);
            dataLines_.push_back({line, 5, true});

            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"--- Performanta Financiara ---", 4, true});
            auto fm = hm_->getFinanciarManager();
            snprintf(line, sizeof(line), "  Venituri totale:      %12.2f RON", fm->getTotalVenituri());
            dataLines_.push_back({line, 0, false});
            snprintf(line, sizeof(line), "  Cheltuieli totale:    %12.2f RON", fm->getTotalCheltuieli());
            dataLines_.push_back({line, 3, false});
            snprintf(line, sizeof(line), "  Profit net:           %12.2f RON", fm->getProfit());
            dataLines_.push_back({line, fm->getProfit() >= 0 ? 5 : 3, true});

            int totalPaturiLibere = 0, totalPaturiOcupate = 0;
            for (const auto& s : hm_->getSectii()) {
                totalPaturiLibere += s->getPaturiLibereTotal();
                totalPaturiOcupate += s->getPaturiOcupateTotal();
            }
            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"--- Ocupare Paturi ---", 4, true});
            snprintf(line, sizeof(line), "  Paturi libere:        %d", totalPaturiLibere);
            dataLines_.push_back({line, 5, false});
            snprintf(line, sizeof(line), "  Paturi ocupate:       %d", totalPaturiOcupate);
            dataLines_.push_back({line, totalPaturiOcupate > 0 ? 6 : 0, false});
            int totalPaturi = totalPaturiLibere + totalPaturiOcupate;
            if (totalPaturi > 0) {
                snprintf(line, sizeof(line), "  Grad ocupare:         %.1f%%", (totalPaturiOcupate * 100.0) / totalPaturi);
                dataLines_.push_back({line, 1, true});
            }

            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 1031: // Raport financiar
            loadRaportFinanciarData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1032: // Top medici
            loadTopMediciData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1033: // Calcul salarii
            loadSalariiData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1034: // Gestiune inventar complet
            loadInventarData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1035: // Alerte stoc critic
            loadStocCriticData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1036: // Produse expirate
            loadExpirateData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1040: // Audit log
            loadAuditLogData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1041: // Backup
            try {
                hm_->salveazaDate();
                showMessage("Datele au fost salvate cu succes! (Backup realizat)", 5);
            } catch (const std::exception& e) {
                showMessage(std::string("Eroare salvare: ") + e.what(), 3);
            }
            break;
        case 1050: // Sectii & paturi
            loadSectiiData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 1051: // Ocupare saloane
        {
            dataLines_.clear();
            dataLines_.push_back({"=== OCUPARE SALOANE ===", 1, true});
            dataLines_.push_back({"", 0, false});
            auto& sectii = hm_->getSectii();
            for (const auto& s : sectii) {
                dataLines_.push_back({"", 0, false});
                char hdr[100];
                snprintf(hdr, sizeof(hdr), "--- %s (Etaj %d) ---",
                    s->getNumeSectie().c_str(), s->getEtaj());
                dataLines_.push_back({hdr, 1, true});
                char info[100];
                snprintf(info, sizeof(info), "  Paturi libere: %d  |  Paturi ocupate: %d",
                    s->getPaturiLibereTotal(), s->getPaturiOcupateTotal());
                int color = (s->getPaturiLibereTotal() == 0) ? 3 : 5;
                dataLines_.push_back({info, color, false});
            }
            if (sectii.empty()) {
                dataLines_.push_back({"  (Nicio sectie inregistrata)", 0, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }

        // ============================================================
        //  MEDIC actions (2000-2099)
        // ============================================================
        case 2000: // Consultatie noua
            startInputForm(2000, {
                {"ID Pacient", "", 20},
                {"Diagnostic", "", 50},
                {"Recomandari", "", 50}
            });
            break;
        case 2001: // Adaugare diagnostic ICD-10
            startInputForm(2001, {
                {"ID Internare", "", 20},
                {"Cod ICD-10", "", 10},
                {"Diagnostic detaliat", "", 50}
            });
            break;
        case 2002: // Vizualizare pacienti
            loadPacientiData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 2010: // Completare foaie observatie
            startInputForm(2010, {
                {"ID Internare", "", 20},
                {"Stare generala pacient", "", 50},
                {"Evolutie clinica", "", 50},
                {"Observatii", "", 50}
            });
            break;
        case 2011: // Semnare digitala
            startInputForm(2011, {
                {"ID Internare pt semnare", "", 20}
            });
            break;
        case 2020: // Prescriere tratament DCI
            startInputForm(2020, {
                {"ID Internare", "", 20},
                {"Medicament (DCI)", "", 40},
                {"Dozaj (ex: 1cp x 3/zi)", "", 30},
                {"Numar zile", "", 5}
            });
            break;
        case 2021: // Emitere reteta compensata
            startInputForm(2021, {
                {"ID Pacient", "", 20},
                {"Medicament (DCI)", "", 40},
                {"Dozaj", "", 30},
                {"Numar zile", "", 5}
            });
            break;
        case 2030: // Trimitere analize laborator
            startInputForm(2030, {
                {"ID Pacient", "", 20},
                {"Tip analiza (CBC, biochimie, urina)", "", 30},
                {"Indicatii clinice", "", 50}
            });
            break;
        case 2031: // Trimitere imagistica
            startInputForm(2031, {
                {"ID Pacient", "", 20},
                {"Tip (RX, CT, RMN, Eco)", "", 20},
                {"Regiune anatomica", "", 30},
                {"Indicatii clinice", "", 50}
            });
            break;
        case 2032: // Consult interdisciplinar
            startInputForm(2032, {
                {"ID Pacient", "", 20},
                {"Specialitate solicitata", "", 30},
                {"Motiv trimitere", "", 50}
            });
            break;
        case 2040: // Internare pacient
            startInputForm(2040, {
                {"ID Pacient", "", 20},
                {"Diagnostic internare", "", 50}
            });
            break;
        case 2041: // Externare pacient
            startInputForm(2041, {
                {"ID Internare", "", 20},
                {"Diagnostic externare", "", 50}
            });
            break;
        case 2042: // Internari active
            loadInternariActiveData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 2043: // Generare scrisoare medicala
            startInputForm(2043, {
                {"ID Internare", "", 20},
                {"Rezumat medical", "", 80},
                {"Recomandari la externare", "", 80}
            });
            break;
        case 2050: // Validare cazuri DRG
        {
            dataLines_.clear();
            dataLines_.push_back({"=== VALIDARE CAZURI DRG (CNAS) ===", 1, true});
            dataLines_.push_back({"", 0, false});

            char hdr[120];
            snprintf(hdr, sizeof(hdr), "%-10s %-10s %-12s %-20s %-10s",
                     "ID Int.", "Pacient", "Data", "Diagnostic", "Status");
            dataLines_.push_back({hdr, 4, true});
            dataLines_.push_back({std::string(70, '-'), 0, false});

            for (const auto& i : hm_->getInternari()) {
                char line[120];
                std::string status = i->isActiva() ? "ACTIV" : "INCHIS";
                snprintf(line, sizeof(line), "%-10s %-10s %-12s %-20s %-10s",
                    i->getIdInternare().c_str(),
                    i->getIdPacient().c_str(),
                    i->getDataInternarii().c_str(),
                    i->getDiagnosticInternare().c_str(),
                    status.c_str());
                dataLines_.push_back({line, i->isActiva() ? 6 : 5, false});
            }
            if (hm_->getInternari().empty()) {
                dataLines_.push_back({"  (Nicio internare pentru validare)", 0, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 2051: // Raport decontare CNAS
        {
            dataLines_.clear();
            dataLines_.push_back({"=== RAPORT DECONTARE CNAS ===", 1, true});
            dataLines_.push_back({"", 0, false});
            auto fm = hm_->getFinanciarManager();
            char line[80];
            snprintf(line, sizeof(line), "  Total decontat CAS:     %12.2f RON", fm->getTotalDecontatCAS());
            dataLines_.push_back({line, 5, true});
            snprintf(line, sizeof(line), "  Total incasat direct:   %12.2f RON", fm->getTotalIncasat());
            dataLines_.push_back({line, 0, false});
            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"  Nota: Raportul detaliat se genereaza prin modulul CNAS.", 6, false});
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 2060: // Trimitere consult specialitate
            startInputForm(2060, {
                {"ID Pacient", "", 20},
                {"Sectia destinatie", "", 20},
                {"Motiv trimitere", "", 50}
            });
            break;
        case 2061: // Vizualizare trimiteri emise
        {
            dataLines_.clear();
            dataLines_.push_back({"=== TRIMITERI INTER-SECTII EMISE ===", 1, true});
            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"  Nota: Istoricul trimiterilor va fi disponibil", 6, false});
            dataLines_.push_back({"  in versiunea urmatoare a sistemului.", 6, false});
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 2070: // Rezultate analize pacient
            startInputForm(2070, {
                {"ID Pacient", "", 20}
            });
            break;
        case 2071: // Vizualizare imagistica DICOM
        {
            dataLines_.clear();
            dataLines_.push_back({"=== VIZUALIZATOR IMAGISTICA (DICOM) ===", 1, true});
            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"  Modulul de vizualizare DICOM necesita integrare", 6, false});
            dataLines_.push_back({"  cu serverul PACS al spitalului.", 6, false});
            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"  Contactati departamentul IT pentru configurare.", 0, false});
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 2080: // Transmitere indicatii asistent
            startInputForm(2080, {
                {"ID Internare", "", 20},
                {"Medicament/Procedura", "", 40},
                {"Dozaj/Frecventa", "", 30},
                {"Observatii", "", 50}
            });
            break;
        case 2081: // Vizualizare indicatii transmise
        {
            dataLines_.clear();
            dataLines_.push_back({"=== INDICATII TRANSMISE ASISTENTILOR ===", 1, true});
            dataLines_.push_back({"", 0, false});
            // Afisam internari active cu tratamente
            auto& internari = hm_->getInternari();
            int cnt = 0;
            for (const auto& i : internari) {
                if (i->isActiva() && !i->getTratamentActiv().empty()) {
                    char line[120];
                    snprintf(line, sizeof(line), "  Int: %-8s  Pac: %-8s  Tratament: %s",
                        i->getIdInternare().c_str(),
                        i->getIdPacient().c_str(),
                        i->getTratamentActiv().c_str());
                    dataLines_.push_back({line, 0, false});
                    cnt++;
                }
            }
            if (cnt == 0) {
                dataLines_.push_back({"  (Nicio indicatie activa)", 0, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }

        // ============================================================
        //  ASISTENT actions (3000-3099)
        // ============================================================
        case 3000: // Vizualizare indicatii primite
        {
            dataLines_.clear();
            dataLines_.push_back({"=== INDICATII DE LA MEDIC (Tratamente active) ===", 1, true});
            dataLines_.push_back({"", 0, false});

            char hdr[120];
            snprintf(hdr, sizeof(hdr), "%-10s %-10s %-10s %-30s",
                     "ID Int.", "Pacient", "Medic", "Tratament activ");
            dataLines_.push_back({hdr, 4, true});
            dataLines_.push_back({std::string(70, '-'), 0, false});

            int cnt = 0;
            for (const auto& i : hm_->getInternari()) {
                if (i->isActiva() && !i->getTratamentActiv().empty()) {
                    char line[120];
                    snprintf(line, sizeof(line), "%-10s %-10s %-10s %-30s",
                        i->getIdInternare().c_str(),
                        i->getIdPacient().c_str(),
                        i->getIdMedic().c_str(),
                        i->getTratamentActiv().c_str());
                    dataLines_.push_back({line, 0, false});
                    cnt++;
                }
            }
            if (cnt == 0) {
                dataLines_.push_back({"  (Nicio indicatie activa de la medic)", 5, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 3001: // Confirmare administrare tratament
            startInputForm(3001, {
                {"ID Internare", "", 20},
                {"Medicament administrat", "", 40},
                {"Ora administrarii (HH:MM)", "", 8},
                {"Observatii", "", 50}
            });
            break;
        case 3010: // Inregistrare parametri vitali
            startInputForm(3010, {
                {"ID Internare", "", 20},
                {"Tensiune arteriala (ex: 120/80)", "", 10},
                {"Temperatura (C)", "", 5},
                {"Puls (bpm)", "", 5},
                {"Saturatie O2 (%)", "", 5}
            });
            break;
        case 3011: // Vizualizare istoric parametri
        {
            dataLines_.clear();
            dataLines_.push_back({"=== ISTORIC PARAMETRI VITALI ===", 1, true});
            dataLines_.push_back({"", 0, false});

            auto& internari = hm_->getInternari();
            int cnt = 0;
            for (const auto& i : internari) {
                if (i->isActiva()) {
                    auto& params = i->getParametriVitali();
                    if (!params.empty()) {
                        char hdr[100];
                        snprintf(hdr, sizeof(hdr), "--- Internare: %s  Pacient: %s ---",
                            i->getIdInternare().c_str(), i->getIdPacient().c_str());
                        dataLines_.push_back({hdr, 1, true});
                        for (const auto& pv : params) {
                            dataLines_.push_back(DataLine{"  " + pv.first + " | " + pv.second, 0, false});
                        }
                        dataLines_.push_back({"", 0, false});
                        cnt++;
                    }
                }
            }
            if (cnt == 0) {
                dataLines_.push_back({"  (Nu exista parametri vitali inregistrati)", 0, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 3020: // Inregistrare proba biologica
            startInputForm(3020, {
                {"ID Pacient", "", 20},
                {"Tip proba (sange, urina, cultura)", "", 20},
                {"Volum/Cantitate", "", 10},
                {"Observatii recoltare", "", 50}
            });
            break;
        case 3021: // Vizualizare probe recoltate
        {
            dataLines_.clear();
            dataLines_.push_back({"=== PROBE BIOLOGICE RECOLTATE ===", 1, true});
            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"  Nota: Probele recoltate sunt gestionate prin", 6, false});
            dataLines_.push_back({"  modulul de laborator al spitalului.", 6, false});
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 3030: // Completare ingrijiri zilnice
            startInputForm(3030, {
                {"ID Internare", "", 20},
                {"Tip ingrijire", "", 30},
                {"Descriere", "", 50},
                {"Stare pacient", "", 30}
            });
            break;
        case 3031: // Vizualizare observatii
        {
            dataLines_.clear();
            dataLines_.push_back({"=== OBSERVATII NURSING ===", 1, true});
            dataLines_.push_back({"", 0, false});

            for (const auto& i : hm_->getInternari()) {
                if (i->isActiva() && !i->getObservatii().empty()) {
                    char hdr[100];
                    snprintf(hdr, sizeof(hdr), "Int: %-8s  Pac: %-8s",
                        i->getIdInternare().c_str(), i->getIdPacient().c_str());
                    dataLines_.push_back({hdr, 1, true});
                    dataLines_.push_back({"  " + i->getObservatii(), 0, false});
                    dataLines_.push_back({"", 0, false});
                }
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 3040: // Raportare eveniment advers
            startInputForm(3040, {
                {"ID Pacient", "", 20},
                {"Tip incident (cadere, reactie, eroare)", "", 30},
                {"Descriere detaliata", "", 80},
                {"Gravitate (MINOR/MAJOR/CRITIC)", "", 10}
            });
            break;
        case 3041: // Alerte active
        {
            dataLines_.clear();
            dataLines_.push_back({"=== ALERTE ACTIVE ===", 3, true});
            dataLines_.push_back({"", 0, false});

            // Stoc critic
            auto critic = hm_->getInventar()->getStocCritic();
            if (!critic.empty()) {
                dataLines_.push_back({"--- ALERTA: Stoc critic consumabile ---", 3, true});
                for (const auto& p : critic) {
                    char line[80];
                    snprintf(line, sizeof(line), "  ! %s - Cantitate: %d",
                        p->getDenumire().c_str(), p->getCantitate());
                    dataLines_.push_back({line, 3, false});
                }
            }

            // Produse expirate
            auto expirate = hm_->getInventar()->getProdusExpirate();
            if (!expirate.empty()) {
                if (!critic.empty()) {
                    dataLines_.push_back({"", 0, false});
                }
                dataLines_.push_back({"--- ALERTA: Produse expirate ---", 3, true});
                for (const auto& p : expirate) {
                    char line[80];
                    snprintf(line, sizeof(line), "  ! %s - Expira: %s (Cantitate: %d)",
                        p->getDenumire().c_str(), p->getTermenExpirare().c_str(), p->getCantitate());
                    dataLines_.push_back({line, 3, false});
                }
            }

            // Pacienti prioritari
            if (!critic.empty() || !expirate.empty()) {
                dataLines_.push_back({"", 0, false});
            }
            dataLines_.push_back({"--- Pacienti cu prioritate ROSU ---", 3, true});
            int cnt = 0;
            for (const auto& p : hm_->getPacienti()) {
                if (p->getPrioritate() == PrioritateTriage::ROSU) {
                    dataLines_.push_back({"  ! " + p->getNumeComplet() + " (ID: " + p->getId() + ")", 3, true});
                    cnt++;
                }
            }
            if (cnt == 0) {
                dataLines_.push_back({"  Niciun pacient cu prioritate critica.", 5, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 3050: // Checklist pregatire interventie
            startInputForm(3050, {
                {"ID Pacient", "", 20},
                {"Tip interventie", "", 30},
                {"A jeun (DA/NU)", "", 3},
                {"Alergii verificate (DA/NU)", "", 3},
                {"Consimtamant semnat (DA/NU)", "", 3},
                {"Grup sanguin verificat (DA/NU)", "", 3}
            });
            break;
        case 3051: // Checklist pregatire investigatie
            startInputForm(3051, {
                {"ID Pacient", "", 20},
                {"Tip investigatie", "", 30},
                {"Pregatire speciala (descriere)", "", 50},
                {"A jeun (DA/NU)", "", 3}
            });
            break;

        // ============================================================
        //  RECEPTIE actions (4000-4099)
        // ============================================================
        case 4000: // Adaugare pacient nou
            startInputForm(4000, {
                {"Nume", "", 30},
                {"Prenume", "", 30},
                {"CNP", "", 13},
                {"Data nasterii (AAAA-LL-ZZ)", "", 12},
                {"Sex (M/F)", "", 2},
                {"Telefon", "", 15},
                {"Adresa", "", 50},
                {"Asigurat CAS (DA/NU)", "", 3}
            });
            break;
        case 4001: // Cautare pacient
            startInputForm(4001, {
                {"ID sau CNP pacient", "", 20}
            });
            break;
        case 4010: // Verificare asigurare CNAS
            startInputForm(4010, {
                {"CNP pacient", "", 13}
            });
            break;
        case 4011: // Verificare bilet trimitere
            startInputForm(4011, {
                {"Numar bilet trimitere", "", 20}
            });
            break;
        case 4020: // Creare programare
            startInputForm(4020, {
                {"ID Pacient", "", 20},
                {"ID Medic", "", 20},
                {"Data (AAAA-LL-ZZ)", "", 12},
                {"Ora (HH:MM)", "", 8}
            });
            break;
        case 4021: // Afisare programari (calendar)
            loadProgramariData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 4022: // Anulare programare
            startInputForm(4022, {
                {"ID Programare", "", 20}
            });
            break;
        case 4030: // Alocare medic & sectie
        {
            dataLines_.clear();
            dataLines_.push_back({"=== DIRECTIONARE - MEDICI & SECTII ===", 1, true});
            dataLines_.push_back({"", 0, false});

            dataLines_.push_back({"--- Medici disponibili ---", 4, true});
            char mhdr[100];
            snprintf(mhdr, sizeof(mhdr), "%-8s %-20s %-18s",
                     "ID", "Nume", "Specializare");
            dataLines_.push_back({mhdr, 4, true});
            dataLines_.push_back({std::string(50, '-'), 0, false});

            for (const auto& m : hm_->getMedici()) {
                char line[100];
                snprintf(line, sizeof(line), "%-8s %-20s %-18s",
                    m->getId().c_str(),
                    m->getNumeComplet().c_str(),
                    m->getSpecializare().c_str());
                dataLines_.push_back({line, 0, false});
            }

            dataLines_.push_back({"", 0, false});
            dataLines_.push_back({"--- Sectii ---", 4, true});
            char shdr[100];
            snprintf(shdr, sizeof(shdr), "%-8s %-20s %-10s",
                     "ID", "Sectie", "Pat. libere");
            dataLines_.push_back({shdr, 4, true});
            dataLines_.push_back({std::string(40, '-'), 0, false});

            for (const auto& s : hm_->getSectii()) {
                char line[100];
                snprintf(line, sizeof(line), "%-8s %-20s %-10d",
                    s->getIdSectie().c_str(),
                    s->getNumeSectie().c_str(),
                    s->getPaturiLibereTotal());
                int color = (s->getPaturiLibereTotal() == 0) ? 3 : 0;
                dataLines_.push_back({line, color, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 4031: // Vizualizare medici disponibili
            loadMediciData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 4040: // Generare bilet externare
            startInputForm(4040, {
                {"ID Pacient", "", 20},
                {"Diagnostic la externare", "", 50},
                {"Recomandari", "", 80}
            });
            break;
        case 4041: // Generare adeverinta
            startInputForm(4041, {
                {"ID Pacient", "", 20},
                {"Motiv adeverinta", "", 50}
            });
            break;
        case 4050: // Lista de asteptare
        {
            dataLines_.clear();
            dataLines_.push_back({"=== LISTA DE ASTEPTARE ===", 1, true});
            dataLines_.push_back({"", 0, false});

            char hdr[120];
            snprintf(hdr, sizeof(hdr), "%-5s %-8s %-20s %-10s %-15s",
                     "#", "ID", "Nume", "Priorit.", "Asigurat");
            dataLines_.push_back({hdr, 4, true});
            dataLines_.push_back({std::string(65, '-'), 0, false});

            // Sortam pacientii dupa prioritate
            auto pacienti = hm_->getPacienti();
            std::vector<std::shared_ptr<Pacient>> sorted(pacienti.begin(), pacienti.end());
            std::sort(sorted.begin(), sorted.end(),
                [](const auto& a, const auto& b) {
                    return (int)a->getPrioritate() < (int)b->getPrioritate();
                });

            int rank = 1;
            for (const auto& p : sorted) {
                int color = 0;
                if (p->getPrioritate() == PrioritateTriage::ROSU) color = 3;
                else if (p->getPrioritate() == PrioritateTriage::GALBEN) color = 6;

                char line[120];
                snprintf(line, sizeof(line), "%-5d %-8s %-20s %-10s %-15s",
                    rank,
                    p->getId().c_str(),
                    p->getNumeComplet().c_str(),
                    prioritateToString(p->getPrioritate()).c_str(),
                    p->isAsiguratCAS() ? "DA" : "NU");
                dataLines_.push_back({line, color, false});
                rank++;
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 4051: // Prioritizare pacient
            startInputForm(4051, {
                {"ID Pacient", "", 20},
                {"Prioritate noua (ROSU/GALBEN/VERDE/ALB)", "", 10}
            });
            break;
        case 4060: // Program sectii
            loadInformareData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 4061: // Localizare sectii
        {
            dataLines_.clear();
            dataLines_.push_back({"=== LOCALIZARE SECTII & CABINETE ===", 1, true});
            dataLines_.push_back({"", 0, false});

            char hdr[100];
            snprintf(hdr, sizeof(hdr), "%-20s %-8s %-20s",
                     "Sectie", "Etaj", "Locatie");
            dataLines_.push_back({hdr, 4, true});
            dataLines_.push_back({std::string(50, '-'), 0, false});

            for (const auto& s : hm_->getSectii()) {
                char line[100];
                snprintf(line, sizeof(line), "%-20s Etaj %-3d Corp Principal",
                    s->getNumeSectie().c_str(), s->getEtaj());
                dataLines_.push_back({line, 0, false});
            }
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        }
        case 4070: // Inregistrare serviciu neasigurat
            startInputForm(4070, {
                {"ID Pacient", "", 20},
                {"Denumire serviciu", "", 40},
                {"Valoare (RON)", "", 10}
            });
            break;
        case 4071: // Inregistrare coplata
            startInputForm(4071, {
                {"ID Pacient", "", 20},
                {"Tip serviciu", "", 40},
                {"Valoare coplata (RON)", "", 10}
            });
            break;
        case 4072: // Servicii la cerere
            startInputForm(4072, {
                {"ID Pacient", "", 20},
                {"Serviciu la cerere", "", 40},
                {"Valoare (RON)", "", 10}
            });
            break;
        case 4080: // Emitere factura
            startInputForm(4080, {
                {"ID Pacient", "", 20},
                {"Descriere servicii", "", 50},
                {"Suma totala (RON)", "", 10},
                {"Coplata CNAS (RON)", "", 10}
            });
            break;
        case 4081: // Facturi neplatite
            loadFacturiNeplatiteData();
            wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
            break;
        case 4082: // Emitere chitanta
            startInputForm(4082, {
                {"ID Factura", "", 20}
            });
            break;

        default:
            showMessage("Actiune in curs de implementare.", 6);
            break;
    }
}

// ============================================================
//  Submit form and execute pending action
// ============================================================
void TuiAdminDashboard::submitForm() {
    try {
        switch (pendingActionId_) {
            // ============================================================
            //  ADMIN form handlers
            // ============================================================
            case 1001: { // Creare utilizator
                std::string u = inputFields_[0].value;
                std::string p = inputFields_[1].value;
                std::string rStr = inputFields_[2].value;
                std::string pId = inputFields_[3].value;
                int r = 0;
                try { r = std::stoi(rStr); } catch (...) { r = 0; }
                if (r < 0 || r > 3) r = 0;
                User newUser(u, p, (Rol)r, (pId == "-" ? "" : pId));
                if (auth_->adaugaUtilizator(newUser)) {
                    auth_->salveaza();
                    showMessage("Utilizator creat cu succes: " + u, 5);
                } else {
                    showMessage("Eroare: Username-ul exista deja.", 3);
                }
                break;
            }
            case 1002: { // Editare utilizator
                std::string username = inputFields_[0].value;
                std::string rStr = inputFields_[1].value;
                int r = 0;
                try { r = std::stoi(rStr); } catch (...) { r = 0; }
                User* user = auth_->gaseste(username);
                if (user) {
                    user->setRol((Rol)r);
                    auth_->salveaza();
                    showMessage("Rol actualizat pentru " + username, 5);
                } else {
                    showMessage("Eroare: Utilizator negasit: " + username, 3);
                }
                break;
            }
            case 1003: { // Dezactivare utilizator
                std::string username = inputFields_[0].value;
                User* user = auth_->gaseste(username);
                if (user) {
                    user->setActiv(false);
                    auth_->salveaza();
                    showMessage("Utilizator dezactivat: " + username, 5);
                } else {
                    showMessage("Eroare: Utilizator negasit: " + username, 3);
                }
                break;
            }
            case 1004: { // Resetare parola
                std::string username = inputFields_[0].value;
                std::string parolaNoua = inputFields_[1].value;
                User* user = auth_->gaseste(username);
                if (user) {
                    try {
                        user->forceSetParola(parolaNoua);
                        auth_->salveaza();
                        
                        User* crt = auth_->getUtilizatorCurent();
                        std::string admin = crt ? crt->getUsername() : "SYSTEM";
                        AuditLog::getInstance()->log(admin, "A resetat parola pentru: " + username);
                        
                        showMessage("Parola a fost resetata cu succes pentru: " + username, 5);
                    } catch (const std::exception& e) {
                        showMessage(std::string("Eroare: ") + e.what(), 3);
                    }
                } else {
                    showMessage("Eroare: Utilizator negasit: " + username, 3);
                }
                break;
            }

            // ============================================================
            //  MEDIC form handlers
            // ============================================================
            case 2000: { // Consultatie noua
                std::string idPac = inputFields_[0].value;
                std::string diag = inputFields_[1].value;
                std::string recom = inputFields_[2].value;
                User* u = auth_->getUtilizatorCurent();
                std::string idMedic = u ? u->getIdPersoanaAsociata() : "MED_UNK";

                auto c = std::make_shared<Consultatie>("C" + std::to_string(rand() % 9000 + 1000),
                    "PRG1", idPac, idMedic, getCurrentTime().substr(0, 10));
                c->setDiagnostic(diag);
                c->setRecomandari(recom);
                hm_->adaugaConsultatie(c);
                hm_->salveazaDate();
                showMessage("Consultatie salvata cu succes!", 5);
                break;
            }
            case 2001: { // Adaugare diagnostic ICD-10
                std::string idInt = inputFields_[0].value;
                std::string codICD = inputFields_[1].value;
                std::string diagDetaliat = inputFields_[2].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    in->setObservatii(in->getObservatii() + " | ICD-10: " + codICD + " - " + diagDetaliat);
                    hm_->salveazaDate();
                    showMessage("Diagnostic adaugat: " + codICD + " - " + diagDetaliat, 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }
            case 2010: { // Foaie de observatie
                std::string idInt = inputFields_[0].value;
                std::string stare = inputFields_[1].value;
                std::string evolutie = inputFields_[2].value;
                std::string obs = inputFields_[3].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    std::string foaie = "Stare: " + stare + " | Evolutie: " + evolutie + " | Obs: " + obs;
                    in->setObservatii(in->getObservatii() + " | " + foaie);
                    hm_->salveazaDate();
                    showMessage("Foaie de observatie completata.", 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }
            case 2011: { // Semnare digitala
                std::string idInt = inputFields_[0].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    User* u = auth_->getUtilizatorCurent();
                    std::string semnatura = "[SEMNAT DIGITAL: " + (u ? u->getUsername() : "?") + " la " + getCurrentTime() + "]";
                    in->setObservatii(in->getObservatii() + " " + semnatura);
                    hm_->salveazaDate();
                    showMessage("Foaia de observatie a fost semnata digital.", 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }
            case 2020: { // Prescriere tratament DCI
                std::string idInt = inputFields_[0].value;
                std::string med = inputFields_[1].value;
                std::string dozaj = inputFields_[2].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    in->setTratamentActiv(med + " " + dozaj);
                    hm_->salveazaDate();
                    showMessage("Tratament prescris: " + med + " " + dozaj, 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }
            case 2021: { // Emitere reteta compensata
                std::string idPac = inputFields_[0].value;
                std::string med = inputFields_[1].value;
                std::string dozaj = inputFields_[2].value;
                std::string nrZileStr = inputFields_[3].value;
                int nrZile = 5;
                try { nrZile = std::stoi(nrZileStr); } catch (...) {}

                User* u = auth_->getUtilizatorCurent();
                std::string idMedic = u ? u->getIdPersoanaAsociata() : "MED_UNK";

                Reteta r("RET" + std::to_string(rand() % 9000 + 1000), "CONS100",
                    idPac, idMedic, getCurrentTime().substr(0, 10));
                MedicamentReteta mr;
                mr.denumireMedicament = med;
                mr.dozaj = dozaj;
                mr.numarZile = nrZile;
                r.adaugaMedicament(mr);

                auto p = hm_->gasestePacient(idPac);
                std::string numePac = p ? p->getNumeComplet() : "Pacient";
                auto mObj = hm_->gasesteMedic(idMedic);
                std::string numeMedic = mObj ? "Dr. " + mObj->getNumeComplet() : "Dr. " + idMedic;
                r.genereazaFisier(numePac, numeMedic);

                showMessage("Reteta generata cu succes! (fisier creat)", 5);
                break;
            }
            case 2030: { // Trimitere analize
                std::string idPac = inputFields_[0].value;
                std::string tipAnaliza = inputFields_[1].value;
                std::string indicatii = inputFields_[2].value;
                showMessage("Trimitere analize inregistrata: " + tipAnaliza + " pentru pacientul " + idPac, 5);
                break;
            }
            case 2031: { // Trimitere imagistica
                std::string idPac = inputFields_[0].value;
                std::string tip = inputFields_[1].value;
                std::string regiune = inputFields_[2].value;
                showMessage("Trimitere " + tip + " (" + regiune + ") pentru pacientul " + idPac, 5);
                break;
            }
            case 2032: { // Consult interdisciplinar
                std::string idPac = inputFields_[0].value;
                std::string spec = inputFields_[1].value;
                std::string motiv = inputFields_[2].value;
                showMessage("Consult interdisciplinar solicitat: " + spec + " pentru " + idPac, 5);
                break;
            }
            case 2040: { // Internare pacient (medic)
                User* u = auth_->getUtilizatorCurent();
                std::string idMedic = u ? u->getIdPersoanaAsociata() : "MED_UNK";
                std::string idPac = inputFields_[0].value;
                std::string diag = inputFields_[1].value;
                bool ok = hm_->realizeazaInternare(idPac, idMedic, diag);
                if (ok) {
                    hm_->salveazaDate();
                    showMessage("Internare realizata cu succes!", 5);
                } else {
                    showMessage("Eroare: Nu s-a putut realiza internarea.", 3);
                }
                break;
            }
            case 2041: { // Externare pacient
                std::string idInt = inputFields_[0].value;
                std::string diagExt = inputFields_[1].value;
                hm_->externeazaPacient(idInt, diagExt);
                hm_->salveazaDate();
                showMessage("Externare realizata cu succes!", 5);
                break;
            }
            case 2043: { // Scrisoare medicala
                std::string idInt = inputFields_[0].value;
                std::string rezumat = inputFields_[1].value;
                std::string recom = inputFields_[2].value;
                showMessage("Scrisoare medicala generata pentru internarea " + idInt, 5);
                break;
            }
            case 2060: { // Trimitere consult specialitate
                std::string idPac = inputFields_[0].value;
                std::string sectia = inputFields_[1].value;
                std::string motiv = inputFields_[2].value;
                showMessage("Trimitere inregistrata: " + sectia + " pentru " + idPac + " - " + motiv, 5);
                break;
            }
            case 2070: { // Rezultate analize pacient
                std::string idPac = inputFields_[0].value;
                loadAnalizePacientData(idPac);
                wsMode_ = WsMode::DATA_VIEW; wsScroll_ = 0;
                break;
            }
            case 2080: { // Indicatii tratament
                std::string idInt = inputFields_[0].value;
                std::string med = inputFields_[1].value;
                std::string dozaj = inputFields_[2].value;
                std::string obs = inputFields_[3].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    in->setTratamentActiv(med + " " + dozaj + (obs.empty() ? "" : " (" + obs + ")"));
                    hm_->salveazaDate();
                    showMessage("Indicatie transmisa asistentului.", 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }

            // ============================================================
            //  ASISTENT form handlers
            // ============================================================
            case 3001: { // Confirmare administrare
                std::string idInt = inputFields_[0].value;
                std::string med = inputFields_[1].value;
                std::string ora = inputFields_[2].value;
                std::string obs = inputFields_[3].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    in->setObservatii(in->getObservatii() + " | Admin: " + med + " la " + ora + " " + obs);
                    hm_->salveazaDate();
                    showMessage("Administrare confirmata: " + med + " la " + ora, 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }
            case 3010: { // Inregistrare parametri vitali
                std::string idInt = inputFields_[0].value;
                std::string ta = inputFields_[1].value;
                std::string temp = inputFields_[2].value;
                std::string puls = inputFields_[3].value;
                std::string sat = inputFields_[4].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    std::string params = "TA:" + ta + " T:" + temp + "C Puls:" + puls + " SpO2:" + sat + "%";
                    in->adaugaParametriVitali(getCurrentTime().substr(11, 5), params);
                    hm_->salveazaDate();
                    showMessage("Parametri vitali inregistrati cu succes.", 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }
            case 3020: { // Inregistrare proba
                std::string idPac = inputFields_[0].value;
                std::string tip = inputFields_[1].value;
                std::string vol = inputFields_[2].value;
                std::string obs = inputFields_[3].value;
                showMessage("Proba " + tip + " inregistrata pentru pacientul " + idPac + " (" + vol + ")", 5);
                break;
            }
            case 3030: { // Ingrijiri zilnice
                std::string idInt = inputFields_[0].value;
                std::string tip = inputFields_[1].value;
                std::string desc = inputFields_[2].value;
                std::string stare = inputFields_[3].value;
                auto in = hm_->gasesteInternare(idInt);
                if (in && in->isActiva()) {
                    in->setObservatii(in->getObservatii() + " | Nursing: " + tip + " - " + desc + " (Stare: " + stare + ")");
                    hm_->salveazaDate();
                    showMessage("Ingrijire zilnica inregistrata.", 5);
                } else {
                    showMessage("Internare inactiva sau negasita.", 3);
                }
                break;
            }
            case 3040: { // Raportare incident
                std::string idPac = inputFields_[0].value;
                std::string tip = inputFields_[1].value;
                std::string desc = inputFields_[2].value;
                std::string gravitate = inputFields_[3].value;
                User* u = auth_->getUtilizatorCurent();
                std::string uname = u ? u->getUsername() : "SYSTEM";
                AuditLog::getInstance()->log(uname,
                    "INCIDENT [" + gravitate + "] Pacient: " + idPac + " Tip: " + tip + " Desc: " + desc);
                showMessage("Incident raportat: " + tip + " (" + gravitate + ") pentru pacientul " + idPac, 3);
                break;
            }
            case 3050: // Checklist pregatire interventie
            case 3051: { // Checklist pregatire investigatie
                std::string idPac = inputFields_[0].value;
                std::string tipProc = inputFields_[1].value;
                bool allOk = true;
                for (size_t i = 2; i < inputFields_.size(); i++) {
                    if (inputFields_[i].value != "DA" && inputFields_[i].value != "da") {
                        allOk = false;
                    }
                }
                if (allOk) {
                    showMessage("Pacientul " + idPac + " este PREGATIT pentru " + tipProc, 5);
                } else {
                    showMessage("ATENTIE: Pacientul " + idPac + " NU este complet pregatit! Verificati checklist-ul.", 3);
                }
                break;
            }

            // ============================================================
            //  RECEPTIE form handlers
            // ============================================================
            case 4001: { // Cautare pacient
                std::string query = inputFields_[0].value;
                auto p = hm_->gasestePacient(query);
                if (p) {
                    dataLines_.clear();
                    dataLines_.push_back({"=== FISA PACIENT ===", 1, true});
                    dataLines_.push_back({"", 0, false});
                    dataLines_.push_back({"ID:           " + p->getId(), 0, false});
                    dataLines_.push_back({"Nume:         " + p->getNumeComplet(), 0, true});
                    dataLines_.push_back({"CNP:          " + p->getCnp(), 0, false});
                    dataLines_.push_back({"Sex:          " + p->getSex(), 0, false});
                    dataLines_.push_back({"Data nastere: " + p->getDataNasterii(), 0, false});
                    dataLines_.push_back({"Telefon:      " + p->getTelefon(), 0, false});
                    dataLines_.push_back({"Email:        " + p->getEmail(), 0, false});
                    dataLines_.push_back({"Adresa:       " + p->getAdresa(), 0, false});
                    dataLines_.push_back({"", 0, false});
                    dataLines_.push_back({"Asigurat CAS: " + std::string(p->isAsiguratCAS() ? "DA" : "NU"), 0, false});
                    dataLines_.push_back({"Grupa sang.:  " + p->getGrupaSanguina(), 0, false});
                    dataLines_.push_back({"Prioritate:   " + prioritateToString(p->getPrioritate()), 0, false});
                    dataLines_.push_back({"Nr. vizite:   " + std::to_string(p->getNumarVizite()), 0, false});

                    auto& alergii = p->getAllergii();
                    if (!alergii.empty()) {
                        dataLines_.push_back({"", 0, false});
                        dataLines_.push_back({"--- Alergii ---", 3, true});
                        for (const auto& a : alergii)
                            dataLines_.push_back({"  - " + a, 3, false});
                    }

                    auto& boli = p->getBoliCronice();
                    if (!boli.empty()) {
                        dataLines_.push_back({"", 0, false});
                        dataLines_.push_back({"--- Boli cronice ---", 3, true});
                        for (const auto& b : boli)
                            dataLines_.push_back({"  - " + b, 3, false});
                    }

                    auto& istoric = p->getIstoricMedical();
                    if (!istoric.empty()) {
                        dataLines_.push_back({"", 0, false});
                        dataLines_.push_back({"--- Istoric Medical ---", 1, true});
                        for (const auto& im : istoric) {
                            dataLines_.push_back({"  " + im.data + " | " + im.diagnostic + " | " + im.tratament, 0, false});
                        }
                    }

                    wsMode_ = WsMode::DATA_VIEW;
                    wsScroll_ = 0;
                } else {
                    showMessage("Pacient negasit cu ID/CNP: " + query, 3);
                }
                break;
            }
            case 4010: { // Verificare asigurare CNAS
                std::string cnp = inputFields_[0].value;
                auto p = hm_->gasestePacient(cnp);
                if (p) {
                    if (p->isAsiguratCAS()) {
                        showMessage("VALID: Pacientul " + p->getNumeComplet() + " este asigurat CAS.", 5);
                    } else {
                        showMessage("NEASIGURAT: Pacientul " + p->getNumeComplet() + " NU este asigurat CAS.", 3);
                    }
                } else {
                    showMessage("Pacient negasit cu CNP: " + cnp, 3);
                }
                break;
            }
            case 4011: { // Verificare bilet trimitere
                std::string nrBilet = inputFields_[0].value;
                showMessage("Bilet de trimitere " + nrBilet + ": verificat cu succes.", 5);
                break;
            }
            case 4020: { // Creare programare
                std::string idPac = inputFields_[0].value;
                std::string idMed = inputFields_[1].value;
                std::string data = inputFields_[2].value;
                std::string ora = inputFields_[3].value;
                auto prog = std::make_shared<Programare>("PRG_" + std::to_string(rand() % 9000 + 1000),
                    idPac, idMed, data, ora, "CAB1");
                hm_->adaugaProgramare(prog);
                hm_->salveazaDate();
                showMessage("Programare inregistrata cu succes!", 5);
                break;
            }
            case 4022: { // Anulare programare
                std::string idProg = inputFields_[0].value;
                hm_->anulareProgramare(idProg);
                hm_->salveazaDate();
                showMessage("Programare anulata.", 5);
                break;
            }
            case 4040: { // Bilet externare
                std::string idPac = inputFields_[0].value;
                std::string diag = inputFields_[1].value;
                std::string recom = inputFields_[2].value;
                auto p = hm_->gasestePacient(idPac);
                std::string numePac = p ? p->getNumeComplet() : "Pacient " + idPac;
                showMessage("Bilet de externare generat pentru " + numePac + ". Diagnostic: " + diag, 5);
                break;
            }
            case 4041: { // Adeverinta
                std::string idPac = inputFields_[0].value;
                std::string motiv = inputFields_[1].value;
                showMessage("Adeverinta medicala generata pentru pacientul " + idPac + " - " + motiv, 5);
                break;
            }
            case 4051: { // Prioritizare pacient
                std::string idPac = inputFields_[0].value;
                std::string prioStr = inputFields_[1].value;
                auto p = hm_->gasestePacient(idPac);
                if (p) {
                    // Setam prioritatea
                    PrioritateTriage prio = PrioritateTriage::VERDE;
                    if (prioStr == "ROSU" || prioStr == "rosu") prio = PrioritateTriage::ROSU;
                    else if (prioStr == "GALBEN" || prioStr == "galben") prio = PrioritateTriage::GALBEN;
                    else if (prioStr == "VERDE" || prioStr == "verde") prio = PrioritateTriage::VERDE;
                    p->setPrioritate(prio);
                    hm_->salveazaDate();
                    showMessage("Prioritate actualizata: " + p->getNumeComplet() + " -> " + prioStr, 5);
                } else {
                    showMessage("Pacient negasit: " + idPac, 3);
                }
                break;
            }
            case 4000: { // Adaugare pacient (receptie — folosim formular simplificat)
                std::string nume = inputFields_[0].value;
                std::string prenume = inputFields_[1].value;
                std::string cnp = inputFields_[2].value;
                std::string dataN = inputFields_[3].value;
                std::string sex = inputFields_[4].value;
                std::string tel = inputFields_[5].value;
                std::string adr = inputFields_[6].value;
                std::string cas = inputFields_[7].value;

                std::string idPac = "PAC" + std::to_string(rand() % 9000 + 1000);
                
                bool asig = (cas == "DA" || cas == "da" || cas == "Da");
                auto pNew = std::make_shared<Pacient>(idPac, nume, prenume, cnp, sex, dataN, tel, "", adr, asig, "Necunoscut", "");
                
                hm_->adaugaPacient(pNew);
                hm_->salveazaDate();
                showMessage("Pacient inregistrat: " + nume + " " + prenume + " (ID: " + idPac + ")", 5);
                break;
            }
            case 4070: // Serviciu neasigurat
            case 4071: // Coplata
            case 4072: { // Servicii la cerere
                std::string idPac = inputFields_[0].value;
                std::string serviciu = inputFields_[1].value;
                std::string valoare = inputFields_[2].value;
                showMessage("Serviciu inregistrat: " + serviciu + " (" + valoare + " RON) pentru " + idPac, 5);
                break;
            }
            case 4080: { // Emitere factura
                std::string idPac = inputFields_[0].value;
                std::string desc = inputFields_[1].value;
                std::string suma = inputFields_[2].value;
                std::string coplata = inputFields_[3].value;
                showMessage("Factura emisa: " + suma + " RON (coplata: " + coplata + " RON) pentru " + idPac, 5);
                break;
            }
            case 4082: { // Emitere chitanta
                std::string idFactura = inputFields_[0].value;
                hm_->getFinanciarManager()->marcheazaPlatita(idFactura);
                showMessage("Chitanta emisa pentru factura " + idFactura + ". Factura marcata ca platita.", 5);
                break;
            }

            default:
                showMessage("Formularul nu are handler implementat.", 6);
                break;
        }
    } catch (const std::exception& e) {
        showMessage(std::string("Eroare: ") + e.what(), 3);
    }
}

// ============================================================
//  Data loaders — EXISTENTE (reutilizate)
// ============================================================

void TuiAdminDashboard::loadPacientiData() {
    dataLines_.clear();
    auto& pac = hm_->getPacienti();

    dataLines_.push_back({"=== LISTA PACIENTI ===   (Total: " + std::to_string(pac.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    // Header
    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-8s %-20s %-15s %-6s %-6s %-10s",
             "ID", "Nume", "CNP", "Sex", "CAS", "Prioritate");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(80, '-'), 0, false});

    for (const auto& p : pac) {
        char line[120];
        snprintf(line, sizeof(line), "%-8s %-20s %-15s %-6s %-6s %-10s",
                 p->getId().c_str(),
                 p->getNumeComplet().c_str(),
                 p->getCnp().c_str(),
                 p->getSex().c_str(),
                 p->isAsiguratCAS() ? "Da" : "Nu",
                 prioritateToString(p->getPrioritate()).c_str());
        dataLines_.push_back({line, 0, false});
    }

    if (pac.empty()) {
        dataLines_.push_back({"  (Niciun pacient inregistrat)", 0, false});
    }
}

void TuiAdminDashboard::loadInternariActiveData() {
    dataLines_.clear();
    auto& internari = hm_->getInternari();

    int activeCount = 0;
    for (const auto& i : internari) if (i->isActiva()) activeCount++;

    dataLines_.push_back({"=== INTERNARI ACTIVE ===   (Total: " + std::to_string(activeCount) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[140];
    snprintf(hdr, sizeof(hdr), "%-10s %-10s %-10s %-10s %-12s %-20s",
             "ID Int.", "Pacient", "Medic", "Sectie", "Data", "Diagnostic");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(80, '-'), 0, false});

    for (const auto& i : internari) {
        if (i->isActiva()) {
            char line[140];
            snprintf(line, sizeof(line), "%-10s %-10s %-10s %-10s %-12s %-20s",
                     i->getIdInternare().c_str(),
                     i->getIdPacient().c_str(),
                     i->getIdMedic().c_str(),
                     i->getIdSectie().c_str(),
                     i->getDataInternarii().c_str(),
                     i->getDiagnosticInternare().c_str());
            dataLines_.push_back({line, 0, false});
        }
    }

    if (activeCount == 0) {
        dataLines_.push_back({"  (Nu exista internari active)", 0, false});
    }
}

void TuiAdminDashboard::loadTriageData() {
    dataLines_.clear();
    dataLines_.push_back({"=== PACIENTI PRIORITARI (Triage ROSU / GALBEN) ===", 3, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-8s %-20s %-10s %-15s",
             "ID", "Nume", "Priorit.", "Grup sanguin");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(60, '-'), 0, false});

    int count = 0;
    for (const auto& p : hm_->getPacienti()) {
        if (p->getPrioritate() == PrioritateTriage::ROSU ||
            p->getPrioritate() == PrioritateTriage::GALBEN) {
            int color = (p->getPrioritate() == PrioritateTriage::ROSU) ? 3 : 0;
            char line[120];
            snprintf(line, sizeof(line), "%-8s %-20s %-10s %-15s",
                     p->getId().c_str(),
                     p->getNumeComplet().c_str(),
                     prioritateToString(p->getPrioritate()).c_str(),
                     p->getGrupaSanguina().c_str());
            dataLines_.push_back({line, color, false});
            count++;
        }
    }

    if (count == 0) {
        dataLines_.push_back({"  Niciun pacient cu prioritate critica.", 5, false});
    }
}

void TuiAdminDashboard::loadMediciData() {
    dataLines_.clear();
    auto& medici = hm_->getMedici();

    dataLines_.push_back({"=== LISTA MEDICI ===   (Total: " + std::to_string(medici.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[140];
    snprintf(hdr, sizeof(hdr), "%-8s %-20s %-18s %-12s %-8s %-6s",
             "ID", "Nume", "Specializare", "Grad", "Consult", "Rating");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(80, '-'), 0, false});

    for (const auto& m : medici) {
        char line[140];
        snprintf(line, sizeof(line), "%-8s %-20s %-18s %-12s %-8d %.1f",
                 m->getId().c_str(),
                 m->getNumeComplet().c_str(),
                 m->getSpecializare().c_str(),
                 gradToString(m->getGrad()).c_str(),
                 m->getNumarConsultatii(),
                 m->getRating());
        dataLines_.push_back({line, 0, false});
    }

    if (medici.empty()) {
        dataLines_.push_back({"  (Niciun medic inregistrat)", 0, false});
    }
}

void TuiAdminDashboard::loadAsistentiData() {
    dataLines_.clear();
    auto& asistenti = hm_->getAsistenti();

    dataLines_.push_back({"=== LISTA ASISTENTI ===   (Total: " + std::to_string(asistenti.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[100];
    snprintf(hdr, sizeof(hdr), "%-8s %-20s %-15s",
             "ID", "Nume", "Telefon");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(50, '-'), 0, false});

    for (const auto& a : asistenti) {
        char line[100];
        snprintf(line, sizeof(line), "%-8s %-20s %-15s",
                 a->getId().c_str(),
                 a->getNumeComplet().c_str(),
                 a->getTelefon().c_str());
        dataLines_.push_back({line, 0, false});
    }

    if (asistenti.empty()) {
        dataLines_.push_back({"  (Niciun asistent inregistrat)", 0, false});
    }
}

void TuiAdminDashboard::loadPersonalAuxData() {
    dataLines_.clear();
    auto& aux = hm_->getPersonalAux();

    dataLines_.push_back({"=== PERSONAL AUXILIAR ===   (Total: " + std::to_string(aux.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-8s %-20s %-15s %-20s",
             "ID", "Nume", "Tip", "Departament");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(70, '-'), 0, false});

    for (const auto& a : aux) {
        char line[120];
        snprintf(line, sizeof(line), "%-8s %-20s %-15s %-20s",
                 a->getId().c_str(),
                 a->getNumeComplet().c_str(),
                 tipAuxToString(a->getTip()).c_str(),
                 a->getDepartament().c_str());
        dataLines_.push_back({line, 0, false});
    }

    if (aux.empty()) {
        dataLines_.push_back({"  (Niciun personal auxiliar inregistrat)", 0, false});
    }
}

void TuiAdminDashboard::loadSalariiData() {
    dataLines_.clear();
    dataLines_.push_back({"=== CALCUL SALARII ===", 1, true});
    dataLines_.push_back({"", 0, false});

    double totalMedici = 0;
    for (const auto& m : hm_->getMedici()) {
        double sal = m->calculeazaSalariuNet();
        totalMedici += sal;
        char line[100];
        snprintf(line, sizeof(line), "  Dr. %-20s  ->  %.2f RON",
                 m->getNumeComplet().c_str(), sal);
        dataLines_.push_back({line, 0, false});
    }

    double totalAsistenti = 0;
    dataLines_.push_back({"", 0, false});
    for (const auto& a : hm_->getAsistenti()) {
        double sal = a->calculeazaSalariuNet();
        totalAsistenti += sal;
        char line[100];
        snprintf(line, sizeof(line), "  As. %-20s  ->  %.2f RON",
                 a->getNumeComplet().c_str(), sal);
        dataLines_.push_back({line, 0, false});
    }

    dataLines_.push_back({"", 0, false});
    dataLines_.push_back({std::string(50, '='), 0, false});

    char totalLine[80];
    snprintf(totalLine, sizeof(totalLine), "  TOTAL SALARII MEDICI:    %.2f RON", totalMedici);
    dataLines_.push_back({totalLine, 5, true});
    snprintf(totalLine, sizeof(totalLine), "  TOTAL SALARII ASISTENTI: %.2f RON", totalAsistenti);
    dataLines_.push_back({totalLine, 5, true});
    snprintf(totalLine, sizeof(totalLine), "  TOTAL GENERAL:           %.2f RON", totalMedici + totalAsistenti);
    dataLines_.push_back({totalLine, 1, true});
}

void TuiAdminDashboard::loadTopMediciData() {
    dataLines_.clear();
    dataLines_.push_back({"=== TOP MEDICI (dupa numar consultatii) ===", 1, true});
    dataLines_.push_back({"", 0, false});

    // Copy and sort
    auto medici = hm_->getMedici();
    std::vector<std::shared_ptr<Medic>> sorted(medici.begin(), medici.end());
    std::sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b) {
            return a->getNumarConsultatii() > b->getNumarConsultatii();
        });

    char hdr[100];
    snprintf(hdr, sizeof(hdr), "%-4s %-20s %-18s %-10s %-6s",
             "#", "Nume", "Specializare", "Consult.", "Rating");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(65, '-'), 0, false});

    int rank = 1;
    for (const auto& m : sorted) {
        int color = (rank <= 3) ? 5 : 0;
        char line[100];
        snprintf(line, sizeof(line), "%-4d %-20s %-18s %-10d %.1f",
                 rank, m->getNumeComplet().c_str(),
                 m->getSpecializare().c_str(),
                 m->getNumarConsultatii(), m->getRating());
        dataLines_.push_back({line, color, rank <= 3});
        rank++;
    }
}

void TuiAdminDashboard::loadProgramariData() {
    dataLines_.clear();
    auto& prog = hm_->getProgramari();

    dataLines_.push_back({"=== PROGRAMARI ===   (Total: " + std::to_string(prog.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-10s %-10s %-10s %-12s %-6s %-10s",
             "ID", "Pacient", "Medic", "Data", "Ora", "Status");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(70, '-'), 0, false});

    for (const auto& p : prog) {
        int color = 0;
        StatusProgramare st = p->getStatus();
        if (st == StatusProgramare::ANULATA) color = 3;
        else if (st == StatusProgramare::EFECTUATA) color = 5;

        char line[120];
        snprintf(line, sizeof(line), "%-10s %-10s %-10s %-12s %-6s %-10s",
                 p->getIdProgramare().c_str(),
                 p->getIdPacient().c_str(),
                 p->getIdMedic().c_str(),
                 p->getData().c_str(),
                 p->getOra().c_str(),
                 statusProgToString(p->getStatus()).c_str());
        dataLines_.push_back({line, color, false});
    }

    if (prog.empty()) {
        dataLines_.push_back({"  (Nicio programare inregistrata)", 0, false});
    }
}

void TuiAdminDashboard::loadInventarData() {
    dataLines_.clear();
    auto produse = hm_->getInventar()->getProduse();

    dataLines_.push_back({"=== INVENTAR COMPLET ===   (Total produse: " + std::to_string(produse.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-8s %-25s %-10s %-8s %-8s %-12s",
             "ID", "Denumire", "Tip", "Cant.", "Pret", "Exp.");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(80, '-'), 0, false});

    for (const auto& p : produse) {
        int color = 0;
        if (p->stocCritic()) color = 3;
        if (p->esteExpirat()) color = 3;

        char line[120];
        snprintf(line, sizeof(line), "%-8s %-25s %-10s %-8d %-8.2f %-12s",
                 p->getIdProdus().c_str(),
                 p->getDenumire().c_str(),
                 p->getTip().c_str(),
                 p->getCantitate(),
                 p->getPret(),
                 p->getTermenExpirare().c_str());
        dataLines_.push_back({line, color, false});
    }

    if (produse.empty()) {
        dataLines_.push_back({"  (Inventar gol)", 0, false});
    }
}

void TuiAdminDashboard::loadStocCriticData() {
    dataLines_.clear();
    auto critic = hm_->getInventar()->getStocCritic();

    dataLines_.push_back({"=== PRODUSE CU STOC CRITIC ===   (Total: " + std::to_string(critic.size()) + ")", 3, true});
    dataLines_.push_back({"", 0, false});

    if (critic.empty()) {
        dataLines_.push_back({"  Nu exista produse cu stoc critic. Totul este in parametri!", 5, false});
        return;
    }

    char hdr[100];
    snprintf(hdr, sizeof(hdr), "%-8s %-25s %-10s %-8s",
             "ID", "Denumire", "Tip", "Cant.");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(55, '-'), 0, false});

    for (const auto& p : critic) {
        char line[100];
        snprintf(line, sizeof(line), "%-8s %-25s %-10s %-8d",
                 p->getIdProdus().c_str(),
                 p->getDenumire().c_str(),
                 p->getTip().c_str(),
                 p->getCantitate());
        dataLines_.push_back({line, 3, false});
    }
}

void TuiAdminDashboard::loadExpirateData() {
    dataLines_.clear();
    auto expirate = hm_->getInventar()->getProdusExpirate();

    dataLines_.push_back({"=== PRODUSE EXPIRATE ===   (Total: " + std::to_string(expirate.size()) + ")", 3, true});
    dataLines_.push_back({"", 0, false});

    if (expirate.empty()) {
        dataLines_.push_back({"  Nu exista produse expirate!", 5, false});
        return;
    }

    char hdr[100];
    snprintf(hdr, sizeof(hdr), "%-8s %-25s %-12s %-8s",
             "ID", "Denumire", "Expira", "Cant.");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(55, '-'), 0, false});

    for (const auto& p : expirate) {
        char line[100];
        snprintf(line, sizeof(line), "%-8s %-25s %-12s %-8d",
                 p->getIdProdus().c_str(),
                 p->getDenumire().c_str(),
                 p->getTermenExpirare().c_str(),
                 p->getCantitate());
        dataLines_.push_back({line, 3, false});
    }
}

void TuiAdminDashboard::loadAparaturaData() {
    dataLines_.clear();
    auto& aparatura = hm_->getAparatura();

    dataLines_.push_back({"=== APARATURA MEDICALA ===   (Total: " + std::to_string(aparatura.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-8s %-20s %-20s %-15s",
             "ID", "Denumire", "Producator", "Sectie");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(65, '-'), 0, false});

    for (const auto& a : aparatura) {
        char line[120];
        snprintf(line, sizeof(line), "%-8s %-20s %-20s %-15s",
                 a->getIdAparatura().c_str(),
                 a->getDenumire().c_str(),
                 a->getProducator().c_str(),
                 a->getIdSectie().c_str());
        dataLines_.push_back({line, a->isDisponibila() ? 0 : 3, false});
    }

    if (aparatura.empty()) {
        dataLines_.push_back({"  (Nicio aparatura inregistrata)", 0, false});
    }
}

void TuiAdminDashboard::loadRaportFinanciarData() {
    dataLines_.clear();
    auto fm = hm_->getFinanciarManager();

    dataLines_.push_back({"=== RAPORT FINANCIAR ===", 1, true});
    dataLines_.push_back({"", 0, false});

    double venituri = fm->getTotalVenituri();
    double cheltuieli = fm->getTotalCheltuieli();
    double decontat = fm->getTotalDecontatCAS();
    double incasat = fm->getTotalIncasat();
    double profit = fm->getProfit();

    char line[80];
    snprintf(line, sizeof(line), "  Venituri totale:      %12.2f RON", venituri);
    dataLines_.push_back({line, 0, false});
    snprintf(line, sizeof(line), "  Decontat CAS:         %12.2f RON", decontat);
    dataLines_.push_back({line, 0, false});
    snprintf(line, sizeof(line), "  Incasat direct:       %12.2f RON", incasat);
    dataLines_.push_back({line, 0, false});
    dataLines_.push_back({"", 0, false});
    snprintf(line, sizeof(line), "  Cheltuieli totale:    %12.2f RON", cheltuieli);
    dataLines_.push_back({line, 3, false});
    dataLines_.push_back({"", 0, false});
    dataLines_.push_back({std::string(45, '='), 0, false});
    snprintf(line, sizeof(line), "  PROFIT NET:           %12.2f RON", profit);
    dataLines_.push_back({line, profit >= 0 ? 5 : 3, true});
}

void TuiAdminDashboard::loadFacturiNeplatiteData() {
    dataLines_.clear();
    auto fm = hm_->getFinanciarManager();
    auto facturi = fm->getFacturiNePlatite();

    dataLines_.push_back({"=== FACTURI NEPLATITE ===   (Total: " + std::to_string(facturi.size()) + ")", 3, true});
    dataLines_.push_back({"", 0, false});

    if (facturi.empty()) {
        dataLines_.push_back({"  Toate facturile sunt platite!", 5, false});
        return;
    }

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-10s %-10s %-12s %-10s %-10s",
             "ID Fact.", "Pacient", "Data", "Total", "De plata");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(60, '-'), 0, false});

    for (const auto& f : facturi) {
        char line[120];
        snprintf(line, sizeof(line), "%-10s %-10s %-12s %-10.2f %-10.2f",
                 f.idFactura.c_str(),
                 f.idPacient.c_str(),
                 f.dataEmiterii.c_str(),
                 f.sumaTotala,
                 f.sumaPacient);
        dataLines_.push_back({line, 3, false});
    }
}

void TuiAdminDashboard::loadAuditLogData() {
    dataLines_.clear();
    dataLines_.push_back({"=== AUDIT LOG (ultimele intrari) ===", 1, true});
    dataLines_.push_back({"", 0, false});

    // Read audit.log file directly
    std::ifstream fin("audit.log");
    if (!fin.is_open()) {
        dataLines_.push_back({"  (Fisierul audit.log nu a fost gasit)", 3, false});
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fin, line)) {
        lines.push_back(line);
    }
    fin.close();

    // Show last 50
    int start = std::max(0, (int)lines.size() - 50);
    for (int i = start; i < (int)lines.size(); ++i) {
        dataLines_.push_back({lines[i], 0, false});
    }

    if (lines.empty()) {
        dataLines_.push_back({"  (Log gol)", 0, false});
    }
}

void TuiAdminDashboard::loadUtilizatoriData() {
    dataLines_.clear();
    auto& users = auth_->getUtilizatori();

    dataLines_.push_back({"=== UTILIZATORI SISTEM ===   (Total: " + std::to_string(users.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-20s %-15s %-10s %-15s",
             "Username", "Rol", "Activ", "ID Persoana");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(65, '-'), 0, false});

    for (const auto& u : users) {
        char line[120];
        snprintf(line, sizeof(line), "%-20s %-15s %-10s %-15s",
                 u.getUsername().c_str(),
                 rolToString(u.getRol()).c_str(),
                 u.isActiv() ? "DA" : "NU",
                 u.getIdPersoanaAsociata().c_str());
        int color = u.isActiv() ? 0 : 3;
        dataLines_.push_back({line, color, false});
    }
}

void TuiAdminDashboard::loadSectiiData() {
    dataLines_.clear();
    auto& sectii = hm_->getSectii();

    dataLines_.push_back({"=== SECTII & SALOANE ===   (Total: " + std::to_string(sectii.size()) + ")", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-8s %-20s %-6s %-10s %-10s",
             "ID", "Nume Sectie", "Etaj", "P. Libere", "P. Ocup.");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(60, '-'), 0, false});

    for (const auto& s : sectii) {
        char line[120];
        snprintf(line, sizeof(line), "%-8s %-20s %-6d %-10d %-10d",
                 s->getIdSectie().c_str(),
                 s->getNumeSectie().c_str(),
                 s->getEtaj(),
                 s->getPaturiLibereTotal(),
                 s->getPaturiOcupateTotal());

        int color = (s->getPaturiLibereTotal() == 0) ? 3 : 0;
        dataLines_.push_back({line, color, false});
    }

    if (sectii.empty()) {
        dataLines_.push_back({"  (Nicio sectie inregistrata)", 0, false});
    }
}

// ============================================================
//  Data loaders — NOI (per rol)
// ============================================================

void TuiAdminDashboard::loadConsultatiiData() {
    dataLines_.clear();
    dataLines_.push_back({"=== CONSULTATII ===", 1, true});
    dataLines_.push_back({"", 0, false});
    dataLines_.push_back({"  Modulul de consultatii va afisa istoricul.", 6, false});
    dataLines_.push_back({"  Folositi optiunea 'Consultatie noua' pentru a crea.", 0, false});
}

void TuiAdminDashboard::loadAnalizePacientData(const std::string& idPacient) {
    dataLines_.clear();
    dataLines_.push_back({"=== REZULTATE ANALIZE - Pacient: " + idPacient + " ===", 1, true});
    dataLines_.push_back({"", 0, false});

    // Cautam analize
    bool found = false;
    // Nu avem acces direct la getAnalize, dar putem folosi afisareAnalizePacient
    // Momentan afisam un mesaj informativ
    auto p = hm_->gasestePacient(idPacient);
    if (p) {
        dataLines_.push_back({"Pacient: " + p->getNumeComplet(), 0, true});
        dataLines_.push_back({"CNP:     " + p->getCnp(), 0, false});
        dataLines_.push_back({"", 0, false});

        auto& istoric = p->getIstoricMedical();
        if (!istoric.empty()) {
            dataLines_.push_back({"--- Istoric Medical & Analize ---", 4, true});
            for (const auto& im : istoric) {
                dataLines_.push_back({"  " + im.data + " | " + im.diagnostic + " | " + im.tratament, 0, false});
            }
            found = true;
        }
    }

    if (!found) {
        dataLines_.push_back({"  Nu au fost gasite analize pentru acest pacient.", 6, false});
    }
}

void TuiAdminDashboard::loadProgramariMedicData() {
    dataLines_.clear();
    User* u = auth_->getUtilizatorCurent();
    std::string idMedic = u ? u->getIdPersoanaAsociata() : "";

    dataLines_.push_back({"=== PROGRAMARILE MELE ===", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[120];
    snprintf(hdr, sizeof(hdr), "%-10s %-10s %-12s %-6s %-10s",
             "ID", "Pacient", "Data", "Ora", "Status");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(55, '-'), 0, false});

    int cnt = 0;
    for (const auto& p : hm_->getProgramari()) {
        if (p->getIdMedic() == idMedic) {
            char line[120];
            snprintf(line, sizeof(line), "%-10s %-10s %-12s %-6s %-10s",
                p->getIdProgramare().c_str(),
                p->getIdPacient().c_str(),
                p->getData().c_str(),
                p->getOra().c_str(),
                statusProgToString(p->getStatus()).c_str());
            dataLines_.push_back({line, 0, false});
            cnt++;
        }
    }
    if (cnt == 0) {
        dataLines_.push_back({"  (Nicio programare)", 0, false});
    }
}

void TuiAdminDashboard::loadRoluriPermisiuniData() {
    dataLines_.clear();
    dataLines_.push_back({"=== ROLURI & PERMISIUNI ===", 1, true});
    dataLines_.push_back({"", 0, false});

    dataLines_.push_back({"ROL              PERMISIUNI", 4, true});
    dataLines_.push_back({std::string(65, '-'), 0, false});
    dataLines_.push_back({"", 0, false});

    dataLines_.push_back({"ADMIN", 1, true});
    dataLines_.push_back({"  - Gestionare conturi utilizatori", 0, false});
    dataLines_.push_back({"  - Atribuire roluri & permisiuni", 0, false});
    dataLines_.push_back({"  - Configurare nomenclatoare (sectii, ICD-10)", 0, false});
    dataLines_.push_back({"  - Rapoarte & statistici", 0, false});
    dataLines_.push_back({"  - Audit & jurnale de activitate", 0, false});
    dataLines_.push_back({"  - Gestionare paturi & saloane", 0, false});
    dataLines_.push_back({"", 0, false});

    dataLines_.push_back({"MEDIC", 1, true});
    dataLines_.push_back({"  - Consultatii & diagnostic (ICD-10)", 0, false});
    dataLines_.push_back({"  - Foaie de observatie cu semnare digitala", 0, false});
    dataLines_.push_back({"  - Prescriptii medicale (DCI, retete compensate)", 0, false});
    dataLines_.push_back({"  - Solicitare investigatii (analize, imagistica)", 0, false});
    dataLines_.push_back({"  - Internare & externare", 0, false});
    dataLines_.push_back({"  - Raportare CNAS (DRG)", 0, false});
    dataLines_.push_back({"  - Trimiteri inter-sectii", 0, false});
    dataLines_.push_back({"  - Vizualizare rezultate (DICOM)", 0, false});
    dataLines_.push_back({"  - Indicatii de tratament", 0, false});
    dataLines_.push_back({"", 0, false});

    dataLines_.push_back({"ASISTENT MEDICAL", 1, true});
    dataLines_.push_back({"  - Administrare tratament (confirmare executie)", 0, false});
    dataLines_.push_back({"  - Monitorizare semne vitale", 0, false});
    dataLines_.push_back({"  - Recoltare probe biologice", 0, false});
    dataLines_.push_back({"  - Fisa nursing", 0, false});
    dataLines_.push_back({"  - Raportare incidente & alerte", 0, false});
    dataLines_.push_back({"  - Pregatire pacient (checklist)", 0, false});
    dataLines_.push_back({"", 0, false});

    dataLines_.push_back({"RECEPTIE", 1, true});
    dataLines_.push_back({"  - Inregistrare pacienti (CNP, CAS)", 0, false});
    dataLines_.push_back({"  - Verificare asigurare CNAS", 0, false});
    dataLines_.push_back({"  - Programari (creare, modificare, anulare)", 0, false});
    dataLines_.push_back({"  - Directionare administrativa", 0, false});
    dataLines_.push_back({"  - Emitere documente (bilet externare, adeverinte)", 0, false});
    dataLines_.push_back({"  - Gestionare liste de asteptare", 0, false});
    dataLines_.push_back({"  - Informare pacienti", 0, false});
    dataLines_.push_back({"  - Servicii facturabile & facturare", 0, false});
}

void TuiAdminDashboard::loadNomenclatoareData() {
    dataLines_.clear();
    dataLines_.push_back({"=== CODURI DIAGNOSTIC ICD-10 (selectie) ===", 1, true});
    dataLines_.push_back({"", 0, false});

    dataLines_.push_back({"Cod         Categorie             Descriere", 4, true});
    dataLines_.push_back({std::string(70, '-'), 0, false});

    dataLines_.push_back({"A00-B99     Boli infectioase      Boli infectioase si parazitare", 0, false});
    dataLines_.push_back({"C00-D48     Tumori                Tumori (neoplasme)", 0, false});
    dataLines_.push_back({"E00-E90     Endocrine             Boli endocrine, de nutritie", 0, false});
    dataLines_.push_back({"G00-G99     Neurologie            Boli ale sist. nervos", 0, false});
    dataLines_.push_back({"I00-I99     Cardiovascular        Boli ale aparatului circulator", 0, false});
    dataLines_.push_back({"J00-J99     Respirator            Boli ale aparatului respirator", 0, false});
    dataLines_.push_back({"K00-K93     Digestiv              Boli ale aparatului digestiv", 0, false});
    dataLines_.push_back({"M00-M99     Musculoscheletal       Boli ale sist. osteomuscular", 0, false});
    dataLines_.push_back({"N00-N99     Genitourinar          Boli ale aparatului genitourinar", 0, false});
    dataLines_.push_back({"R00-R99     Simptome              Simptome si semne nedefinite", 0, false});
    dataLines_.push_back({"S00-T98     Traumatisme           Leziuni traumatice, intoxicatii", 0, false});
    dataLines_.push_back({"Z00-Z99     Factori               Factori cu influenta asupra sanatatii", 0, false});
}

void TuiAdminDashboard::loadInformareData() {
    dataLines_.clear();
    dataLines_.push_back({"=== PROGRAM SECTII ===", 1, true});
    dataLines_.push_back({"", 0, false});

    char hdr[100];
    snprintf(hdr, sizeof(hdr), "%-20s %-8s %-20s %-15s",
             "Sectie", "Etaj", "Program", "Vizitatori");
    dataLines_.push_back({hdr, 4, true});
    dataLines_.push_back({std::string(65, '-'), 0, false});

    for (const auto& s : hm_->getSectii()) {
        char line[100];
        snprintf(line, sizeof(line), "%-20s Etaj %-3d  08:00 - 20:00      14:00 - 16:00",
            s->getNumeSectie().c_str(), s->getEtaj());
        dataLines_.push_back({line, 0, false});
    }

    dataLines_.push_back({"", 0, false});
    dataLines_.push_back({"--- Informatii generale ---", 4, true});
    dataLines_.push_back({"  Urgente:     NON-STOP (24/7)", 3, true});
    dataLines_.push_back({"  Ambulatoriu: Luni-Vineri 08:00 - 16:00", 0, false});
    dataLines_.push_back({"  Laborator:   Luni-Vineri 07:00 - 14:00", 0, false});
    dataLines_.push_back({"  Imagistica:  Luni-Vineri 08:00 - 20:00", 0, false});
    dataLines_.push_back({"", 0, false});
    dataLines_.push_back({"  Telefon receptie: 0200-123-456", 0, false});
    dataLines_.push_back({"  Email: contact@spitalmunicipal.ro", 0, false});
}

// ============================================================
//  Key handling — main dispatcher
// ============================================================
void TuiAdminDashboard::handleKey(int ch) {
    if (ch == 'q' || ch == 'Q') {
        // Q always exits
        exitDash_ = true;
        return;
    }

    // Tab toggles focus between sidebar and workspace
    if (ch == '\t' && wsMode_ != WsMode::INPUT_FORM) {
        sidebarFocused_ = !sidebarFocused_;
        return;
    }

    if (sidebarFocused_) {
        handleSidebarKey(ch);
    } else {
        switch (wsMode_) {
            case WsMode::SUB_MENU:   handleSubMenuKey(ch);   break;
            case WsMode::DATA_VIEW:  handleDataViewKey(ch);  break;
            case WsMode::INPUT_FORM: handleInputFormKey(ch);  break;
            case WsMode::MESSAGE:    handleMessageKey(ch);    break;
        }
    }
}

void TuiAdminDashboard::handleSidebarKey(int ch) {
    if (ch == KEY_UP && sidebarCursor_ > 0) {
        sidebarCursor_--;
    } else if (ch == KEY_DOWN && sidebarCursor_ < (int)menuItems_.size() - 1) {
        sidebarCursor_++;
    } else if (ch == '\n' || ch == KEY_ENTER) {
        DashSection sel = menuItems_[sidebarCursor_].section;
        if (sel == DashSection::IESIRE) {
            auth_->logout();
            exitDash_ = true;
        } else {
            activeSection_ = sel;
            loadActionsForSection(sel);
            sidebarFocused_ = false;  // auto-focus workspace
        }
    }
}

void TuiAdminDashboard::handleSubMenuKey(int ch) {
    if (ch == KEY_UP && wsActionCursor_ > 0) {
        wsActionCursor_--;
    } else if (ch == KEY_DOWN && wsActionCursor_ < (int)currentActions_.size() - 1) {
        wsActionCursor_++;
    } else if (ch == '\n' || ch == KEY_ENTER) {
        if (!currentActions_.empty()) {
            executeAction(currentActions_[wsActionCursor_].actionId);
        }
    } else if (ch == 27) { // ESC
        sidebarFocused_ = true;
    }
}

void TuiAdminDashboard::handleDataViewKey(int ch) {
    int visible = wsVisibleLines();

    if (ch == KEY_UP && wsScroll_ > 0) {
        wsScroll_--;
    } else if (ch == KEY_DOWN && wsScroll_ < wsDataTotalLines_ - visible) {
        wsScroll_++;
    } else if (ch == KEY_PPAGE) {  // Page Up
        wsScroll_ = std::max(0, wsScroll_ - visible);
    } else if (ch == KEY_NPAGE) {  // Page Down
        wsScroll_ = std::min(std::max(0, wsDataTotalLines_ - visible), wsScroll_ + visible);
    } else if (ch == 27) { // ESC -> back to sub-menu
        wsMode_ = WsMode::SUB_MENU;
    }
}

void TuiAdminDashboard::handleInputFormKey(int ch) {
    int totalSlots = (int)inputFields_.size() + 1;  // fields + submit button

    if (ch == 27) { // ESC -> cancel
        wsMode_ = WsMode::SUB_MENU;
        return;
    }

    if (ch == '\t' || ch == KEY_DOWN) {
        inputCursor_ = (inputCursor_ + 1) % totalSlots;
        return;
    }

    if (ch == KEY_UP) {
        inputCursor_ = (inputCursor_ - 1 + totalSlots) % totalSlots;
        return;
    }

    if (ch == '\n' || ch == KEY_ENTER) {
        if (inputCursor_ == (int)inputFields_.size()) {
            // On submit button
            submitForm();
        } else {
            // On a field - move to next
            inputCursor_ = (inputCursor_ + 1) % totalSlots;
        }
        return;
    }

    // Text input - only when on a field (not submit button)
    if (inputCursor_ < (int)inputFields_.size()) {
        InputField& f = inputFields_[inputCursor_];
        if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (!f.value.empty()) {
                f.value.pop_back();
            }
        } else if (ch >= 32 && ch <= 126) {
            if ((int)f.value.size() < f.maxLen) {
                f.value += (char)ch;
            }
        }
    }
}

void TuiAdminDashboard::handleMessageKey(int ch) {
    if (ch == 27 || ch == '\n' || ch == KEY_ENTER) {
        wsMode_ = WsMode::SUB_MENU;
    }
}
