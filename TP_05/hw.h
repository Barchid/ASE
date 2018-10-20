#ifndef ARCHI_SIZE
#define ARCHI_SIZE 4
#endif

#ifndef HW_INI
#define HW_INI "hardware.ini"

//Paramètres du contrôleur IDE
#define HDA_CMDREG 0x3F6
#define HDA_DATAREGS 0x110
#define HDA_IRQ 14
#define HDA_SECTORSIZE 32
#define HDA_MAXSECTOR 16
#define HDA_MAXCYLINDER 16

//Commandes ATA-2
#define CMD_SEEK 0x02
#define CMD_READ 0x04
#define CMD_WRITE 0x06
#define CMD_FORMAT 0x08
#define CMD_DSKNFO 0x16

#endif