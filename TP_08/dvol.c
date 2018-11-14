#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"
#include "drive.h"

int main(int argc, char **argv) {
    init();

    if(!load_mbr()) {
        puts("Disk not initialized.");
        exit(EXIT_FAILURE);
    }

    list_vol();
}