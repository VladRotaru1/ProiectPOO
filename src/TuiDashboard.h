#pragma once
// ============================================================
//  TuiDashboard.h — Dashboard cu ncurses, personalizat per rol
//  Roluri: ADMIN, MEDIC, ASISTENT, RECEPTIE
//  Fiecare rol vede doar sectiunile si actiunile proprii.
// ============================================================
#include <ncurses.h>
#include <string>
#include <chrono>
#include <vector>
#include <functional>
#include "HospitalManager.h"
#include "AuthService.h"

// ============================================================
//  Sectiunile dashboard-ului — per rol
// ============================================================
enum class DashSection {
    // ---- ADMIN ----
    ADMIN_CONTURI,           // Gestionare conturi
    ADMIN_ROLURI,            // Roluri & permisiuni
    ADMIN_NOMENCLATOARE,     // Configurare nomenclatoare
    ADMIN_RAPOARTE,          // Rapoarte & statistici
    ADMIN_AUDIT,             // Audit & jurnale
    ADMIN_PATURI,            // Gestionare paturi & saloane

    // ---- MEDIC ----
    MEDIC_CONSULTATII,       // Consultatii & diagnostic
    MEDIC_FOAIE_OBS,         // Foaie de observatie
    MEDIC_PRESCRIPTII,       // Prescriptii medicale
    MEDIC_INVESTIGATII,      // Solicitare investigatii
    MEDIC_INTERNARE,         // Internare & externare
    MEDIC_CNAS,              // Raportare CNAS
    MEDIC_TRIMITERI,         // Trimiteri inter-sectii
    MEDIC_REZULTATE,         // Vizualizare rezultate
    MEDIC_INDICATII,         // Indicatii de tratament

    // ---- ASISTENT ----
    ASIST_TRATAMENT,         // Administrare tratament
    ASIST_SEMNE_VITALE,      // Monitorizare semne vitale
    ASIST_RECOLTARE,         // Recoltare probe biologice
    ASIST_FISA_NURSING,      // Fisa nursing
    ASIST_INCIDENTE,         // Raportare incidente
    ASIST_PREGATIRE,         // Pregatire pacient

    // ---- RECEPTIE ----
    RECEP_INREGISTRARE,      // Inregistrare pacienti
    RECEP_ASIGURARE,         // Verificare asigurare
    RECEP_PROGRAMARI,        // Programari
    RECEP_DIRECTIONARE,      // Directionare administrativa
    RECEP_DOCUMENTE,         // Emitere documente
    RECEP_LISTE_ASTEPTARE,   // Gestionare liste de asteptare
    RECEP_INFORMARE,         // Informare pacienti
    RECEP_SERVICII,          // Inregistrare servicii facturabile
    RECEP_FACTURA,           // Emitere factura & chitanta

    IESIRE                   // Logout / Iesire (comun tuturor)
};

// Modul de afisare al workspace-ului
enum class WsMode {
    SUB_MENU,    // lista de actiuni navigabila
    DATA_VIEW,   // tabel/date read-only (scrollable)
    INPUT_FORM,  // formular cu campuri text
    MESSAGE      // mesaj rezultat (succes/eroare)
};

// O actiune dintr-un sub-meniu workspace
struct WsAction {
    std::string label;
    int actionId;
};

// Camp de input pentru formulare
struct InputField {
    std::string label;
    std::string value;
    int maxLen;
};

// Element sidebar
struct SidebarItem {
    DashSection section;
    std::string icon;
    std::string label;
};

// O linie de text formatata pentru afisare in data view
struct DataLine {
    std::string text;
    int colorPair;   // 0 = implicit
    bool bold;
};

class TuiAdminDashboard {
public:
    TuiAdminDashboard(HospitalManager* hm, AuthService* auth);
    ~TuiAdminDashboard();

    void run();

private:
    HospitalManager* hm_;
    AuthService*     auth_;
    Rol              rol_;   // Rolul utilizatorului curent

    // ---- Ferestre ncurses ----
    WINDOW* headerWin_;
    WINDOW* sidebarWin_;
    WINDOW* workspaceWin_;
    WINDOW* footerWin_;

    // ---- Dimensiuni terminal ----
    int termRows_, termCols_;

    // ---- Constante layout ----
    static constexpr int HEADER_H  = 3;
    static constexpr int FOOTER_H  = 2;
    static constexpr int SIDEBAR_W = 32;

    // ---- Stare Sidebar ----
    DashSection activeSection_;
    int         sidebarCursor_;
    bool        sidebarFocused_;
    bool        exitDash_;
    std::vector<SidebarItem> menuItems_;

    // ---- Stare Workspace ----
    WsMode wsMode_;
    int    wsActionCursor_;      // cursor in sub-meniu actiuni
    int    wsScroll_;            // offset scroll in data view
    int    wsDataTotalLines_;    // total linii in data view
    std::vector<WsAction>   currentActions_;  // actiunile sectiunii curente
    std::vector<DataLine>   dataLines_;       // liniile de afisat in DATA_VIEW
    std::string             messageText_;     // mesaj in MODE_MESSAGE
    int                     messageColor_;    // culoare mesaj

    // ---- Stare Formular ----
    std::vector<InputField> inputFields_;
    int                     inputCursor_;     // campul activ
    int                     pendingActionId_; // actiunea care a generat formularul

    std::chrono::steady_clock::time_point lastTimeRefresh_;

    // ---- Metode de initializare ----
    void initPlanes();
    void initMenu();
    void destroyPlanes();
    void loadActionsForSection(DashSection sec);

    // ---- Randare ----
    void renderAll();
    void renderHeader();
    void renderSidebar();
    void renderWorkspace();
    void renderFooter();
    void clearWorkspace();

    // Randare workspace pe moduri
    void renderWsSubMenu();
    void renderWsDataView();
    void renderWsInputForm();
    void renderWsMessage();

    // ---- Executie actiuni ----
    void executeAction(int actionId);
    void submitForm();

    // ---- Populare date (existente, reutilizate) ----
    void loadPacientiData();
    void loadInternariActiveData();
    void loadTriageData();
    void loadMediciData();
    void loadAsistentiData();
    void loadPersonalAuxData();
    void loadSalariiData();
    void loadTopMediciData();
    void loadProgramariData();
    void loadInventarData();
    void loadStocCriticData();
    void loadExpirateData();
    void loadAparaturaData();
    void loadRaportFinanciarData();
    void loadFacturiNeplatiteData();
    void loadAuditLogData();
    void loadUtilizatoriData();
    void loadSectiiData();

    // ---- Populare date (noi, per rol) ----
    void loadConsultatiiData();
    void loadAnalizePacientData(const std::string& idPacient);
    void loadProgramariMedicData();
    void loadRoluriPermisiuniData();
    void loadNomenclatoareData();
    void loadInformareData();

    // ---- Input handling ----
    void handleKey(int ch);
    void handleSidebarKey(int ch);
    void handleSubMenuKey(int ch);
    void handleDataViewKey(int ch);
    void handleInputFormKey(int ch);
    void handleMessageKey(int ch);

    // ---- Utilitare ----
    std::string getCurrentTime() const;
    int wsVisibleLines() const;   // cate linii se pot afisa in workspace
    void showMessage(const std::string& msg, int color);
    void startInputForm(int actionId, const std::vector<InputField>& fields);
    std::string getSectionTitle(DashSection sec) const;
};
