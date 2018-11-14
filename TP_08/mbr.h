#define SUPER_MAGIC 0xCAFEBABE //
#define SUPERNAMESIZE 32
#define BLOCKEND 0
#define SUPER 0

enum vol_type_e {VBASE, VANX, VOTH, VNONE};

// Mémoriser le numéro de volume courant pour pouvoir écrire dans save_super
static int current_vol;

unsigned int load_mbr();

void save_mbr();

unsigned int create_vol(unsigned int cylinder, unsigned int sector, unsigned int size, enum vol_type_e vol_type);

void delete_vol(unsigned int num_vol);

void list_vol();

void read_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer);

void read_block_size(unsigned int num_vol, unsigned int num_block, unsigned int size, unsigned char *buffer);

void write_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer);

void write_block_size(unsigned int num_vol, unsigned int num_block, unsigned int size, unsigned char *buffer);

void format_vol(unsigned int num_vol, unsigned int value);

// fonctions de gestion des blocs libres

void init_volume(unsigned int vol);

int load_super(unsigned int vol);

void save_super();

unsigned int new_block();

void free_bloc(unsigned int bloc);

void occupancy_free_rate();

unsigned int get_vol_size(unsigned int vol);