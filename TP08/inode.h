#include "hw.h"

#ifndef _INODE_H_
#define _INODE_H_

#define NDIRECT 10 // le nombre de blocs directs contenus dans un fichier (inode)
#define BLOCK_NULL 0 // numéro de bloc de données qui veut dire que le bloc est plein de 0
#define NNBPB (HDA_SECTORSIZE / sizeof(unsigned int)) // nombre de numéros de bloc par bloc (la taille du bloc divisé par la taille de unsigned int)
#define BLOC_SIZE       HDA_SECTORSIZE  
#define DATA_BLOC_SIZE  BLOC_SIZE

enum file_type_e {FILE_FILE, FILE_DIRECTORY, FILE_SPECIAL};

struct inode_s {
	enum file_type_e inode_ft;
	unsigned int inode_size;
	unsigned int inode_direct[NDIRECT]; // liste des blocs directs
	unsigned int inode_indirect; // numéro de la liste des blocs indirects
	unsigned int inode_2indirect; // numéro de la liste des blocs doubles indirects
};

void read_inode (unsigned int inumber, struct inode_s *inode);

void write_inode (unsigned int inumber, const struct inode_s *inode);

unsigned int create_inode(enum file_type_e type); 

int delete_inode(unsigned int inumber);

unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc, unsigned int do_allocate);

void free_blocs(unsigned int blocs[], unsigned int size);
#endif