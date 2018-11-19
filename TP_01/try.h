#ifndef CTX_MAGIC
#define CTX_MAGIC 0xCAFEBABE
#endif

typedef int (func_t)(int); /* a function that returns an int from an int */

struct ctx_s {
    unsigned magic; // sert à checker si le ctx_s a bien été initialisé
    void *ebp; // registre ebp (base de la pile)
    void *esp; // registre esp (sommet de la pile)
};

int try(struct ctx_s *pctx, func_t *f, int arg);

int throw(struct ctx_s *pctx, int r);