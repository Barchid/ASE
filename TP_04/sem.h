#ifndef CTX_MAGIC
#define CTX_MAGIC 0xCAFEBABE
#endif

struct sem_s {
	int sem_cpt;
	struct ctx_s *sem_ctx_list;
};

typedef void (func_t) (void*);

int create_ctx(int stack_size, func_t f, void *args);

void yield();

void start_sched();

void sem_down(struct sem_s *sem);

void sem_up(struct sem_s *sem);

void sem_init(struct sem_s *sem, unsigned int val);