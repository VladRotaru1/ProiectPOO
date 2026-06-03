#!/bin/bash
# Script utilitar pentru a lansa aplicatia SpitalHIS cu TUI

# Setam un terminal cu suport complet de culori (in cazul in care lipseste)
export TERM=xterm-256color

# Ne asiguram ca suntem in folderul build
cd "$(dirname "$0")/build" || {
    echo "Folderul build nu exista! Compilati mai intai aplicatia."
    exit 1
}

# Verificam daca executabilul exista
if [ ! -x ./SpitalHIS ]; then
    echo "Executabilul SpitalHIS nu a fost gasit in folderul build!"
    echo "Rulati: cd build && /opt/cmake-3.26/bin/cmake -DUSE_TUI=ON .. && make"
    exit 1
fi

echo "Se lanseaza SpitalHIS (TUI Mode)..."
./SpitalHIS
