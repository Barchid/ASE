#ifndef CTX_MAGIC
#define CTX_MAGIC 0xCAFEBABE
#endif

typedef int (func_t)(int); /* a function that returns an int from an int */

struct ctx_s {
    unsigned magic;
    void *ebp;
    void *esp;
};

int try(struct ctx_s *pctx, func_t *f, int arg);

int throw(struct ctx_s *pctx, int r);