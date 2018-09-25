# TP 01 ASE

## Compilation
Lancer la commande ```make``` compilera un exécutable "display_stack" et un autre "try_mul".

## Exécution

### try_mul
Lancer la commande ```make execNumbers```

### display_stack
Lancer la commande ```make displayStack```

## Explication pour display_stack
ESP est le registre qui pointe le sommet de pile (Stack Pointer).
EBP est le registre qui pointe la base de la pile (Base Pointer).

Lorsque l'on compare les valeurs de esp et ebp du main() par rapport aux valeurs de esp et ebp d'une fonction imbriquée, elles sont différentes car on change de contexte. En effet, à chaque appel de fonction, on ajoute une frame dans la pile d'exécution. Les valeurs des deux registres changent pour pointer vers la nouvelle frame en cours d'exécution.

Les variables locales et les paramètres ont des adresses comprises entre ebp et esp de la fonction courante car elles sont propres à une frame de la pile.

## Note importante

Suite à la nouvelle version du compilateur gcc, l'exécution en 32 bits de try_mul posait problème à cause de la gestion du code de try.c et throw.c.
Pour que cela fonctionne, il a été décidé d'utiliser plutôt la version 64 bits. Les changements pour cette version sont :

- Supprimer l'option "-m32" dans le Makefile
- Changer l'appel aux registres "esp" et "ebp" par "rsp" et "rbp" dans les instructions ´´´asm()´´´

**Cette solution a été expliquée par Monsieur GRIMAUD après le cours théorique d'ASE du 25/09/2018 de 13:30 à 15:00.**