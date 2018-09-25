# TP 01 ASE

### Compilation
Lancer la commande ```make``` compilera un exécutable "display_stack" et un autre "try_mul"

### Note importante

Suite à la nouvelle version du compilateur gcc, l'exécution en 32 bits de try_mul posait problème à cause de la gestion du code de try.c et throw.c.
Pour que cela fonctionne, il a été décidé d'utiliser plutôt la version 64 bits. Les changements pour cette version sont :

- Supprimer l'option "-m32" dans le Makefile
- Changer l'appel aux registres "esp" et "ebp" par "rsp" et "rbp" dans les instructions ´´´asm()´´´

**Cette solution a été expliquée par Monsieur GRIMAUD après le cours théorique d'ASE du 25/09/2018 de 13:30 à 15:00.**