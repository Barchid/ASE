/* ------------------------------
   $Id: c.ins,v 1.1 2003-02-21 14:33:51 marquet Exp $
   ------------------------------------------------------------

   memory isolation
   Philippe Marquet, Nov 2017
   
*/

#include "mi.h"

static int current_process;

static int 
sum(void *ptr) 
{
    int i;
    int sum = 0; 
    
    for(i = 0; i < PAGE_SIZE * N/2 ; i++)
        sum += ((char*)ptr)[i];
    return sum;
}

static void 
switch_to_process0(void) 
{
    current_process = 0;
    _out(MMU_CMD, MMU_RESET);
}

static void
switch_to_process1(void) 
{
    current_process = 1;
    _out(MMU_CMD, MMU_RESET);
}

int 
main(int argc, char **argv) 
{
    void *ptr;
    int res;

    ... /* init_hardware(); */
    IRQVECTOR[16] = switch_to_process0;
    IRQVECTOR[17] = switch_to_process1;
    _mask(0x1001);

    ptr = virtual_memory;

    _int(16);
    memset(ptr, 1, PAGE_SIZE * N/2);
    _int(17);
    memset(ptr, 3, PAGE_SIZE * N/2);

    _int(16);
    res = sum(ptr);
    printf("Resultat du processus 0 : %d\n",res);
    _int(17);
    res = sum(ptr);
    printf("Resultat processus 1 : %d\n",res);
}
