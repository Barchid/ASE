#include "drive.h"
#include "hw.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {    
    init();
    unsigned int cylinder = atoi(argv[1]);
    unsigned int sector = atoi(argv[2]);
    unsigned int nsector = atoi(argv[3]);
    int value = atoi(argv[4]);
    puts("Je vais formaté le disque");
    format_sector(cylinder, sector, nsector, value);
    puts("J'ai formaté le disque");
}

