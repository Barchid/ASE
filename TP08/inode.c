#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mbr.h"
#include "drive.h"
#include "bloc.h"



// créer un inode
unsigned int create_inode(enum file_type_e type) {
	unsigned int i;
	// Initialiser un inode structure
	struct inode_s inode;
	inode.inode_ft = type;
	inode.inode_size = BLOCK_NULL;
	inode.inode_indirect = BLOCK_NULL; // PAR CONVENTION : un numéro de bloc de données qui vaut 0 --> bloc plein de 0
	inode.inode_2indirect = BLOCK_NULL; // PAR CONVENTION : un numéro de bloc de données qui vaut 0 --> bloc plein de 0
	
	for(i = 0; i < NDIRECT; i++) {
		inode.inode_direct[i] = BLOCK_NULL; // PAR CONVENTION : un numéro de bloc de données qui vaut 0 --> bloc plein de 0
	}
	
	// trouver un bloc sur le volume (le nombre renvoyé par new_block = le inumber)
	i = new_bloc();
	assert(i != 0); // si i = 0, il n'y a plus de blocs libres.
	
	// Ecrire inode dans le bloc trouvé
	write_inode(i, &inode);
	
	// retourner inumber;
	return i;
}


unsigned int new_bloc_zero() {
    unsigned int i;
    unsigned int blocks[NNBPB];
    unsigned int n_block = new_bloc();

    if(n_block != 0){
        for(i = 0; i < NNBPB; i++)
            blocks[i] = BLOCK_NULL;
        write_bloc_size(current_vol, n_block, NNBPB * sizeof(unsigned int), (unsigned char *) blocks);
    }

    return n_block;
}


// récupérer le numéro de bloc du volume à partir du numéro de bloc du fichier
// allocate est un booleen pour dire qu'on veut allouer fbloc
// Quand do_allocate = false, retourner BLOCK_NULL veut dire qu'on retourne un bloc plein de zéros
// Quand do_allocate = true, retourner BLOCK_NULL veut dire qu'on n'a pas pu allouer.
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc, unsigned int do_allocate) {
	struct inode_s inode;
	unsigned int blocs[NNBPB];
	unsigned int sousblocs[NNBPB];
    unsigned int new;
	unsigned int i;
	
	// check inumber != 0
	assert(inumber != 0);
	
	// lire l'inode du fichier dont on veut trouver le bloc
	read_inode(inumber, &inode);
	
	// il est dans ma liste de blocs directs
	if(fbloc < NDIRECT) {
		// Si je dois allouer le bloc et que fbloc est 0, alors j'alloue
		if(inode.inode_direct[fbloc] == BLOCK_NULL && do_allocate) {
			inode.inode_direct[fbloc] = new_bloc();
            
			// tester si le bloc a pu être alloué
            if(inode.inode_direct[fbloc] == 0) {
                return 0;
            }

			// comme j'ai modifié l'inode, je l'écris sur le disque
			write_inode(inumber, &inode);
		}
		return inode.inode_direct[fbloc]; // s'il retourne BLOCK_NULL, ça veut dire que c'est un bloc plein de 0 !!!
	}
	
	fbloc -= NDIRECT; // translation des numéros pour retrouver dans les blocs indirects
	
	// il est dans ma liste de blocs indirects
	if(fbloc < NNBPB) {
		// si le bloc indirect est BLOCK_NULL, ça veut dire que j'aurai un bloc plein de 0
		if(inode.inode_indirect == BLOCK_NULL && !do_allocate) {
			return BLOCK_NULL;
		}

		// je dois allouer sur le bloc d'indirect alors qu'il n'est pas encore initialisé
		if(do_allocate && inode.inode_indirect == BLOCK_NULL) {
			inode.inode_indirect = new_bloc_zero(); // créer le bloc d'indirect

			// il n'y a plus de blocs dispo, on retourne BLOCK_NULL
			if(inode.inode_indirect == 0) {
				return BLOCK_NULL;
			}
			
			// sauvegarder l'inode
			write_inode(inumber, &inode);
		}
		
		// Lire le bloc d'indirection et placer son contenu dans blocs
		read_bloc_size(current_vol, inode.inode_indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
		
		// si je dois allouer un bloc dans les indirections et que fbloc est BLOCK_NULL
		if(blocs[fbloc] == BLOCK_NULL && do_allocate) {
			
			// allouer un nouveau block et le référencer dans le bloc d'indirection
			blocs[fbloc] = new_bloc();
			
			// Pas d'allocation possible
			if(blocs[fbloc] == 0) {
				return BLOCK_NULL;
			}
			
			// écrire le block pour persister le changement
			write_bloc_size(current_vol, inode.inode_indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
		}

		return blocs[fbloc];
	}
	
	// décalage au double indirect
	fbloc -= NNBPB;
	
	// fbloc est dans les blocs 2indirects
	if(fbloc < NNBPB * NNBPB) {
		// SI je ne dois pas allouer et que les 2indirects sont null, on renvoie BLOCK_NULL
		if(inode.inode_2indirect == BLOCK_NULL && !do_allocate) {
			return BLOCK_NULL;
		}
		
		// SI je dois allouer et que 2indirect est BLOCK_NULL
		if(do_allocate && inode.inode_2indirect == BLOCK_NULL) {
			// allouer le bloc de 2indirect
			inode.inode_2indirect = new_bloc_zero();
			
			// RETURN BLOCK_NULL si new_bloc_zero n'a pas marché
			if(inode.inode_2indirect == BLOCK_NULL) {
				return BLOCK_NULL;
			}
			
			// sauvegarder l'inode
			write_inode(inumber, &inode);
			
		}
		
		// Charger le bloc de 2indirect
		read_bloc_size(current_vol, inode.inode_2indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
		
		// TROUVER LE BON SOUS-BLOC où fbloc peut se trouver
		for(i=0;i<NNBPB;i++) {
			// Je change d'itération si fbloc n'est pas compris dans le sous bloc i
			if(fbloc >= (i+1)*NNBPB) {
				continue;
			}
			// ici, je suis sûr que mon sous-bloc est le bon
			
			// Si le sous-bloc n'est pas alloué et que je dois
			if(blocs[i] == BLOCK_NULL && do_allocate) {
				// initialiser le sous-bloc d'indirection
				blocs[i] = new_bloc_zero();
				
				// Pas réussi à allouer le block
				if(blocs[i] == BLOCK_NULL) {
					return BLOCK_NULL;
				}
			}
			
			// Si le sous-bloc n'est pas initialisé et que je ne dois pas allouer
			if(blocs[i] == BLOCK_NULL) {
				return BLOCK_NULL;
			}
			// décaler fbloc pour avoir un bon indice
			fbloc-=(i+1)*NNBPB;
			
			// Lire le sous-bloc
			read_bloc_size(current_vol, blocs[i], NNBPB * sizeof(unsigned int), (unsigned char *) sousblocs);
			
			// SI je dois allouer le fbloc et qu'il ne l'est pas
			if(do_allocate && sousblocs[fbloc] == BLOCK_NULL) {
				sousblocs[fbloc] = new_bloc();
				
				// Si mon allocation n'a pas marché, je vais d'office renvoyer block_null plus tard --> PAS BESOIN DE FAIRE UN IF
				
				// sauvegarder le changement dans le sous-bloc
				write_bloc_size(current_vol, blocs[i], NNBPB * sizeof(unsigned int), (unsigned char *) sousblocs);
			}
			
			return sousblocs[fbloc];
		}
		// Je ne rentrerai jamais ici car je check si i < NNBPB²
	}
	
	// si fbloc > NNBPB², ça veut dire qu'on essaie d'accéder un numéro trop grand pour notre fichier, donc NON
	assert(false);
    return 0; // pas atteignable
}


// supprimer inode et libérer les blocs alloués
// libérer les blocs directs, indirects, 2indirects
int delete_inode(unsigned int inumber) {
	struct inode_s inode;
	unsigned int i;
	unsigned int blocs[NNBPB];
	unsigned int blocs2i[NNBPB];
	
	// inumber doit être non nul
	assert(inumber != 0);
	
	// lire l'inode à inumber
	read_inode(inumber, &inode);
	
	// free les blocs directs
	free_blocs(inode.inode_direct, NDIRECT);
	
	// SI indirects != BLOCK_NULL, c'est que j'ai des blocs indirects
	if(inode.inode_indirect != BLOCK_NULL) {
		// lire le bloc d'indirection
		read_bloc_size(current_vol, inode.inode_indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
		free_blocs(blocs, NNBPB); // free les blocs référencés par le bloc d'indirection
		free_bloc(inode.inode_indirect); // ET le bloc d'indirection lui-même 
	}
	
	// Si 2indirects != 0, c'est que j'ai des blocs 2indirects
	if(inode.inode_2indirect) {
		// lire le bloc de 2indirection
		read_bloc_size(current_vol, inode.inode_2indirect, NNBPB * sizeof(unsigned int), (unsigned char *) blocs2i);
		
		// POUR CHAQUE bloc référencé par le bloc de 2indirection
		for(i=0;i<NNBPB;i++) {
			if(blocs2i[i] == BLOCK_NULL) continue; // passer si le sous-bloc est BLOCK_NULL
			
			// lire le sous-bloc de 2indirection
			read_bloc_size(current_vol, blocs2i[i], NNBPB * sizeof(unsigned int), (unsigned char *) blocs);
			free_blocs(blocs, NNBPB); // free les blocs référencés par le sous-bloc d'indirection
			free_bloc(blocs2i[i]); // free le sous-bloc lui-même
		}
		// free le bloc de 2indirection lui-même
		free_bloc(inode.inode_2indirect);
	}
	
	return 1;
}


// lire l'inode
void read_inode(unsigned int inumber, struct inode_s *inode) {
	// Juste lire le bloc d'inode placé à l'inumber
	read_bloc_size(current_vol, inumber, sizeof(struct inode_s), (unsigned char *) &inode);
}

void write_inode(unsigned int inumber, struct inode_s *inode) {
	write_block_size(current_vol, inumber, sizeof(struct inode_s), (unsigned char *) &inode);
}

// libérer le tableau de numéro de blocs
void free_blocs(unsigned int blocs[], unsigned int size) {
	unsigned int i;
	for(i = 0; i < size; i++) {
		// Si le bloc est un BLOCK_NULL, on ne le free pas
		if(blocs[i] != BLOCK_NULL){
			free_bloc(blocs[i]);
		}
	}
}