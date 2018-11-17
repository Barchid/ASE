#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"
#include "drive.h"
#include "bloc.h"

/* load super bloc of the $CURRENT_VOLUME
   set current_volume accordingly */
int
load_current_volume ()
{
    char* current_volume_str;
    int current_volume;
    
    current_volume_str = getenv("CURRENT_VOLUME");
    if (! current_volume_str)
        return EXIT_FAILURE;

    errno = 0;
    current_volume = strtol(current_volume_str, NULL, 10);
    if (errno)
        return EXIT_FAILURE;

    return current_volume;
}

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

    list_vol();

    load_super(load_current_volume());
    occupancy_free_rate();
    return 0;
}