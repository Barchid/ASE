#include "drive.h"
#include "hw.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    init();
    unsigned int cylinder = atoi(argv[1]);
    unsigned int sector = atoi(argv[2]);
    
    printf("Je vais écrire le secteur %d au cylindre %d\n", sector, cylinder);
    write_sector(cylinder,sector,(unsigned char *) argv[3]);
    printf("J'ai écrit le secteur %d au cylindre %d\n", sector, cylinder);
}
