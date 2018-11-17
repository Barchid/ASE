#define MBR_MAGIC 0xCAFEBABE // Sert à vérifier l'initialisation du MBR
#define MAX_VOL 8 // nombre de volumes max

enum vol_type_e {VBASE, VANX, VOTH, VNONE}; // types de partition existants

// Structure décrivant une partition
struct vol_descr_s{
    unsigned int vol_first_sector; // numéro de secteur du premier secteur de la partition
    unsigned int vol_first_cylinder; // numéro de cylindre du premier secteur de la partition
    unsigned int vol_n_sectors; // nombre de secteurs de la partition
    enum vol_type_e vol_type; // type de la partition
};

// Structure qui décrit le MBR (Master Boot Record)
struct mbr_s {
    struct vol_descr_s mbr_vols[MAX_VOL]; // Tableau des partitions
    unsigned int mbr_magic; // Vérifie si le MBR a été correctement initialisé
};

// le MBR gardé en mémoire
extern struct mbr_s mbr;

// Charge le MBR en mémoire (dans la variable statique)
unsigned int load_mbr();

// sauvegarde le MBR
void save_mbr();

// crée un volume
unsigned int create_vol(unsigned int cylinder, unsigned int sector, unsigned int size, enum vol_type_e vol_type);

void delete_vol(unsigned int vol);

// Lister l'état des volumes
void list_vol();

// Lire un bloc
void read_bloc(unsigned int vol, unsigned int nblock, unsigned char *buffer);

void read_bloc_size(unsigned int vol, unsigned int nbloc, unsigned int size, unsigned char *buffer);

void write_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer);

void write_bloc_size(unsigned int vol, unsigned int nbloc, unsigned int size, unsigned char *buffer);

void format_vol(unsigned int vol, unsigned int value);