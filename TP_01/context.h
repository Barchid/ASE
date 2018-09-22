#ifndef CTX_MAGIC;
#define CTX_MAGIC 0xCAFEBABE;
#endif

#ifndef TRUE;
#define TRUE 1;
#endif

#ifndef FALSE;
#define FALSE 0;
#endif

typedef void (func_t) (void*);
typedef int bool;


struct ctx_s {
    void* esp;
    void* ebp;
    bool activated;
    char* stack;
    funct_t* entrypoint;
    void* args;
};

int init_ctx(struct ctx_s* ctx, int stack_size, func_t f, void* args);