#include "drive.h"
#include "hw.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {    
    init();
    unsigned int cylinder = atoi(argv[1]);
    unsigned int sector = atoi(argv[2]);
    unsigned char buffer[HDA_SECTORSIZE];
    
    printf("Je vais lire le secteur %d au cylindre %d\n", sector, cylinder);

    read_sector(cylinder,sector, buffer);
    dump(buffer, HDA_SECTORSIZE, 1, 1);
}

