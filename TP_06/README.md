# TP 06 ASE

## Compilation
Lancer la commande ``make compile`` permet de compiler trois fichiers exécutables : dvol, mkvol, vm.

### dvol
Exécutable qui liste les partitions du disque.

Usage : 
``./dvol`` 

### mkvol
Exécutable qui crée un volume.

Usage :
``./mkvol cylindre secteur nblock``

``cylindre`` est le numéro de cylindre du disque à partir duquel on crée la partition.
``secteur`` est le numéro de secteur du disque à partir duquel on crée la partition.
``nblock`` est le nombre de blocs de la partition à créer.

### vm
Exécutable interactif en ligne de commande pour tester l'application.

Usage :
``./vm``