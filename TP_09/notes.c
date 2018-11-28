// 					Adresse virtuelle
//	 virtual memory			vpage			delta
// <---------------><-----------------><------------------>
//		8 bits				12 bits			12 bits

// 					Adresse physique
//		physical memory		ppage			delta
// <--------------------><------------><------------------>
//		12 bits				12 bits			12 bits


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
 
 // Numéro du processus courant
int current_process;

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
	_out(TLB_ADD_ENTRY, (unsigned char*) tlbe); //Dans mémoire, _out fait sur 32 bits
}