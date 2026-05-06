#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <mutex>

// ============================================================
// AuditLog - singleton care logheza toate actiunile importante
// ============================================================

class AuditLog {
private:
    std::string fisierLog;

    AuditLog() : fisierLog("audit.log") {}
    AuditLog(const AuditLog&) = delete;
    AuditLog& operator=(const AuditLog&) = delete;

public:
    static AuditLog* getInstance();
    void setFisier(const std::string& path);
    void log(const std::string& utilizator, const std::string& actiune);
    void afisareLog(int ultimeleN = 50) const;
    std::string getTimestamp() const;
};
