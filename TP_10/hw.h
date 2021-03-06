#ifndef ARCHI_SIZE
#define ARCHI_SIZE 4
#endif

#ifndef HW_INI
#define HW_INI "hardware.ini"

#define MMU_IRQ 13
#define MMU_FAULT_ADDR 0xCD
#define MMU_CMD 0x66

#define TLB_ADD_ENTRY 0xCE // pour ajouter une entrée table du TLB
#define TLB_DEL_ENTRY 0xDE
#define TLB_SIZE 32
#define TLB_ENTRIES 0x800

#define PAGE_SIZE 4096
#define VM_SIZE 16777216 // 16mo en octet

#define N 200 // Nombre de pages physiques

#endif