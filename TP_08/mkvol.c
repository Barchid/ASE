#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"
#include "drive.h"

int main(int argc, char **argv) {
    unsigned int cylinder = atoi(argv[1]);
    unsigned int sector = atoi(argv[2]);
    unsigned int n_block = atoi(argv[3]);
    enum vol_type_e vol_type = VBASE;
    char choice;

    init();

    if(!load_mbr()) {
        puts("Disk not initialized. Continue [Y/N] ?");
        scanf("%c", &choice);

        if(choice == 'N') {
            exit(EXIT_SUCCESS);
        }
    }

    if(!create_vol(cylinder, sector, n_block, vol_type)) {
        perror("Create volume failed.\n");
        exit(EXIT_FAILURE);
    }

    puts("Create volume success.");
    save_mbr();
}