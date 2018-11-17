#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"
#include "drive.h"
#include "current.h"

int main() {
    char choice;
    init();

    if(!load_mbr()) {
        puts("Disk not initialized. Continue [Y/N] ?");
        scanf("%c", &choice);

        if(choice == 'N') {
            exit(EXIT_SUCCESS);
        }
    }
    
    init_volume(load_current_volume());
    save_super();

    return 0;
}