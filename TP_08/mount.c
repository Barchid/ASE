/* ------------------------------
   $Id: mount.c,v 1.2 2009/11/17 16:20:54 marquet Exp $
   ------------------------------------------------------------

   Initialization and finalization
   Philippe Marquet, Nov 2009
   
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include "hardware.h"
#include "hw.h"
#include "tools.h"

#include "mbr.h"
#include "drive.h"
#include "bloc.h"

/* load super bloc of the $current_vol
   set current_vol accordingly */
static void
load_current_vol ()
{
    char* current_vol_str;
    int status;
    
    current_vol_str = getenv("current_vol");
    ffatal(current_vol_str != NULL, "no definition of $current_vol"); 

    errno = 0;
    current_vol = strtol(current_vol_str, NULL, 10);
    ffatal(!errno, "bad value of $current_vol %s", current_vol_str);
    
    status = load_super(current_vol);
    ffatal(!status, "unable to load super of vol %d", current_vol);
}

/* return hw_config filename */
static char *
get_hw_config ()
{
    char* hw_config;

    hw_config = getenv("HW_CONFIG");
    return hw_config ? hw_config : HW_INI;
}

static void
emptyIT()
{
    return;
}

/* ------------------------------
   Initialization and finalization fucntions
   ------------------------------------------------------------*/
void
mount()
{
    char *hw_config;
    int status, i; 

    /* Hardware initialization */
    hw_config = get_hw_config();
    status = init_hardware(hw_config);
    ffatal(status, "error in hardware initialization with %s\n", hw_config);

    /* Interrupt handlers */
    for(i=0; i<16; i++)
	IRQVECTOR[i] = emptyIT;
    
    /* Allows all IT */
    _mask(1);

    /* Load MBR and current volume */
    load_mbr();
    load_current_vol();
}

void
umount()
{
    /* save current volume super bloc */
    save_super();

    /* bye */
}
 

