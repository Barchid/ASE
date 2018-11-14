#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mbr.h"
#include "drive.h"



// créer un inode
unsigned int create_inode(enum file_type_e type) {
	unsigned int i;
	// Initialiser un inode structure
	struct inode_s inode;
	inode.inode_ft = type;
	inode.inode_size = BLOCK_NULL;
	inode.inode_indirect = BLOCK_NULL; // PAR CONVENTION : un numéro de bloc de données qui vaut 0 --> bloc plein de 0
	inode.inode_2indirect = BLOCK_NULL; // PAR CONVENTION : un numéro de bloc de données qui vaut 0 --> bloc plein de 0
	
	for(int i = 0; i < NDIRECT; i++) {
		inode.inode_direct[i] = BLOCK_NULL; // PAR CONVENTION : un numéro de bloc de données qui vaut 0 --> bloc plein de 0
	}
	
	// trouver un bloc sur le volume (le nombre renvoyé par new_block = le inumber)
	i = new_block();
	assert(i != 0); // si i = 0, il n'y a plus de blocs libres.
	
	// Ecrire inode dans le bloc trouvé
	write_inode(i, &inode);
	
	// retourner inumber;
	return i;
}


unsigned int new_block_zero() {
    int i;
    unsigned int blocks[NNBPB];
    unsigned int n_block = new_block();

    if(n_block != 0){
        for(i = 0; i < NNBPB; i++)
            blocks[i] = BLOCK_NULL;
        write_block_size(current_vol, n_block, NNBPB * sizeof(unsigned int), (unsigned char *) blocks);
    }

    return n_block;
}


// récupérer le numéro de bloc du volume à partir du numéro de bloc du fichier
// allocate est un booleen pour dire qu'on veut allouer fbloc
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc, unsigned int do_allocate) {
	struct inode_s inode;
	unsigned int blocs[NNBPB];
    unsigned int new;
	
	// check inumber != 0
	assert(inumber != 0);
	
	read_inode(inumber, &inode);
	
	// il est dans ma liste de blocs directs
	if(fbloc < NDIRECT) {
		// Si je dois allouer le bloc et que fbloc est 0, alors j'alloue
		if(inode.inode_direct[fbloc] == BLOCK_NULL && do_allocate) {
			inode.inode_direct[fbloc] = new_block();
            
            if(inode.inode_direct[fbloc] == 0) {
                return BLOCK_NULL;
            }

			// comme j'ai modifié l'inode, je l'écris sur le disque
			write_inode(inumber, &inode);
		}
		return inode.inode_direct[fbloc]; // s'il retourne BLOCK_NULL, ça veut dire que c'est un bloc plein de 0 !!!
	}
	
	fbloc -= NDIRECT; // translation des numéros pour retrouver dans les blocs indirects
	
	// il est dans ma liste de blocs indirects
	if(fbloc < NNBPB) {
		// si mon numéro d'inode est null, ça veut dire que j'aurai un bloc plein de 0
		if(inode.inode_indirect == BLOCK_NULL) {
			// si je dois allouer
			// if(do_allocate) {
			// 	inode.inode_indirect = new_block_zero(); // créer le bloc d'indirect
            //     // TODO : finir
			// }
			
			return BLOCK_NULL;
		}
		
		read_bloc_size(current_vol, inode.inode_indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
		
		// si je dois allouer un bloc dans les indirections et que fbloc est null
		if(blocs[fbloc] == BLOCK_NULL && do_allocate) {
			blocs[fbloc] = new_block();
			// écrire le block pour persister le changement
			write_block_size(current_vol, inode.inode_indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
		}
		return blocs[fbloc];
	}
	
	// décalage au premier double indirect
	fbloc -= NNBPB;
	
	// il est dans ma liste de blocs 2indirects
	// if(fbloc < NNBPB * NNBPB) {
	// 	if(inode.inode_2indirect == BLOCK_NULL) {
	// 		// si je dois allouer
	// 		if(do_allocate) {
    //             // creer le bloc de 2indirect
	// 			inode.inode_2indirect = new_block_zero(); // créer le bloc d'indirect

    //             // creer le premier bloc d'indirection de 2indirect
    //             new = new_block_zero();
    //             read_block_size(current_vol, inode.inode_2indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
    //             blocs[0] = new;
    //             write_block_size(current_vol, new, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
    //             write_inode(inumber, &inode);
    //             return ;
	// 		}
			
	// 		return BLOCK_NULL;
	// 	}


	// } 
	
	// si fbloc > NNPB², ça veut dire qu'on essaie d'accéder un numéro trop grand pour notre fichier, donc -1
	assert(false);
    return 0; // pas atteignable
}


// supprimer inode et libérer les blocs alloués
// libérer les blocs directs, indirects, 2indirects
int delete_inode(unsigned int inumber) {
	struct inode_s inode;
	unsigned int i;
	unsigned int blocs[NNBPB];
	
	// inumber doit être non nul
	assert(inumber != 0);
	
	// lire l'inode à inumber
	read_inode(inumber, &inode);
	
	// free les blocs directs
	free_blocs(inode.inode_direct, NDIRECT);
	
	// SI indirects != 0, c'est que j'ai des blocs indirects
	if(inode.inode_indirect) {
		// free les blocs indirects
		read_block_size(current_vol, inode.inode_indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
		free_blocs(blocs, NNBPB); // free les blocs 
		free_bloc(inode.inode_indirect); // ET le bloc d'indirection lui-même 
	}
	
	// Si 2indirects != 0, c'est que j'ai des blocs 2indirects
	if(inode.inode_2indirect) {
		// TODO
	}
	
	// free les blocs indirects
	read_block();
}


// lire l'inode
void read_inode(unsigned int inumber, struct inode_s *inode) {
	// Juste lire le bloc d'inode placé à l'inumber
	read_bloc_size(current_vol, inumber, sizeof(struct inode_s), (unsigned char *) &inode);
}

void write_inode(unsigned int inumber, struct inode_s *inode) {
	write_block_size(current_vol, inumber, sizeof(struct inode_s), (unsigned char *) &inode);
}