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
Le programme se trouve dans le répertoire d'installation. Pour l'utiliser, il suffit de lui passer le nom du fichier à analyser en argument:

./transcription_piano Sons/La.wav

# Limitations
Le programme implémente un algorithme de transcription très rudimentaire, qui est limité aux morceaux de piano dans lesquels une seule note est jouée à chaque instant. Le fichier doit être au format WAV 16 bits signé little endian, en mono ou stéréo (auquel cas les deux cannaux sont moyennés automatiquement). Le réglage de l'algorithme de transcription est réglé pour donner les meilleures performances pour une fréquence d'échantillonnage supérieure ou égale à 44100Hz.
