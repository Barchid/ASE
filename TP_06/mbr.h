enum vol_type_e {VBASE, VANX, VOTH, VNONE};

unsigned int load_mbr();

void save_mbr();

unsigned int create_vol(unsigned int cylinder, unsigned int sector, unsigned int size, enum vol_type_e vol_type);

void delete_vol(unsigned int num_vol);

void list_vol();

void read_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer);

void write_block(unsigned int num_vol, unsigned int num_block, unsigned char *buffer);

void format_vol(unsigned int num_vol, unsigned int value);