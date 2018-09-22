#ifndef CTX_MAGIC
#define CTX_MAGIC 0xCAFEBABE
#endif

struct ctx_s {
  int ctx_magic;
  void* ctx_esp;
  void* ctx_ebp;
  char* stack;
  func_t entrypoint;
};

typedef int (func_t) (int);

int try(struct ctx_s *pctx, func_t *f, int arg);

int throw(struct ctx_s *pctx, int r);
