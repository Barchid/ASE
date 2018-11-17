#include "drive.h"
#include "hw.h"
#include "mbr.h"
#include <assert.h>
#include <stdio.h>

unsigned int sector_of_bloc(unsigned int vol, unsigned int nbloc);

unsigned int cylinder_of_bloc(unsigned int vol, unsigned int nbloc);

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
            lastCylinderVol = cylinder_of_bloc(i, mbr.mbr_vols[i].vol_n_sectors - 1);
            lastSector = sector + (size / HDA_MAXSECTOR);
            lastSectorVol = sector_of_bloc(i, mbr.mbr_vols[i].vol_n_sectors - 1);

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

// supprimer le volume de numero vol
void delete_vol(unsigned int vol) {
    assert(vol <= MAX_VOL);
	assert(mbr.mbr_magic == MBR_MAGIC);
    mbr.mbr_vols[vol].vol_type = VNONE;
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
unsigned int sector_of_bloc(unsigned int vol, unsigned int nbloc) {
	// Vérifier que le mbr a été initialisé.
    assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que le numéro de la partition est correcte
    assert(vol < MAX_VOL);
	
	// Vérifier que la partition à utiliser existe
    assert(mbr.mbr_vols[vol].vol_type != VNONE);
	
	// Vérifier que le numéro de block logique existe dans la partition
    assert(nbloc < mbr.mbr_vols[vol].vol_n_sectors);

	// return (s0 + n°block) % NSPC (Nombre de Secteur Par Cylindre)
    return (mbr.mbr_vols[vol].vol_first_sector + nbloc) % HDA_MAXSECTOR;
}

// Renvoie le numéro de cylindre du bloc logique d'une partition
unsigned int cylinder_of_bloc(unsigned int vol, unsigned int nbloc) {
	// Vérifier que le mbr a été initialisé.
    assert(mbr.mbr_magic == MBR_MAGIC);
	
	// Vérifier que le numéro de la partition est correcte
    assert(vol < MAX_VOL);
	
	// Vérifier que la partition à utiliser existe
    assert(mbr.mbr_vols[vol].vol_type != VNONE);
	
	// Vérifier que le numéro de block logique existe dans la partition
    assert(nbloc < mbr.mbr_vols[vol].vol_n_sectors);

	// return c0 + (s0+n°block) / NSPC
    return mbr.mbr_vols[vol].vol_first_cylinder + (mbr.mbr_vols[vol].vol_first_sector + nbloc) / HDA_MAXSECTOR;
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
                cylinder_of_bloc(i, mbr.mbr_vols[i].vol_n_sectors - 1), 
                sector_of_bloc(i, mbr.mbr_vols[i].vol_n_sectors - 1)
            );
        }
        else {
            printf("%d is none\n", i);
        }
    }
}

// Lit le block de la partition n° vol
void read_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer) {
    read_sector(cylinder_of_bloc(vol, nbloc), sector_of_bloc(vol, nbloc), buffer);
}

// Lit le block de la partition n° vol
void read_bloc_size(unsigned int vol, unsigned int nbloc, unsigned int size, unsigned char *buffer) {
    read_sector_size(cylinder_of_bloc(vol, nbloc), sector_of_bloc(vol, nbloc), size, buffer);
}

// Écrit le block de la partition
void write_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer) {
    write_sector(cylinder_of_bloc(vol, nbloc), sector_of_bloc(vol, nbloc), buffer);
}

// Écrit le block de la partition
void write_bloc_size(unsigned int vol, unsigned int nbloc, unsigned int size, unsigned char *buffer) {
    write_sector_size(cylinder_of_bloc(vol, nbloc), sector_of_bloc(vol, nbloc), size, buffer);
}

// Formate la partition vol
void format_vol(unsigned int vol, unsigned int value) {
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(vol < MAX_VOL);
    assert(mbr.mbr_vols[vol].vol_type != VNONE);
    format_sector(mbr.mbr_vols[vol].vol_first_cylinder, mbr.mbr_vols[vol].vol_first_sector, mbr.mbr_vols[vol].vol_n_sectors, value);
}

