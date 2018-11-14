#include "drive.h"
#include "hw.h"
#include "mbr.h"
#include <assert.h>
#include <stdio.h>

#define MBR_MAGIC 0xCAFEBABE // Sert à vérifier l'initialisation du MBR
#define MAX_VOL 8 // nombre de volumes max

#define SUPER_MAGIC 0xCAFEBABE //
#define SUPERNAMESIZE 32
#define BLOCKEND 0
#define SUPER 0

// Structure décrivant une partition
struct vol_descr_s{
    unsigned int vol_first_sector; // numéro de secteur du premier secteur de la partition
    unsigned int vol_first_cylinder; // numéro de cylindre du premier secteur de la partition
    unsigned int vol_n_sectors; // nombre de secteurs de la partition
    enum vol_type_e vol_type; // type de la partition
};

// Structure qui décrit le MBR (Master Boot Record)
struct mbr_s {
    struct vol_descr_s mbr_vols[MAX_VOL]; // Tableau des partitions
    unsigned int mbr_magic; // Vérifie si le MBR a été correctement initialisé
};

struct super_s {
	unsigned int super_magic;
	unsigned int super_s_serie;
	char super_s_name[SUPERNAMESIZE];
	unsigned int super_first_free;
	unsigned int super_first_inode;
};

struct freeb_s {
	unsigned int fb_n_blocks;
	unsigned int fb_next;
};

static struct mbr_s mbr; // le MBR

// On garde en mémoire le super bloc
static struct super_s super;

unsigned int sector_of_block(unsigned int num_vol, unsigned int num_block);

unsigned int cylinder_of_block(unsigned int num_vol, unsigned int num_block);

// Charge le Master boot record dans la struct mbr
unsigned int load_mbr(){
    int i;

	// Vérifier que le premier secteur du disque peut accueillir la structure mbr (sinon, on ne peut pas utiliser le master boot record)
    assert(sizeof(struct mbr_s) <= HDA_SECTORSIZE);

	// Lit le secteur 0 dans mbr
    read_sector_size(0, 0, sizeof(struct mbr_s), (unsigned char *) &mbr);

	//Si le mbr a déjà été initialisé (= magic est initialisé)
    if(mbr.mbr_magic == MBR_MAGIC)
        return 1;
    
	// Sinon on initialise le MBR
    mbr.mbr_magic = MBR_MAGIC;

	// Toutes les partitions sont vides (n'existent pas)
    for(i = 0; i < MAX_VOL; i++)
        mbr.mbr_vols[i].vol_type = VNONE;
    
    return 0;
}

// Sauvegarde mbr dans le Master Boot Record
void save_mbr(){
	// Vérifier que le mbr a été initialisé
    assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que la structure mbr peut être écrite dans le Master Boot Record
    assert(sizeof(struct mbr_s) <= HDA_SECTORSIZE);
	
	// Écrire mbr dans le Master Boot Record
    write_sector_size(0, 0, sizeof(struct mbr_s), (unsigned char *) &mbr);
}

// Crée une partition
unsigned int create_vol(unsigned int cylinder, unsigned int sector, unsigned int size, enum vol_type_e vol_type) {
    int i, lastCylinder, lastCylinderVol, lastSector, lastSectorVol;
    
	// Vérifier que le mbr a été initialisé
    assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que la partition à ajouter n'est pas NONE
    assert(vol_type != VNONE);

    // Vérifier qu'on n'écrit pas sur le MBR
    assert(sector!=0 || cylinder !=0);
    
    // Vérifier last sector pas dans une partition
    for(i=0;i<MAX_VOL;i++) {
        if(mbr.mbr_vols[i].vol_type != VNONE) {
            lastCylinder = cylinder + (size / HDA_MAXSECTOR);
            lastCylinderVol = cylinder_of_block(i, mbr.mbr_vols[i].vol_n_sectors - 1);
            lastSector = sector + (size / HDA_MAXSECTOR);
            lastSectorVol = sector_of_block(i, mbr.mbr_vols[i].vol_n_sectors - 1);

            // Vérifier que le premier cylindre et secteur de la partition à créer n'est pas compris dans la partition[i]
            assert(
                cylinder < mbr.mbr_vols[i].vol_first_cylinder ||
                cylinder > lastCylinderVol ||
                (cylinder =  mbr.mbr_vols[i].vol_first_cylinder && sector < mbr.mbr_vols[i].vol_first_sector) ||
                (cylinder =  lastCylinderVol && sector > lastSectorVol)
            );

            // Vérifier que le dernier cylindre et secteur de la partition à créer n'est pas compris dans la partition[i]
            assert(
                lastCylinder < mbr.mbr_vols[i].vol_first_cylinder ||
                lastCylinder > lastCylinderVol ||
                (lastCylinder =  mbr.mbr_vols[i].vol_first_cylinder && sector < mbr.mbr_vols[i].vol_first_sector) || 
                (lastCylinder =  lastCylinderVol && lastSector > lastSectorVol)
            );
        }
    }
	
	// Parcourir les partitions
    for(i = 0; i < MAX_VOL; i++) {
		// Créer la partition à l'adresse de la première partition trouvée qui est NONE (inexistante)
        if(mbr.mbr_vols[i].vol_type == VNONE) {
            mbr.mbr_vols[i].vol_first_cylinder = cylinder;
            mbr.mbr_vols[i].vol_first_sector = sector;
            mbr.mbr_vols[i].vol_n_sectors = size;
            mbr.mbr_vols[i].vol_type = vol_type;

            return 1;
        }
    }

    return 0;
}

// supprimer le volume de numero num_vol
void delete_vol(unsigned int num_vol) {
    assert(num_vol <= MAX_VOL);
    mbr.mbr_vols[num_vol].vol_type = VNONE;
}

char* enumToString(enum vol_type_e vol_type){
    switch(vol_type){
        case VBASE :
            return "BASE";
        case VANX :
            return "ANNEXE";
        case VOTH :
            return "OTHER";
        default :
            return "NONE";
    }
}

// Renvoie le numéro de secteur du bloc logique d'une partition
unsigned int sector_of_block(unsigned int num_vol, unsigned int num_block) {
	// Vérifier que le mbr a été initialisé.
    assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que le numéro de la partition est correcte
    assert(num_vol < MAX_VOL);
	
	// Vérifier que la partition à utiliser existe
    assert(mbr.mbr_vols[num_vol].vol_type != VNONE);
	
	// Vérifier que le numéro de block logique existe dans la partition
    assert(num_block < mbr.mbr_vols[num_vol].vol_n_sectors);

	// return (s0 + n°block) % NSPC (Nombre de Secteur Par Cylindre)
    return (mbr.mbr_vols[num_vol].vol_first_sector + num_block) % HDA_MAXSECTOR;
}

// Renvoie le numéro de cylindre du bloc logique d'une partition
unsigned int cylinder_of_block(unsigned int num_vol, unsigned int num_block) {
	// Vérifier que le mbr a été initialisé.
    assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que le numéro de la partition est correcte
    assert(num_vol < MAX_VOL);
	
	// Vérifier que la partition à utiliser existe
    assert(mbr.mbr_vols[num_vol].vol_type != VNONE);
	
	// Vérifier que le numéro de block logique existe dans la partition
    assert(num_block < mbr.mbr_vols[num_vol].vol_n_sectors);

	// return c0 + (s0+n°block) / NSPC
    return mbr.mbr_vols[num_vol].vol_first_cylinder + (mbr.mbr_vols[num_vol].vol_first_sector + num_block) / HDA_MAXSECTOR;
}

// Liste les volumes existants (printf)
void list_vol() {
    int i;
    puts("num_vol\t(c0,s0)\tnbloc\t type_vol\t(cl,sl)");
    for(i = 0; i < MAX_VOL; i++){

        if(mbr.mbr_vols[i].vol_type != VNONE) {
            printf("%7d\t(%2d,%2d)\t%5d\t%9s\t(%2d,%2d)\n",
                i,
                mbr.mbr_vols[i].vol_first_cylinder, 
                mbr.mbr_vols[i].vol_first_sector, 
                mbr.mbr_vols[i].vol_n_sectors, 
                enumToString(mbr.mbr_vols[i].vol_type), 
                cylinder_of_block(i, mbr.mbr_vols[i].vol_n_sectors - 1), 
                sector_of_block(i, mbr.mbr_vols[i].vol_n_sectors - 1)
            );
        }
        else {
            printf("%d is none\n", i);
        }
    }
}

// Lit le block de la partition n° num_vol
void read_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer) {
    read_sector(cylinder_of_block(num_vol, num_block), sector_of_block(num_vol, num_block), buffer);
}

// Lit le block de la partition n° num_vol
void read_block_size(unsigned int num_vol, unsigned int num_block, unsigned int size, unsigned char *buffer) {
    read_sector_size(cylinder_of_block(num_vol, num_block), sector_of_block(num_vol, num_block), size, buffer);
}

// Écrit le block de la partition
void write_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer) {
    write_sector(cylinder_of_block(num_vol, num_block), sector_of_block(num_vol, num_block), buffer);
}

// Écrit le block de la partition
void write_block_size(unsigned int num_vol, unsigned int num_block, unsigned int size, unsigned char *buffer) {
    write_sector_size(cylinder_of_block(num_vol, num_block), sector_of_block(num_vol, num_block), size, buffer);
}

// Formate la partition num_vol
void format_vol(unsigned int num_vol, unsigned int value) {
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(num_vol < MAX_VOL);
    assert(mbr.mbr_vols[num_vol].vol_type != VNONE);
    format_sector(mbr.mbr_vols[num_vol].vol_first_cylinder, mbr.mbr_vols[num_vol].vol_first_sector, mbr.mbr_vols[num_vol].vol_n_sectors, value);
}

/**
 * 
 * Fonctions de gestion des blocs libre
 * 
 */

// Initialiser le superbloc d'un volume
void init_volume(unsigned int vol) {
	struct freeb_s fb;
	
	// Vérifier que le MBR est initialisé
	assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que c'est un bon numéro de partition
	assert(vol < MAX_VOL);
	
	// assert correspond à un volume qui a été initialisé
	assert(mbr.mbr_vols[vol].vol_type == VBASE);
	// Le volume doit au moins avoir une taille de 2 blocs
	assert(mbr.mbr_vols[vol].vol_n_sectors > 1);
	
	// Initialiser le super à écrire
	super.super_magic = SUPER_MAGIC;
	super.super_first_free = 1;
	
	// Initialiser le premier bloc libre
	fb.fb_n_blocks = mbr.mbr_vols[vol].vol_n_sectors - 1;
	fb.fb_next = BLOCKEND;
	
	// Ecrire super dans block 0
	write_block_size(vol, SUPER, sizeof(struct super_s), (unsigned char*) &super);
	
	// Ecrire fb 
	write_block_size(vol, super.super_first_free, sizeof(struct freeb_s), (unsigned char*) &fb);
}

// Charger en mémoire le super block du volume
int load_super(unsigned int vol) {
	// Verifier que le volume est ok
	assert(vol < MAX_VOL);
	
	// Vérifier que mbr est initialisé
	assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que le volume est bien VBASE
	assert(mbr.mbr_vols[vol].vol_type == VBASE);

	read_block_size(vol, SUPER, sizeof(struct super_s), (unsigned char *) &super);
	
    // vérifier que le super a été initialisé
    assert(super.super_magic == SUPER_MAGIC);

    current_vol = vol;

    return 1;
}

// Ecrire le super bloc en mémoire dans la partition
void save_super() {
	assert(super.super_magic == SUPER_MAGIC);
	
	write_block_size(current_vol, SUPER, sizeof(struct super_s), (unsigned char*) &super);
}

// renvoyer le numéro de bloc libre suivant
// et l'enlever de la liste des free blocks
unsigned int new_block() {
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
	read_block_size(current_vol, super.super_first_free, sizeof(struct freeb_s), (unsigned char*) &fb);
	
	// Si l'ensemble libre ne contient plus qu'un seul élément
	if(fb.fb_n_blocks == 1)  {
		super.super_first_free = fb.fb_next;
	}
	else {
		fb.fb_n_blocks--; // On a un bloc en moins du coup
		write_block_size(current_vol, new_block + 1, sizeof(struct freeb_s), (unsigned char*) &fb); // écrire dans le fb suivant les nouvelles valeurs
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
    write_block_size(current_vol, bloc, sizeof(struct freeb_s), (unsigned char*) &fb); // écrire dans le fb suivant les nouvelles valeurs

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
        read_block_size(current_vol, i, sizeof(struct freeb_s), (unsigned char*) &fb);
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

// libérer le tableau de numéro de blocs
void free_blocs(unsigned int blocs[], unsigned int size) {
	unsigned int i;
	for(i = 1; i < size; i++) {
		if(i!=0){ // NE JAMAIS libérer le bloc 0 (super bloc)
			free_bloc(i);
		}
	}
}
