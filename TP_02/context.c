#include "context.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static struct ctx_s main_ctx; // TODO : à remplacer par un esp et un ebp, sert à sauvegarder les contextes d'exec
static struct ctx_s *current_ctx = NULL; // contexte courant, celui qui s'exécute

int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args) {

    // initialiser le champ magic
    ctx->ctx_magic = CTX_MAGIC;
    ctx->ctx_state = CTX_INIT;
    ctx->ctx_f = f;
    ctx->ctx_arg = args;

    ctx->ctx_base = malloc(stack_size);
    assert(ctx->ctx_base);
    ctx->ctx_esp = ctx -> ctx_ebp = ctx -> ctx_base + stack_size - ARCHI_SIZE;

    return 0;
}

void start_current_ctx() {
	// exécuter le contexte courant
    current_ctx->ctx_state = CTX_EXEC;
    current_ctx->ctx_f(current_ctx->ctx_arg);
	
	// quand on est ici, le current_ctx a fini de s'exécuter
    current_ctx->ctx_state = CTX_END;
    puts("Contexte terminé");

	// on remet l'esp et ebp d'avant dans les registres
    asm("movl %0, %%esp" "\n\t" "movl %1, %%ebp"
    :
    : "r" (main_ctx.ctx_esp)
    ,"r" (main_ctx.ctx_ebp));

}

void switch_to_ctx(struct ctx_s* ctx) {
    assert(ctx->ctx_state == CTX_INIT || ctx->ctx_state == CTX_EXEC || ctx->ctx_magic);
	
    if(current_ctx){ // quand il y a un current_ctx déjà
        asm("movl %%esp, %0" "\n\t" "movl %%ebp, %1"
        : "=r" (current_ctx->ctx_esp)
        ,"=r" (current_ctx->ctx_ebp));
    }
	// quand current_ctx = null (donc quand c'est la premiere fois que current_ctx)
    else{
        asm("movl %%esp, %0" "\n\t" "movl %%ebp, %1"
        : "=r" (main_ctx.ctx_esp)
        ,"=r" (main_ctx.ctx_ebp));
    }

    current_ctx = ctx;

    asm("movl %0, %%esp" "\n\t" "movl %1, %%ebp"
    :
    : "r" (current_ctx->ctx_esp)
    ,"r" (current_ctx->ctx_ebp));

    if(current_ctx->ctx_state == CTX_INIT)
        start_current_ctx();
}