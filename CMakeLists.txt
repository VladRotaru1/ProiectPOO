cmake_minimum_required(VERSION 3.16)
project(SpitalHIS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# All source files
set(SOURCES_LIB
    src/Persoana.cpp
    src/Pacient.cpp
    src/Angajat.cpp
    src/Medic.cpp
    src/Asistent.cpp
    src/PersonalAuxiliar.cpp
    src/Pat.cpp
    src/Salon.cpp
    src/Sectie.cpp
    src/Internare.cpp
    src/Programare.cpp
    src/Consultatie.cpp
    src/Reteta.cpp
    src/AnalizaLaborator.cpp
    src/AparaturaMedicala.cpp
    src/Produs.cpp
    src/User.cpp
    src/AuditLog.cpp
    src/AuthService.cpp
    src/HospitalManager.cpp
    src/FinanciarManager.cpp
    src/NotificationService.cpp
    src/MenuManager.cpp
)

add_executable(SpitalHIS src/main.cpp ${SOURCES_LIB})
target_include_directories(SpitalHIS PRIVATE src)

# Test executable
add_executable(Tests tests/test_main.cpp ${SOURCES_LIB})
target_include_directories(Tests PRIVATE src)
