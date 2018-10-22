# TP 05 ASE

## Compilation
Lancer la commande ``make compile`` permet de compiler trois fichiers exécutables : dmps, frmt, adhoc.

### dmps
Exécutable qui lit un secteur du disque et l'affiche sur la sortie standard.

Usage : 
``./dmps cylindre secteur`` 

``cylindre`` est le numéro de cylindre du secteur à afficher.
``secteur`` est le numéro de secteur du secteur à afficher.

### frmt
Exécutable qui formate un nombre de secteurs donné à partir d'un secteur donné avec une valeur spécifiée.

Usage :
``./frmt cylindre secteur nsecteurs value``

``cylindre`` est le numéro de cylindre du secteur à partir duquel on formate le disque.
``secteur`` est le numéro de secteur du secteur à partir duquel on formate le disque.
``nsecteurs`` est le nombre de secteurs qui vont être formatés.
``value`` est la valeur à laquelle on va formater le disque.

### adhoc
Exécutable qui écrit une valeur spécifiée dans un secteur donné.

Usage :
``./adhoc cylindre secteur value``

``cylindre`` est le numéro de cylindre du secteur que l'on va écrire.
``secteur`` est le numéro de secteur du secteur que l'on va écrire.
``value`` est la valeur que l'on va écrire dans le secteur spécifié.