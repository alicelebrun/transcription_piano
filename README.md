# transcription_piano
Programme permettant de transcrire un morceau musical de piano en une liste de notes
# Auteur
Alice LEBRUN et Laure GRISEZ
# Prérecquis
cmake, SDL2, SLD_image, fftw3, compilateur C
# Installation
* cloner le dépot
git clone git@github.com:alicelebrun/transcription_piano.git
* aller dans le répertoire transcription_piano
cd transcription_piano
* créer un répertoire pour la compilation et s'y rendre
mkdir build && cd build
* configurer le logiciel en indiquant un répertoir d'installation, par exemple le sous-répertoir install du répertoir build
cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/install
* compiler et installer le logiciel
make install
# Utilisation
Dans le répertoire install se trouve le programme