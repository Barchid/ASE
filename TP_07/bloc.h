#define SUPER_MAGIC 0xCAFEBABE //
#define SUPERNAMESIZE 32
#define BLOCKEND 0
#define SUPER 0

// structure définissant le bloc super
struct super_s {
	unsigned int super_magic; // sert à vérifier si la structure super a été initialisée
	unsigned int super_s_serie; // numéro de série
	char super_s_name[SUPERNAMESIZE]; // nom du super bloc
	unsigned int super_first_free; // Prochain numéro de bloc du premier ensemble de blocs libres
	unsigned int super_first_inode; // numéro du premier inode
};

// structure d'un bloc libre
struct freeb_s {
	unsigned int fb_n_blocks; // Nombre de blocs consécutifs libres
	unsigned int fb_next; // Numéro de bloc à partir duquel commence le prochain ensemble de blocs libres
};

// fonctions de gestion des blocs libres

void init_volume(unsigned int vol);

int load_super(unsigned int vol);

void save_super();

unsigned int new_bloc();

void free_bloc(unsigned int bloc);

void occupancy_free_rate();

unsigned int get_vol_size(unsigned int vol);