
void dump(unsigned char *buffer, unsigned int buffer_size, int ascii_dump, int octal_dump);

void goToSector(unsigned int cylinder, unsigned int sector);

void read_sector(unsigned int cylinder, unsigned int sector, unsigned char *buffer);

void read_sector_size(unsigned int cylinder, unsigned int sector, int size, unsigned char* buffer);

void write_sector(unsigned int cylinder, unsigned int sector, const unsigned char *buffer);

void format_sector(unsigned int cylinder, unsigned int sector, unsigned int nsector, int value);

void init();