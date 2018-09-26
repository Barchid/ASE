#ifndef CTX_MAGIC
#define CTX_MAGIC 0xCAFEBABE
#endif

#ifndef ARCHI_SIZE
#define ARCHI_SIZE 4
#endif

typedef void (func_t) (void*);

enum ctx_state_e {CTX_INIT, CTX_EXEC, CTX_END};

struct ctx_s {
    unsigned int ctx_magic;
    void* ctx_esp;
    void* ctx_ebp;
    unsigned char* ctx_base;
    func_t* ctx_f;
    void* ctx_arg;
    enum ctx_state_e ctx_state;
};

int init_ctx(struct ctx_s* ctx, int stack_size, func_t f, void* args);

void switch_to_ctx(struct ctx_s* ctx);