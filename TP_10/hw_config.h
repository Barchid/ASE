/* ------------------------------
   $Id: hw_config.h 105 2009-11-24 15:22:50Z simon_duquennoy $
   ------------------------------------------------------------

   Fichier de configuration des acces au materiel

   Philippe Marquet, march 2007

   Code au niveau applicatif la description du materiel qui est fournie
   par hardware.ini
   
*/

#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#define HARDWARE_INI	"hardware.ini"

#define MMU_IRQ		13
#define MMU_CMD		0x66
#define MMU_FAULT_ADDR 	0xCD
#define TLB_ADD_ENTRY	0xCE
#define TLB_DEL_ENTRY	0xDE
#define TLB_SIZE	1
#define TLB_ENTRIES	0x800

//#define PM_PAGES (1 << 8)
#define PM_PAGES 3
//#define VM_PAGES (1 << 12)
#define VM_PAGES 3
#define PAGE_SIZE 1
#define PM_SIZE (PAGE_SIZE * PM_PAGES)
#define VM_SIZE (PAGE_SIZE * VM_PAGES)

#endif

