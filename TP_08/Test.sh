#!/bin/bash

next_step() {
	echo -e "\nContinuer ?"
	read reponse
	clear
}

rm vdiskA.bin #Retirer le disque  d'avant
make clean #clean le projet
make # compiler les programmes
clear # enlever les lignes
export current_vol=1 # Le volume courant est 1
export HW_CONFIG="hw.h" # Pour aller chercher le fichier hardware qui contient des constantes

echo -e "le programme est compilé. Voulez-vous lancer ?"
read reponse
clear

echo -e "Creation du disque."
echo -e "Creer la partition 0 de 3 blocs en (0,1)\n"
./mkvol 0 1 3 # creer la partition n°00 de 3 blocs en cylindre = 0 et sector = 1
./dvol
next_step

echo -e "Creer la partition 1 de 50 blocs en (1,0)\n"
./mkvol 1 0 50 # creer la partition n°01 de 100 blocs en cyl=1, sec=0
./dvol
next_step

echo -e "Creer une partition sur le MBR (erreur)\n"
./mkvol 0 0 10
./dvol
next_step

echo -e "Creer une partition en (0,2) (erreur)\n"
./mkvol 0 2 10
./dvol
next_step

echo -e "Remplir le disque de partitions\n"
./mkvol 0 4 3
./mkvol 0 7 3
./mkvol 0 10 3
./mkvol 0 13 3
./mkvol 6 0 3
./mkvol 6 4 3
./dvol
next_step

echo -e "Ajouter une partition de trop (erreur)\n"
./mkvol 6 7 3
./dvol
next_step

echo -e "Supprimer une partition\n"
./vm
./dvol
next_step

echo -e "Rajouter une partition\n"
./mkvol 6 10 3

echo -e "Créer un file system sur partition 1 (le current volume est choisi comme état le 1)\n"
./mknfs # créer un file system sur la partition CURRENT_VOL (1)
next_step

echo -e "Etat de la partition 1\n"
./dfs
next_step

echo -e "Créer un fichier avec une phrase dedans\n"
./if_nfile < "LoremIpsum_Normal.txt"
next_step

echo -e "Copier le fichier d'avant\n"
./if_cfile 1
next_step

echo -e "Afficher le fichier copié\n"
./if_pfile 3
next_step

echo -e "Afficher le fichier original\n"
./if_pfile 1
next_step

echo -e "Supprimer la copie\n"
./if_dfile 3
next_step

echo -e "Essayer d'afficher la copie\n"
./if_pfile 3
next_step

echo -e "Supprimer l'original\n"
./if_dfile 1
next_step

echo -e "Essayer d'afficher l'original\n"
./if_pfile 1
next_step

echo -e "Ecrire un nouveau fichier\n"
./if_nfile
next_step

echo -e "Creer un ENORME fichier\n"
./if_nfile  < "LoremIpsum_Long.txt"
next_step

echo -e "Afficher l'énorme fichier\n"
./if_pfile 3
next_step

echo -e "Delete l'énorme fichier\n"
./if_dfile 3
next_step

echo -e "Essayer d'afficher l'énorme fichier\n"
./if_pfile 3
next_step

echo "FIN !!!!!"
