#pragma once
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <memory>

// ============================================================
// Observer Pattern - NotificationService (Singleton)
// Observerii se inregistreaza si primesc notificari automat
// ============================================================

// Interfata Observer
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onNotificare(const std::string& tip, const std::string& mesaj) = 0;
};

// Observer concret - afisare la consola
class ConsoleObserver : public IObserver {
private:
    std::string nume;
public:
    explicit ConsoleObserver(const std::string& n) : nume(n) {}
    void onNotificare(const std::string& tip, const std::string& mesaj) override {
        std::cout << "\n  *** ALERTA [" << tip << "] -> " << mesaj << " ***\n";
    }
};

// Observer concret - logare in fisier
class FileObserver : public IObserver {
private:
    std::string fisierLog;
public:
    explicit FileObserver(const std::string& f) : fisierLog(f) {}
    void onNotificare(const std::string& tip, const std::string& mesaj) override;
};

// Subject (Publisher) - NotificationService Singleton
class NotificationService {
private:
    std::vector<std::shared_ptr<IObserver>> observeri;
    std::vector<std::pair<std::string,std::string>> istoricNotificari;

    NotificationService() = default;
    NotificationService(const NotificationService&) = delete;
    NotificationService& operator=(const NotificationService&) = delete;

public:
    static NotificationService* getInstance();

    void inregistreazaObserver(std::shared_ptr<IObserver> obs);
    void eliminaObserver(std::shared_ptr<IObserver> obs);
    void notifica(const std::string& tip, const std::string& mesaj);
    void afisareIstoricNotificari() const;
    int getNumarNotificari() const { return (int)istoricNotificari.size(); }
};
