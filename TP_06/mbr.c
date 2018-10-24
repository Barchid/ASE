#include "drive.h"
#include "hw.h"
#include "mbr.h"
#include <assert.h>
#include <stdio.h>

#define MBR_MAGIC 0xCAFEBABE
#define MAX_VOL 8

struct vol_descr_s{
    unsigned int vol_first_sector;
    unsigned int vol_first_cylinder;
    unsigned int vol_n_sectors;
    enum vol_type_e vol_type;
};

struct mbr_s {
    struct vol_descr_s mbr_vols[MAX_VOL];
    unsigned int mbr_magic;
};

static struct mbr_s mbr;

unsigned int load_mbr(){
    int i;

    assert(sizeof(struct mbr_s) <= HDA_SECTORSIZE);

    read_sector_size(0, 0, sizeof(struct mbr_s), (unsigned char *) &mbr);

    if(mbr.mbr_magic == MBR_MAGIC)
        return 1;
    
    mbr.mbr_magic = MBR_MAGIC;

    for(i = 0; i < MAX_VOL; i++)
        mbr.mbr_vols[i].vol_type = VNONE;
        
    return 0;
}

void save_mbr(){
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(sizeof(struct mbr_s) <= HDA_SECTORSIZE);
    write_sector_size(0, 0, sizeof(struct mbr_s), (unsigned char *) &mbr);
}

unsigned int create_vol(unsigned int cylinder, unsigned int sector, unsigned int size, enum vol_type_e vol_type) {
    int i;
    
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(vol_type != VNONE);

    // Vérifier cylinder n'est pas dans une partition
    // Vérifier sector n'est pas dans une partition
    // Vérifier last cylinder pas dans une partition
    // Vérifier last sector pas dans une partition
    for(i=0;i<MAX_VOL;i++) {
        if(mbr.mbr_vols[i].vol_type != VNONE) {
            //assert(cylinder <= );
            //assert(sector <= );
            //assert(cylinder <= );
            //assert(cylinder <= );
            //s = (s0 + b) % NSPC
        }
    }
    for(i = 0; i < MAX_VOL; i++) {
        if(mbr.mbr_vols[i].vol_type == VNONE) {
            mbr.mbr_vols[i].vol_first_cylinder = cylinder;
            mbr.mbr_vols[i].vol_first_sector = sector;
            mbr.mbr_vols[i].vol_n_sectors = size;
            mbr.mbr_vols[i].vol_type = vol_type;

            return 1;
        }
    }

    return 0;
}

char* enumToString(enum vol_type_e vol_type){
    switch(vol_type){
        case VBASE :
            return "BASE";
        case VANX :
            return "ANNEXE";
        case VOTH :
            return "OTHER";
        default :
            return "NONE";
    }
}

unsigned int sector_of_block(unsigned int num_vol, unsigned int num_block) {
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(num_vol < MAX_VOL);
    assert(mbr.mbr_vols[num_vol].vol_type != VNONE);
    assert(num_block < mbr.mbr_vols[num_vol].vol_n_sectors);

    return (mbr.mbr_vols[num_vol].vol_first_sector + num_block) % HDA_MAXSECTOR;
}

unsigned int cylinder_of_block(unsigned int num_vol, unsigned int num_block) {
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(num_vol < MAX_VOL);
    assert(mbr.mbr_vols[num_vol].vol_type != VNONE);
    assert(num_block < mbr.mbr_vols[num_vol].vol_n_sectors);

    return mbr.mbr_vols[num_vol].vol_first_cylinder + (mbr.mbr_vols[num_vol].vol_first_sector + num_block) / HDA_MAXSECTOR;
}

void list_vol() {
    int i;
    puts("num_vol\t(c0,s0)\tnbloc\t type_vol\t(cl,sl)");
    for(i = 0; i < MAX_VOL; i++){

        if(mbr.mbr_vols[i].vol_type != VNONE) {
            printf("%7d\t(%2d,%2d)\t%5d\t%9s\t(%2d,%2d)\n",
                i,
                mbr.mbr_vols[i].vol_first_cylinder, 
                mbr.mbr_vols[i].vol_first_sector, 
                mbr.mbr_vols[i].vol_n_sectors, 
                enumToString(mbr.mbr_vols[i].vol_type), 
                cylinder_of_block(i, mbr.mbr_vols[i].vol_n_sectors - 1), 
                sector_of_block(i, mbr.mbr_vols[i].vol_n_sectors - 1)
            );
        }
        else {
            printf("%d is none\n", i);
        }
    }
}

void read_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer) {
    read_sector(cylinder_of_block(num_vol, num_block), sector_of_block(num_vol, num_block), buffer);
}

void write_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer) {
    write_sector(cylinder_of_block(num_vol, num_block), sector_of_block(num_vol, num_block), buffer);
}

void format_vol(unsigned int num_vol, unsigned int value) {
    assert(mbr.mbr_magic == MBR_MAGIC);
    assert(num_vol < MAX_VOL);
    assert(mbr.mbr_vols[num_vol].vol_type != VNONE);
    format_sector(mbr.mbr_vols[num_vol].vol_first_cylinder, mbr.mbr_vols[num_vol].vol_first_sector, mbr.mbr_vols[num_vol].vol_n_sectors, value);
}