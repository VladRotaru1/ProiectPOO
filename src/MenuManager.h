#pragma once

class MenuManager {
private:
    void meniuAdmin();
    void meniuMedic();
    void meniuAsistent();
    void meniuReceptie();

public:
    MenuManager() = default;
    void start();
};
