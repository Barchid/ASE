/* functions return non null value on error */

#include <stdio.h>
#include <stdint.h>
#include "swap.h"
#include "hardware.h"
#include "hw_config.h"

static FILE *swap_file = NULL;

static int
init_swap(void) 
{
    swap_file = fopen(".swap_file", "w+"); /* w+: create, read, write*/
    return swap_file == NULL;
}

int
store_to_swap(int vpage, int ppage) 
{
    if (swap_file == NULL)
        if (init_swap()) 
            return -2;
    if (fseek(swap_file, vpage << 12, SEEK_SET) == -1) 
        return -1;
    if (fwrite((void*)((ppage << 12) | (uintptr_t)physical_memory), 
               1, PAGE_SIZE, swap_file) == 0)
        return -1;
  return 0;
}

int 
fetch_from_swap(int vpage, int ppage) 
{
    if (swap_file == NULL)
        if (init_swap()) 
            return -2;
    if (fseek(swap_file, vpage << 12, SEEK_SET) == -1) 
        return -1;
    if (fread((void*)((ppage << 12) | (uintptr_t)physical_memory), 
	      1, PAGE_SIZE, swap_file) == 0)
        return -1;
    return 0;
}
