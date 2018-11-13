#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mbr.h"
#include "drive.h"
#include "current.h"

#define RANDOM_IT 5

int main() {
	char choice;
	unsigned int continueNew = 1;
	unsigned int cpt = 0;
    init();

	srand(time(NULL));
	
    if(!load_mbr()) {
        puts("Disk not initialized. Continue [Y/N] ?");
        scanf("%c", &choice);

        if(choice == 'N') {
            exit(EXIT_SUCCESS);
        }
    }
	
	init_volume(load_current_volume());
	
	// new block jusqu'à ce que le disque soit plein
	while(continueNew) {
		continueNew = new_block();
	}
    
	// vérifier que le disque est plein
	occupancy_free_rate();
	
	// itérer 5 fois sur freeblock en aléatoire
	for(i = 0;i < RANDOM_IT; i++) {
		free_bloc(rand(1, get_vol_size())); // TODO : à tester
	}
	
	// afficher le statut du disque
	list_vol();
	
	// allouer des blocs tant que le disque n'est pas plein, 
	continueNew = 1;
	while(continueNew) {
		continueNew = new_block();
		cpt++;
	}
	cpt--;
	// retourner nombre de blocs ayant pu être alloués
	printf("Nombre de blocs écrits à la fin : %d\n", cpt);
    save_super();
	return 0;
}