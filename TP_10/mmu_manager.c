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
#include <stdlib.h>
#include "hardware.h"
#include "mi_syscall.h"
#include "mi_user.h"
#include "hw_config.h"

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

 // récupérer la page physique à partir de la page virtuelle et du process
 // ON SUPPOSE que N doit être plus petit que 255
 static int ppage_of_vpage(int process, unsigned vpage) {
	 // Le processus a droit à accédder à page de 0 à N/2-1 (car il a N/2 pages)
	 if(vpage > N/2-1) {
		 return -1;
	 }
	 
	 // SI je suis le process 0
	 if(process == 0) {
		 // Quand je veux vpage 0, je dois retourner page physique 1, etc
		 // En fait, je saute juste la page 0
		 return vpage + 1;
	 }
	 // SI je suis le process 1
	 else if(process == 1){
		 return vpage + N/2 + 1;
	 }
	 // Je suis un numéro erroné
	 else {
		 return -1;
	 }
 }

static void mmu_handler(void) {
	unsigned int vaddr;
	unsigned int vpage;
	struct tlb_entry_s tlbe;
	
	// RECHERCHER adresse fautive
	vaddr = _in(MMU_FAULT_ADDR);
	
	// VOIR si faultAddr est valide 
	// SI je ne suis pas une vraie adresse virtuelle
	if((vaddr & 0xFFFFFF) == ((int) virtual_memory)) {
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
	tlbe.tlbe_xwr = 7;
	tlbe.tlbe_access = 1;
	
	// ECRIRE tlb_entry_s dans TLB
	// on ne peut pas écrire directement la structure alors qu'on veut un int
	// on doit dire au compilateur d'aller se faire voir ( ça se fait avec les cast chelous)
	_out(TLB_ADD_ENTRY, *((int*)(&tlbe))); //Dans mémoire, _out fait sur 32 bits
}

static void switch_to_process0(void) 
{
    current_process = 0;
    _out(MMU_CMD, MMU_RESET);
}

static void switch_to_process1(void) 
{
    current_process = 1;
    _out(MMU_CMD, MMU_RESET);
}

int main(int argc, char **argv) {
    if(init_hardware(HARDWARE_INI) == 0) {
        exit(EXIT_FAILURE);
    }

    IRQVECTOR[MMU_IRQ] = mmu_handler;
    IRQVECTOR[16] = switch_to_process0;
    IRQVECTOR[17] = switch_to_process1;
    _mask(0x1001);

    init();
}