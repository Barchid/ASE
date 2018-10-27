#include "drive.h"
#include "hw.h"
#include "mbr.h"
#include <assert.h>
#include <stdio.h>

#define MBR_MAGIC 0xCAFEBABE
#define MAX_VOL 8

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

static struct mbr_s mbr; // le MBR

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
    int i;
    
	// Vérifier que le mbr a été initialisé
    assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que la partition à ajouter n'est pas NONE
    assert(vol_type != VNONE);

    // Vérifier cylinder n'est pas dans une partition
    // Vérifier sector n'est pas dans une partition
    // Vérifier last cylinder pas dans une partition
    // Vérifier last sector pas dans une partition
    for(i=0;i<MAX_VOL;i++) {
        if(mbr.mbr_vols[i].vol_type != VNONE) {
            //assert(cylinder <= );
            //assert(sector <= );
            //assert(cylinder <= );
            //assert(cylinder <= );
            //s = (s0 + b) % NSPC
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

// Écrit le block de la partition
void write_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer) {
    write_sector(cylinder_of_block(num_vol, num_block), sector_of_block(num_vol, num_block), buffer);
}

// Formate la partition num_vol
void format_vol(unsigned int num_vol, unsigned int value) {
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(num_vol < MAX_VOL);
    assert(mbr.mbr_vols[num_vol].vol_type != VNONE);
    format_sector(mbr.mbr_vols[num_vol].vol_first_cylinder, mbr.mbr_vols[num_vol].vol_first_sector, mbr.mbr_vols[num_vol].vol_n_sectors, value);
}