#include "context.h"
#include <assert.h>
#include <stdio.h>

int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args)
{
    // Activated flag to false
    ctx->activated = FALSE;

    // Find adress of stack
    asm("movl %%ebp, %1":"=r"(pctx->ctx_ebp));

    // Init stack
    if(malloc(ctx->stack, stack_size * sizeof(char)) == -1) {
       perror("Malloc error");
       return 0;
    }

    // Set esp

    // Set ebp

    // return
    return 1;
}