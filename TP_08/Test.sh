rm vdiskA.bin #Retirer le disque  d'avant
make clean #clean le projet
make # compiler les programmes
clear # enlever les lignes
export current_vol=1 # Le volume courant est 1
export HW_CONFIG="hw.h" # Pour aller chercher le fichier hardware qui contient des constantes

echo "Creer la partition 0 de 20 blocs en (0,1)\n"
./mkvol 0 1 10 # creer la partition n°00 de 20 blocs en cylindre = 0 et sector = 1

echo "\nCreer la partition 1 de 100 blocs en (1,0)\n"
./mkvol 1 0 100 # creer la partition n°01 de 100 blocs en cyl=1, sec=0

echo "Afficher état du disque"
./dvol #afficher l'état du disque (les volumes)

echo "\nCréer un file system sur partition 1"
./mknfs # créer un file system sur la partition CURRENT_VOL (1)

echo "\nEtat de la partition 1"
./dfs