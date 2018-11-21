#include "drive.h"
#include "hw.h"
#include "mbr.h"
#include "bloc.h"
#include <assert.h>
#include <stdio.h>

// On garde en mémoire le super bloc (bloc 0)
static struct super_s super;

// Initialiser le superbloc d'un volume
void init_volume(unsigned int vol) {
	struct freeb_s fb;
	
	// Vérifier que le MBR est initialisé
	assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que c'est un bon numéro de partition
	assert(vol < MAX_VOL);
	
	// assert correspond à un volume qui a été initialisé
	assert(mbr.mbr_vols[vol].vol_type == VBASE);
	// Le volume doit au moins avoir une taille de 2 blocs (sinon, super bloc occupe toute la place)
	assert(mbr.mbr_vols[vol].vol_n_sectors > 1);
	
	// Initialiser le super à écrire
	super.super_magic = SUPER_MAGIC;
	super.super_first_free = 1; // Le prochain bloc libre dans un nouveau volume = bloc 1 (tout le monde libre)
	
	// Initialiser le premier bloc libre
	fb.fb_n_blocks = mbr.mbr_vols[vol].vol_n_sectors - 1; // dernier bloc de l'ensemble de blocs libres = dernier bloc
	fb.fb_next = BLOCKEND;
	
	// Ecrire super dans block 0
	write_bloc_size(vol, SUPER, sizeof(struct super_s), (unsigned char*) &super);
	
	// Ecrire fb 
	write_bloc_size(vol, super.super_first_free, sizeof(struct freeb_s), (unsigned char*) &fb);
}

// Charger en mémoire le super block du volume
int load_super(unsigned int vol) {
	// Verifier que le volume est ok
	assert(vol < MAX_VOL);
	
	// Vérifier que mbr est initialisé
	assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que le volume est bien VBASE
	assert(mbr.mbr_vols[vol].vol_type == VBASE);

	// Lire le super block du volume demandé
	read_bloc_size(vol, SUPER, sizeof(struct super_s), (unsigned char *) &super);
	
    // vérifier que le super a été initialisé
    assert(super.super_magic == SUPER_MAGIC);

    current_vol = vol;
	return 0;
}

// Ecrire le super bloc en mémoire dans la partition
void save_super() {
	assert(super.super_magic == SUPER_MAGIC);
	
	write_bloc_size(current_vol, SUPER, sizeof(struct super_s), (unsigned char*) &super);
}

// renvoyer le numéro de bloc libre suivant
// et l'enlever de la liste des free blocks
unsigned int new_bloc() {
	struct freeb_s fb;
	unsigned int new_block;
	
	//  vérifier le super initialisé
	assert(super.super_magic == SUPER_MAGIC);
	
	// vérifier qu'il reste un bloc libre 
	if(super.super_first_free == BLOCKEND) {
		return 0; // retourne 0 quand le volumme est plein
	}
	
	// mémoriser le numéro de bloc à retourner
	new_block = super.super_first_free;
	
	// charger le premier freeb
	read_bloc_size(current_vol, super.super_first_free, sizeof(struct freeb_s), (unsigned char*) &fb);
	
	// Si l'ensemble libre ne contient plus qu'un seul élément
	if(fb.fb_n_blocks == 1)  {
		super.super_first_free = fb.fb_next;
	}
	else {
		fb.fb_n_blocks--; // On a un bloc en moins du coup
		write_bloc_size(current_vol, new_block + 1, sizeof(struct freeb_s), (unsigned char*) &fb); // écrire dans le fb suivant les nouvelles valeurs
		super.super_first_free++; // le first free bouge puisque 
	}
	
	// Sauvegarder superbloc quand on 
	save_super();
	
	return new_block;
}

// Liberer le bloc de numero bloc
void free_bloc(unsigned int bloc) {
    struct freeb_s fb;

	//  vérifier le super initialisé
	assert(super.super_magic == SUPER_MAGIC);
	
	// Vérifier que mbr est initialisé
	assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que le volume est bien VBASE
	assert(mbr.mbr_vols[current_vol].vol_type == VBASE);
	
	// Vérifier qu'on ne free pas le super
	assert(bloc > 0);

    // TODO : Vérifier que bloc n'est pas déjà free (parcourir les blocs libres)
	
    // instancier un nouveau free block
    // On crée une nouvelle liste de blocs free avec un seul élément dedans 
    // et on la chaîne en tête de la liste du super.
    fb.fb_n_blocks = 1;
    fb.fb_next = super.super_first_free;
    write_bloc_size(current_vol, bloc, sizeof(struct freeb_s), (unsigned char*) &fb); // écrire dans le fb suivant les nouvelles valeurs

    // inclure le nouveau block free dans super (ce sera mon premier block)
    super.super_first_free = bloc;
    save_super();
}

void occupancy_free_rate(){
    int n_free_blocks = 0;
    int i = super.super_first_free;
    struct freeb_s fb;

    assert(current_vol < MAX_VOL);
    assert(super.super_magic == SUPER_MAGIC);

    while(i != BLOCKEND){
        read_bloc_size(current_vol, i, sizeof(struct freeb_s), (unsigned char*) &fb);
        n_free_blocks += fb.fb_n_blocks;
        i = fb.fb_next;
    }

    printf("Free blocks / Total blocks of current volume : %d/%d\n", n_free_blocks, mbr.mbr_vols[current_vol].vol_n_sectors);
    printf("Occupancy free rate of current volume : %0.2f %% \n", (n_free_blocks/(float) (mbr.mbr_vols[current_vol].vol_n_sectors) * 100));
}

unsigned int get_vol_size(unsigned int vol) {
	assert(vol < MAX_VOL);
	return mbr.mbr_vols[vol].vol_n_sectors;
}