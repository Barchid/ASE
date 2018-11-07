#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "hw.h"
#include "drive.h"
#include "hardware.h"

// Vérifie si le système supporte le disque utilisé
void checkDisk(){
	int nbCycl, nbSec, tailleSec;

	_out(HDA_CMDREG, CMD_DSKNFO); // Lancer la commande DISKINFO (dans registre de commande)

	// Récupérer la géométrie du disque sur les data regs
	nbCycl = (_in(HDA_DATAREGS) << 8) + _in(HDA_DATAREGS + 1);
	nbSec = (_in(HDA_DATAREGS + 2) << 8) + _in(HDA_DATAREGS + 3);
	tailleSec = (_in(HDA_DATAREGS + 4) << 8) + _in(HDA_DATAREGS + 5);
	
	// Check
	assert(HDA_MAXCYLINDER == nbCycl);
	assert(HDA_MAXSECTOR == nbSec);
	assert(HDA_SECTORSIZE == tailleSec);
}

// fonction appelée par interruption (vide car on ne s'en occupe pas)
static void empty_it(void)
{
    return;
}

// Initialiser le disque
void init()
{
    unsigned int i;
    /* init hardware */
    if (init_hardware(HW_INI) == 0)
    {
        fprintf(stderr, "Error in hardware initialization\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 16; i++)
        IRQVECTOR[i] = empty_it;

    /* allows all IT */
    _mask(1);

	// Check if disk is supported
	checkDisk();
}

/* dump buffer to stdout,
   and octal dump if octal_dump; an ascii dump if ascii_dump! */
void dump(unsigned char *buffer, unsigned int buffer_size, int ascii_dump, int octal_dump) {
    int i,j;
    
    for (i=0; i<buffer_size; i+=16) {
	/* offset */
	printf("%.8o",i);

	/* octal dump */
	if (octal_dump) {
	    for(j=0; j<8; j++)
		printf(" %.2x", buffer[i+j]);
	    printf(" - ");
	    
	    for( ; j<16; j++)
		printf(" %.2x", buffer[i+j]);
	    
	    printf("\n");
	}
	/* ascii dump */
	if (ascii_dump) {
	    printf("%8c", ' ');
	    
	    for(j=0; j<8; j++)
		printf(" %1c ", isprint(buffer[i+j])?buffer[i+j]:' ');
	    printf(" - ");
	    
	    for( ; j<16; j++)
		printf(" %1c ", isprint(buffer[i+j])?buffer[i+j]:' ');
	    
	    printf("\n");
	}
	
    }
}

// Déplace la tête de lecture jusqu'au secteur précisé par cylinder, numéro de cylindre et sector, numéro de secteur
void goToSector(unsigned int cylinder, unsigned int sector){

	// Vérifier que cylindre et sector sont compris dans le disque.
	assert(cylinder < HDA_MAXCYLINDER && sector < HDA_MAXSECTOR);

	// octet de poids fort du numéro de cylindre = c>>8&0xFF !!!!!!!!IMPORTANT CAR cyl DE BASE FAIT 32 BITS ET PAS 16
	// octet de poids faible du numéro de cylindre = c&0xFF
	
	// octet de poids fort numéro de secteur =  s>>8&0xFF
	// octet de poids faible numéro de secteur = s&0xFF
	
	// Mettre le numéro de cylindre dans les data reg
	_out(HDA_DATAREGS, (cylinder >> 8) & 0xFF); // poids fort
	_out(HDA_DATAREGS + 1, cylinder & 0xFF); // poids faible
	

	// Mettre le numéro de secteru dans data reg 
	_out(HDA_DATAREGS + 2, (sector >> 8) & 0xFF); // poids fort 
	_out(HDA_DATAREGS + 3, (sector  & 0xFF)); // poids faible
	

	// Lancer la commande seek (dans registre de commande)
	_out(HDA_CMDREG, CMD_SEEK); 
	

	// Attendre la fin du SEEK
	_sleep(HDA_IRQ);
}


// Lit le contenu d'un secteur
void read_sector(unsigned int cylinder, unsigned int sector, unsigned char *buffer) {
	// on voit parfois faire des I/O sur des choses qui sont moins grandes qu'un secteur
	// Du coup je ne pourrai pas écrire tout le secteur dans un buffer plus petit, ça ne marche pas
	// Je dois faire un buffer de la taille HDA_SECTORSIZE 
	// Ca fait : Disque -> MASTERBUFFER -> buffer de SECTORSIZE ->

	read_sector_size(cylinder, sector, HDA_SECTORSIZE, buffer);
}



// Alternative
// Transfert de Disque -> MASTERBUFFER -> memcpy(buffer, MASTERBUFFER, size)
// read_sector va appeler read-sector_size
void read_sector_size(unsigned int cylinder, unsigned int sector, unsigned int size, unsigned char* buffer) {
	assert(size <= HDA_SECTORSIZE);

	// Aller au secteur
	goToSector(cylinder, sector);

	// Ecrire dans DATAREG le nombre de secteurs à lire (un seul ici)
	_out(HDA_DATAREGS, 0x00);
	_out(HDA_DATAREGS + 1, 0x01);

	// Lancer la commande READ
	_out(HDA_CMDREG, CMD_READ);

	// Attendre la réponse
	_sleep(HDA_IRQ);

	// Copier dans le buffer le contenu du MASTERBUFFER (ici que le secteur est lu)
	memcpy(buffer, MASTERBUFFER, size);
}



void write_sector(unsigned int cylinder, unsigned int sector, unsigned char *buffer){
	write_sector_size(cylinder, sector, HDA_SECTORSIZE,  buffer);
}


void write_sector_size(unsigned int cylinder, unsigned int sector, unsigned int size, unsigned char *buffer){

	assert(size <= HDA_SECTORSIZE);

	// Aller au secteur
	goToSector(cylinder, sector);

	// Copier dans le buffer le contenu du MASTERBUFFER (ici que le secteur est lu)
	memcpy(MASTERBUFFER, buffer, size);

	// Ecrire dans DATAREG le nombre de scteurs à lire (1)
	_out(HDA_DATAREGS, 0x00);
	_out(HDA_DATAREGS + 1, 0x01);

	// Lancer la commande WRITE
	_out(HDA_CMDREG, CMD_WRITE);

	// Attendre la réponse
	_sleep(HDA_IRQ);
}

// Format nsector secteurs à partir du secteur cylinder,sector en y mettant la valeur value
void format_sector(unsigned int cylinder, unsigned int sector, unsigned int nsector, int value) {
	int i;

	// Aller au secteur
	goToSector(cylinder, sector);

	// Ecrire dans DATAREG le nombre de secteurs à formater (nsector)
	_out(HDA_DATAREGS, (nsector >> 8) & 0xFF);
	_out(HDA_DATAREGS + 1, nsector & 0xFF);

	// Indiquer la valeur à placer dans les secteurs (32 bits)
	_out(HDA_DATAREGS + 2,  (value >> 24) & 0xFF);
	_out(HDA_DATAREGS + 3,  (value >> 16) & 0xFF);
	_out(HDA_DATAREGS + 4,  (value >> 8) & 0xFF);
	_out(HDA_DATAREGS + 5,  value & 0xFF);

	// Lancer la commande FORMAT
	_out(HDA_CMDREG, CMD_FORMAT);

	// Attendre la réponse pour chaque secteurs formaté (une interruption par secteur formaté)
	for(i = 0; i < nsector; i++)
		_sleep(HDA_IRQ);
}