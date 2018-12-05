// stocker bloc vpage (dans disque) dans le bloc ppage de la mémoire physique 
int store_to_swap(int vpage, int ppage);

// stocker ppage (mémoire physique) et mettre à bloc de vpage (dans disque)
int fetch_from_swap(int vpage, int ppage);

// Page qui est dans la mémoire physique
static unsigned vpage_mapped = 0; // = 0 car il faut bien l'initialiser

// Faire une fonction init pour invoquer ça 
// Les deux fonctions sont obligatoires en mode maître !!


// stucture pour relier un numéro de page virtuelle à un numéro de page physique
struct vm_mapping_s {
	unsigned vm_ppage: 8; // numéro de la page physique où on a mis la page virtuelle
	unsigned vm_mapped: 1; // booleen pour savoir si la la page virtuelle est en mémoire physique 
};

// tableau des pages virtuelles et leurs correspondances en page physique 
static struct vm_maping_s vm_maping[VM_PAGES];

// stucture pour relier un numéro de page physique à un numéro de page virtuelle
struct pm_mapping_s {
	unsigned pm_vpage: 12; // numéro de la page virtuelle qui est mise dans la page physique
	unsigned pm_mapped: 1; // booleen pour savoir si la la page physique est reliée à une page vrituelle
};

// tableau des pages physique et leurs correspondances en page virtuelle
static struct pm_maping_s pm_maping[PM_PAGES];

// il faut initialiser les tableaux (vm et pm_mapped à F)

// numéro de la prochaine page virtuelle à swapper de la mémoire physique
static int rr_ppage = 1;

static void mmu_handler(void) {
	unsigned int vaddr;
	unsigned int vpage;
	struct tlb_entry_s tlbe;
	int addr;
	
	// RECHERCHER adresse fautive
	vaddr = _in(MMU_FAULT_ADDR);
	
	// VOIR si faultAddr est valide 
	// SI je ne suis pas une vraie adresse virtuelle
	if((vaddr & 0xFFFFFF) != ((int) virtual_memory)) {
		printf("Adresse virtuelle %d invalide\n", vaddr);
		return;
	}
	
	
	// Mettre la page de la mémoire physique dans le disque (en page 1)
	store_to_swap(vpage_mapped, 1);
	
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
	if(pm_mapping[rr_page].pm_mapped) {
		store_to_swap(pm_mapping[rr_page].pm_vpage, rr_page);
		vm_mapping[pm_mapping[rr_ppage].pm_vpage].vm_mapped = 0; // on dit que 
	}
	
	// supprimer entrée d'avant dans la TLB pour la page qu'on a jarté
	// Construire tlb_entry_s
	tlbe.tlbe_vpage = 0; // osef
	tlbe.tlbe_ppage = rr_page;
	tlbe.tlbe_xwr = 7; // osef
	tlbe.tlbe_access = 1; //osef 
	_out(TLB_DEL_ENTRY, *((int*)(&tlbe))); // Écriture de la suppression
	//----------------------------------------------------------------------------
	
	
	// CHARGER la vpage en mémoire physique (transfert mémoire + mise à jour TLB (add entry))
	
	// charger en mémoire physique une copie de ce qu'il y a sur le disque 
	fetch_from_swap(vage, rr_page);
	
	// mettre à jour le mapping des pages virtuelles
	vm_mapping[vpage].vm_mapped = 1; // on dit que vpage est mappée en mémoire physique
	vm_mapping[vpage].vm_ppage = rr_page; // mappée à la page rr_page en mémoire physique
	
	// mettre à jour le mapping des pages physiques
	pm_mapping[rr_page].pm_mapped = 1; // on dit que rr_page a une page virtuelle
	pm_mapping[rr_page].pm_vpage = vpage; // mappée à la page rr_page en mémoire physique
	
	// ecrire entrée dans TLB (vpage et rrpage)
	tlbe.tlbe_vpage = vpage; // la vpage à charger en TLB
	tlbe.tlbe_ppage = rr_page; // la correspondance physique
	tlbe.tlbe_xwr = 7; // osef
	tlbe.tlbe_access = 1; //osef 
	_out(TLB_ADD_ENTRY, *((int*)(&tlbe))); // Écriture de la suppression
	//----------------------------------------------------------------------------
	
	// mise à jour de rrpage
	rr_page++;
	if(rr_page == PM_PAGES) {
		rr_page = 1;
	}
}

static void mmu_handler(void) {
	unsigned int vaddr;
	unsigned int vpage;
	struct tlb_entry_s tlbe;
	int addr;
	
	// RECHERCHER adresse fautive
	vaddr = _in(MMU_FAULT_ADDR);
	
	// VOIR si faultAddr est valide 
	// SI je ne suis pas une vraie adresse virtuelle
	if((vaddr & 0xFFFFFF) != ((int) virtual_memory)) {
		printf("Adresse virtuelle %d invalide\n", vaddr);
		return;
	}
	
	
	// Mettre la page de la mémoire physique dans le disque (en page 1)
	store_to_swap(vpage_mapped, 1);
	
	// calculer vpage de l'adresse et placer dans la page en mémoire physique
	vpage_mapped = vpage = vaddr>>12 & 0xFFF; // 12 bits du milieu de vaddr
	
	// aller chercher l'adresse fautive dans le disque 
	fetch_from_swap(vpage_mapped, 1);
	
	// SUPPRIMER les entrées de la TLB de page physique 1
	// Construire tlb_entry_s
	tlbe.tlbe_vpage = 0; // osef
	tlbe.tlbe_ppage = 1;
	tlbe.tlbe_xwr = 7; // osef
	tlbe.tlbe_access = 1; //osef 
	
	// détruire l'entrée de ppage = 1 dans la TLB
	_out(TLB_DEL_ENTRY, tlbe);
	
	// AJOUTER l'entrée de ma nouvelle entrée en disque page 1
	tlbe.tlbe_vpage = vpage_mapped; // la nouvelle page mappée en mémoire physique (qu'on vulait de base)
	tlbe.tlbe_ppage = 1; // numéro de page dans disque
	tlbe.tlbe_xwr = 7; // osef
	tlbe.tlbe_access = 1; //osef 
	
	// ECRIRE tlb_entry_s dans TLB
	// on ne peut pas écrire directement la structure alors qu'on veut un int
	// on doit dire au compilateur d'aller se faire voir ( ça se fait avec les cast chelous)
	_out(TLB_ADD_ENTRY, *((int*)(&tlbe))); //Dans mémoire, _out fait sur 32 bits
}