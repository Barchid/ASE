#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mbr.h"
#include "drive.h"
#include "bloc.h"
#include <errno.h>
#define RANDOM_IT 5

/* load super bloc of the $CURRENT_VOLUME
   set current_volume accordingly */
int
load_current_volume ()
{
    char* current_volume_str;
    int current_volume;
    
    current_volume_str = getenv("CURRENT_VOLUME");
    if (! current_volume_str)
        return EXIT_FAILURE;

    errno = 0;
    current_volume = strtol(current_volume_str, NULL, 10);
    if (errno)
        return EXIT_FAILURE;

    return current_volume;
}

unsigned int tabContainVal(unsigned int tab[], unsigned int val){
	unsigned int i;
	for(i=0;i<RANDOM_IT;i++) {
		if(tab[i] == val) {
			return 1;
		}
	}
	return 0;
}

int main() {
	char choice;
	unsigned int continueNew = 1;
	unsigned int cpt = 0;
	unsigned int i;
	unsigned int idx = 0;
	unsigned int vals[RANDOM_IT];
	
	int randomBlock;
	int current_vol = load_current_volume();
	
    init();

	srand(time(NULL));
	
    if(!load_mbr()) {
        puts("Disk not initialized. Continue [Y/N] ?");
        scanf("%c", &choice);

        if(choice == 'N') {
            exit(EXIT_SUCCESS);
        }
    }

	init_volume(current_vol);
	
	// new block jusqu'à ce que le disque soit plein
	while(continueNew) {
		continueNew = new_bloc();
		if(continueNew) printf("Bloc créé au bloc numéro : %d\n", continueNew);
	}
    
	// vérifier que le disque est plein
	occupancy_free_rate();
	
	// itérer 5 fois sur freeblock en aléatoire
	while(i < RANDOM_IT) {
		randomBlock = rand() % (get_vol_size(current_vol) - 1 + 1 - 1) + 1;
		if(tabContainVal(vals, randomBlock)) {
			vals[idx] = randomBlock;
			idx++;
			free_bloc(randomBlock);
			printf("Bloc libéré à l'indice : %d\n", randomBlock);
			i++;
		}
	}
	
	// afficher le statut du disque
	list_vol();
	
	// allouer des blocs tant que le disque n'est pas plein, 
	continueNew = 1;
	while(continueNew) {
		continueNew = new_bloc();
		if(continueNew) printf("Bloc créé au bloc numéro : %d\n", continueNew);
		cpt++;
	}
	cpt--;
	// retourner nombre de blocs ayant pu être alloués
	printf("Nombre de blocs écrits à la fin : %d\n", cpt);
    save_super();
	return 0;
}