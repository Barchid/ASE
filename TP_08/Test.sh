#!/bin/bash

next_step() {
	echo "Continuer ?"
	read reponse
	clear
}

rm vdiskA.bin #Retirer le disque  d'avant
make clean #clean le projet
make # compiler les programmes
clear # enlever les lignes
export current_vol=1 # Le volume courant est 1
export HW_CONFIG="hw.h" # Pour aller chercher le fichier hardware qui contient des constantes

echo "le programme est compilé. Voulez-vous lancer ?"
read reponse
clear

echo "Creation du disque."
./dfs
next_step

echo "Creer la partition 0 de 3 blocs en (0,1)\n"
./mkvol 0 1 3 # creer la partition n°00 de 20 blocs en cylindre = 0 et sector = 1
./dvol
next_step

echo "Creer la partition 1 de 100 blocs en (1,0)\n"
./mkvol 1 0 50 # creer la partition n°01 de 100 blocs en cyl=1, sec=0
./dvol
next_step

echo "Creer une partition sur le MBR (erreur)\n"
./mkvol 0 0 10
./dvol
next_step

echo "Creer une partition en (0,2) (erreur)\n"
./mkvol 0 2 10
./dvol
next_step

echo "Remplir le disque de partitions\n"
./mkvol 0 4 3
./mkvol 0 7 3
./mkvol 0 10 3
./mkvol 0 13 3
./mkvol 6 0 3
./dvol
next_step

echo "Ajouter une partition de trop\n"
./mkvol 6 4 3
./dvol
next_step

echo "Supprimer une partition\n"
./vm
./dvol
next_step

echo "Supprimer une partition\n"
./vm
./dvol
next_step

echo "Créer un file system sur partition 1\n"
./mknfs # créer un file system sur la partition CURRENT_VOL (1)
next_step

echo "Etat de la partition 1\n"
./dfs
next_step

echo "Créer un fichier avec une phrase dedans\n"
./if_nfile 1
next_step

echo "Copier le fichier d'avant\n"
./if_cfile 1
next_step

echo "Afficher le fichier copié\n"
./if_pfile 1
next_step

echo "Afficher le fichier original\n"
./if_pfile 1
next_step

echo "Supprimer la copie\n"
./if_dfile 1
next_step

echo "Supprimer l'original\n"
./if_dfile 1
next_step

echo "Creer un ENORME fichier\n"
./if_nfile 5 < ".txt"
next_step

echo "Afficher l'énorme fichier\n"
./if_pfile 5
next_step