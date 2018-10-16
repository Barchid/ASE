#ifndef CTX_MAGIC
#define CTX_MAGIC 0xCAFEBABE
#endif

#ifndef ARCHI_SIZE
#define ARCHI_SIZE 4
#endif

#ifndef HW_INI
#define HW_INI "hardware.ini"

/* Horloge */
#define TIMER_CLOCK	0xF0
#define TIMER_PARAM     0xF4
#define TIMER_ALARM     0xF8
#define TIMER_IRQ	2   
#endif

typedef void (func_t) (void*);

int create_ctx(int stack_size, func_t f, void *args);

void yield();

void start_sched();

void sem_down(struct sem_s *sem);

void sem_up(struct sem_s *sem);