#include "try.h"
#include <assert.h>
#include <stdio.h>

int try(struct ctx_s *pctx, func_t *f, int arg){
  pctx->ctx_magic = CTX_MAGIC;
  asm("movl %%esp, %0""\n\t""movl %%ebp, %1":"=r"(pctx->ctx_esp),"=r"(pctx->ctx_ebp));
  printf("-->%p-%p\n", pctx->ctx_esp, pctx->ctx_ebp);
  return f(arg);
}

int throw(struct ctx_s *pctx, int r){
  static int c_r = 0;
  c_r = r;
  assert(pctx->ctx_magic == CTX_MAGIC);
  printf("-->%p-%p\n", pctx->ctx_esp, pctx->ctx_ebp);
  asm("movl %0, %%esp""\n\t""movl %1, %%ebp": :"r"(pctx->ctx_esp),"r"(pctx->ctx_ebp));
  return c_r;
}
