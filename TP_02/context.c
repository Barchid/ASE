#include "context.h"
#include <assert.h>
#include <stdio.h>

int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args)
{
    // Activated flag to false
    ctx->activated = FALSE;

    // Find adress of stack
    asm("movl %%ss, %1" :"=r"(ctx->stack));

    // Init stack
    if(malloc(ctx->stack, stack_size * sizeof(char)) == -1) {
       perror("Malloc error");
       return 0;
    }

    // Set esp --> mov ss, esp
    ctx->esp = (void*) ctx->stack;

    // Set ebp
    ctx->ebp = (void*) ctx->stack + stack_size;

    // Assign function
    ctx->entrypoint = &f;
    ctx->args = args;

    // return
    return 1;
}

void switch_to_ctx(struct ctx_s* ctx) {
    static struct ctx_s courant;
    // sauvegarde pointeurs de pile dans contexte courant

    // ctx devient ccontexte courant
    // ctx->ebp/esp dans registres
    // On doit bouger ss ?
    asm("movl %0, %%esp" "\n\t" "movl %1, %%ebp": :"r"(ctx->esp),"r"(ctx->ebp));

    //Si le ctx n'a jamais été activé, on l'active
    if(ctx->activated) {
        return;
    }
    else {
        ctx->activated = 0;
        ctx->entrypoint(ctx->args);
    }
}