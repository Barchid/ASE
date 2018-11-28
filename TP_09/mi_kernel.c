/* ------------------------------
   $Id: c.ins,v 1.1 2003-02-21 14:33:51 marquet Exp $
   ------------------------------------------------------------

   memory isolation
   Philippe Marquet, Nov 2017
   
*/

// 					Adresse virtuelle
//	 virtual memory			vpage			delta
// <---------------><-----------------><------------------>
//		8 bits				12 bits			12 bits

// 					Adresse physique
//		physical memory		ppage			delta
// <--------------------><----------><-------------------->
//		12 bits				8 bits			12 bits
#include <stdio.h>
#include <string.h>
#include "hardware.h"
#include "mi_syscall.h"

// structure qui sert à manipuler la TLB
struct tlb_entry_s { 
	unsigned tlbe_rfu :8; // 8 bits qui servent a rien
	unsigned tlbe_vpage :12; // Numero de page virtuelle
	unsigned tlbe_ppage :8; // Numéro de page physique correspondante
	unsigned tlbe_xwr :3; // droit exécution|write|read 
	unsigned tlbe_access :1; // flag d'entrée utilisée ou non
};

// numero du processus courant
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

int mmu_handler() {
	unsigned int vaddr;
	unsigned vpage: 12;
	struct tlb_entry_s tlbe;
	
	// RECHERCHER adresse fautive
	vaddr = _in(MMU_FAULT_ADDRESS);
	
	// VOIR si faultAddr est valide 
	// SI je ne suis pas une vraie adresse virtuelle
	if(virtual_memory < vaddr || vaddr >= virtual_memory + VM_SIZE) {
		printf("Adresse virtuelle %d invalide\n", vaddr);
		return;
	}
	
	// calculer vpage 
	vpage = vaddr>>12 & 0xFFF; // 12 bits du milieu de vaddr
	
	// VERIFIER que vpage est < n/2
	if(vpage>=N/2) {
		printf("Page virtuelle demandee %d trop grande dans adresse demandée %d\n", vpage, vaddr);
	}
	// Construire tlb_entry_s
	tlbe.tlbe_vpage = vpage;
	tlbe.tlbe_ppage = ppage_of_vpage(current_process, vpage);
	tlbe.tlbe_rwx = 7;
	tlbe.tlbe_access = 1;
	
	// ECRIRE tlb_entry_s dans TLB
	// on ne peut pas écrire directement la structure alors qu'on veut un int
	// on doit dire au compilateur d'aller se faire voir ( ça se fait avec les cast chelous)
	_out(TLB_ADD_ENTRY, *((int*)(&tlbe))); //Dans mémoire, _out fait sur 32 bits
}

// initialise le contexte et les handlers d'interruptions
void init(void) {
    if(init_hardware(HW_INI) == 0) {
        exit(EXIT_FAILURE);
    }

    IRQVECTOR[MMU_IRQ] = mmu_handler;
    IRQVECTOR[16] = switch_to_process0;
    IRQVECTOR[17] = switch_to_process1;
    _mask(0x1001);
}