#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "HospitalManager.h"
#include "Pacient.h"
#include "Medic.h"
#include "Programare.h"

void testHospitalManagerSingleton() {
    std::cout << "Running testHospitalManagerSingleton...\n";
    HospitalManager* h1 = HospitalManager::getInstance();
    HospitalManager* h2 = HospitalManager::getInstance();
    assert(h1 == h2);
    std::cout << "testHospitalManagerSingleton passed!\n";
}

void testPacientCRUD() {
    std::cout << "Running testPacientCRUD...\n";
    HospitalManager* hm = HospitalManager::getInstance();
    
    auto p = std::make_shared<Pacient>("TEST_PAC1", "Test", "User", "1900101000000", "M", "01.01.1990", 
                                      "0711111111", "test@user.com", "Adresa", true, "0+", "0722222222");
    hm->adaugaPacient(p);
    
    auto found = hm->gasestePacient("TEST_PAC1");
    assert(found != nullptr);
    assert(found->getNume() == "Test");
    
    auto foundCnp = hm->gasestePacient("1900101000000");
    assert(foundCnp != nullptr);
    assert(foundCnp->getCodPacient() == "TEST_PAC1");
    
    std::cout << "testPacientCRUD passed!\n";
}

void testMedicCRUD() {
    std::cout << "Running testMedicCRUD...\n";
    HospitalManager* hm = HospitalManager::getInstance();
    
    auto m = std::make_shared<Medic>("TEST_MED1", "Doc", "Strange", "1700101000000", "M", "01.01.1970",
                                    "0733333333", "doc@spital.ro", "New York", "2020-01-01", 15000,
                                    "Chirurgie", GradProfesional::PRIMAR, "P999");
    hm->adaugaMedic(m);
    
    auto found = hm->gasesteMedic("TEST_MED1");
    assert(found != nullptr);
    assert(found->getNume() == "Doc");
    
    std::cout << "testMedicCRUD passed!\n";
}

void testProgramareLogic() {
    std::cout << "Running testProgramareLogic...\n";
    HospitalManager* hm = HospitalManager::getInstance();
    
    // Clear existing programari for a clean test if possible, but since it's a singleton we just add new ones
    auto prog1 = std::make_shared<Programare>("PROG1", "TEST_PAC1", "TEST_MED1", "2026-10-10", "10:00", "CAB1", "Consultatie");
    hm->adaugaProgramare(prog1);
    
    // Test overlap
    auto prog2 = std::make_shared<Programare>("PROG2", "TEST_PAC1", "TEST_MED1", "2026-10-10", "10:00", "CAB1", "Alta consultatie");
    bool overlapCaught = false;
    try {
        hm->adaugaProgramare(prog2);
    } catch (const ProgramareSuperupusaException& e) {
        overlapCaught = true;
    }
    assert(overlapCaught);
    
    // Test wait time estimation
    auto prog3 = std::make_shared<Programare>("PROG3", "TEST_PAC2", "TEST_MED1", "2026-10-10", "10:20", "CAB1", "Alta consultatie");
    hm->adaugaProgramare(prog3);
    
    // For prog3 (10:20), there is 1 active programare before it (prog1 at 10:00)
    // Actually estimareTimpAsteptare takes (idMedic, data, ora)
    int waitTime = hm->estimareTimpAsteptare("TEST_MED1", "2026-10-10", "10:20");
    // prog1 is at 10:00, which is < 10:20. So count = 1. waitTime = 1 * 20 = 20.
    assert(waitTime == 20);
    
    std::cout << "testProgramareLogic passed!\n";
}

int main() {
    std::cout << "Starting Tests...\n";
    
    try {
        testHospitalManagerSingleton();
        testPacientCRUD();
        testMedicCRUD();
    testProgramareLogic();
    
    std::cout << "Running testStaffRemoval...\n";
    HospitalManager* hm = HospitalManager::getInstance();
    auto m_rem = std::make_shared<Medic>("REM_MED1", "Bye", "Bye", "1900101000000", "M", "01.01.1990", "0", "0", "0", "2020-01-01", 1000, "X", GradProfesional::REZIDENT, "X");
    hm->adaugaMedic(m_rem);
    assert(hm->gasesteMedic("REM_MED1") != nullptr);
    hm->eliminaMedic("REM_MED1");
    assert(hm->gasesteMedic("REM_MED1") == nullptr);
    std::cout << "testStaffRemoval passed!\n";

    std::cout << "Running testUserManagement...\n";
    auto auth = hm->getAuthService();
    User newUser("test_user", "pass123", Rol::MEDIC, "MED1");
    assert(auth->adaugaUtilizator(newUser));
    assert(auth->gaseste("test_user") != nullptr);
    assert(auth->eliminaUtilizator("test_user"));
    assert(auth->gaseste("test_user") == nullptr);
    std::cout << "testUserManagement passed!\n";
        // Add more tests as needed
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }

    std::cout << "All tests passed successfully!\n";
    return 0;
}
