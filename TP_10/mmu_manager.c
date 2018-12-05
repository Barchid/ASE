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
#include "hw_config.h"
#include "swap.h"

extern void user_process(); // pour jouer avec les matrices

// structure qui sert à manipuler la TLB
struct tlb_entry_s { 
	unsigned tlbe_rfu :8; // 8 bits qui servent a rien
	unsigned tlbe_vpage :12; // Numero de page virtuelle
	unsigned tlbe_ppage :8; // Numéro de page physique correspondante
	unsigned tlbe_xwr :3; // droit exécution|write|read 
	unsigned tlbe_access :1; // flag d'entrée utilisée ou non
};

// stucture pour relier un numéro de page virtuelle à un numéro de page physique
struct vm_mapping_s {
	unsigned vm_ppage: 8; // numéro de la page physique où on a mis la page virtuelle
	unsigned vm_mapped: 1; // booleen pour savoir si la la page virtuelle est en mémoire physique 
};

// tableau des pages virtuelles et leurs correspondances en page physique 
static struct vm_mapping_s vm_mapping[VM_PAGES];

// stucture pour relier un numéro de page physique à un numéro de page virtuelle
struct pm_mapping_s {
	unsigned pm_vpage: 12; // numéro de la page virtuelle qui est mise dans la page physique
	unsigned pm_mapped: 1; // booleen pour savoir si la la page physique est reliée à une page vrituelle
};

// tableau des pages physique et leurs correspondances en page virtuelle
static struct pm_mapping_s pm_mapping[PM_PAGES];

// il faut initialiser les tableaux (vm et pm_mapped à F)

// numéro de la prochaine page virtuelle à swapper de la mémoire physique
static int rr_ppage = 1;

//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
//------------------------------------------------


static void mmu_handler(void) {
	int vaddr;
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
	
	// calculer la page virtuelle 
	vpage = vaddr>>12 & 0xFFF; // 12 bits du milieu de vaddr;
	
	// regarder si la page est mappée en mémoire physique
	if(vm_mapping[vpage].vm_mapped) {
		// ici, ça veut dire qu'il ne manque que l'entrée dans la TLB
		
		// on l'ajoute
		tlbe.tlbe_vpage = vpage; // la page virtuelle qu'on veut
		tlbe.tlbe_ppage = vm_mapping[vpage].vm_ppage; // numéro de page dans disque
		tlbe.tlbe_xwr = 7;
		tlbe.tlbe_access = 1;
		
		_out(TLB_ADD_ENTRY, *((int*)(&tlbe))); // Ecrire dans TLB
		return; // on a fini
	}
	
	// ici, la page demandée n'est pas en mémoire physique
	
	// LIBERER une page physique (transfert mémoire + mise à jour TLB (del entry))
	
	// libérer la page physique d'avant SI elle existe
	if(pm_mapping[rr_ppage].pm_mapped) {
		store_to_swap(pm_mapping[rr_ppage].pm_vpage, rr_ppage);
		vm_mapping[pm_mapping[rr_ppage].pm_vpage].vm_mapped = 0; // on dit que 
	}
	
	// supprimer entrée d'avant dans la TLB pour la page qu'on a jarté
	// Construire tlb_entry_s
	tlbe.tlbe_vpage = 0; // osef
	tlbe.tlbe_ppage = rr_ppage;
	tlbe.tlbe_xwr = 7; // osef
	tlbe.tlbe_access = 1; //osef 
	_out(TLB_DEL_ENTRY, *((int*)(&tlbe))); // Écriture de la suppression
	//----------------------------------------------------------------------------
	
	
	// CHARGER la vpage en mémoire physique (transfert mémoire + mise à jour TLB (add entry))
	
	// charger en mémoire physique une copie de ce qu'il y a sur le disque 
	fetch_from_swap(vpage, rr_ppage);
	
	// mettre à jour le mapping des pages virtuelles
	vm_mapping[vpage].vm_mapped = 1; // on dit que vpage est mappée en mémoire physique
	vm_mapping[vpage].vm_ppage = rr_ppage; // mappée à la page rr_ppage en mémoire physique
	
	// mettre à jour le mapping des pages physiques
	pm_mapping[rr_ppage].pm_mapped = 1; // on dit que rr_ppage a une page virtuelle
	pm_mapping[rr_ppage].pm_vpage = vpage; // mappée à la page rr_ppage en mémoire physique
	
	// ecrire entrée dans TLB (vpage et rrpage)
	tlbe.tlbe_vpage = vpage; // la vpage à charger en TLB
	tlbe.tlbe_ppage = rr_ppage; // la correspondance physique
	tlbe.tlbe_xwr = 7; // osef
	tlbe.tlbe_access = 1; //osef 
	_out(TLB_ADD_ENTRY, *((int*)(&tlbe))); // Écriture de la suppression
	//----------------------------------------------------------------------------
	
	// mise à jour de rrpage
	rr_ppage++;
	if(rr_ppage == PM_PAGES) {
		rr_ppage = 1;
	}
}

int main() {
	unsigned int i;
	if(init_hardware(HARDWARE_INI) == 0) {
        exit(EXIT_FAILURE);
    }
	IRQVECTOR[MMU_IRQ] = mmu_handler;
	_mask(0x1001);

	// initialiser vm_mapping à FAUX partout
	for(i=0;i<VM_PAGES;i++) {
		vm_mapping[i].vm_mapped = 0;
	}

	// initialiser pm_mapping à FAUX partout
	for(i=0;i<PM_PAGES;i++) {
		pm_mapping[i].pm_mapped = 0;
	}

	/* user mode */
    user_process();
    return 0;
}