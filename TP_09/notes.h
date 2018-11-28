// structure qui sert à manipuler la TLB
struct tlb_entry_s { 
	unsigned tlbe_rfu :8; // 8 bits qui servent a rien
	unsigned tlbe_vpage :12; // Numero de page virtuelle
	unsigned tlbe_ppage :8; // Numéro de page physique correspondante
	unsigned tlbe_xwr :3; // droit exécution|write|read 
	unsigned tlbe_access :1; // flag d'entrée utilisée ou non
};

static int ppage_of_vpage(int process, unsigned vpage);