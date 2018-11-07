/* ------------------------------
   $Id: current.c,v 1.1 2009/10/30 10:15:19 marquet Exp $
   ------------------------------------------------------------

   Access to $CURRENT_VOLUME and $HW_CONFIG
   Philippe Marquet, Oct 2009
   
*/

#include <stdlib.h>
#include <errno.h>

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
