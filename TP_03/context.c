#include "context.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"

enum ctx_state_e
{
    CTX_INIT,
    CTX_EXEC,
    CTX_END
};

struct ctx_s
{
    unsigned int ctx_magic;
    void *ctx_esp;
    void *ctx_ebp;
    unsigned char *ctx_base;
    func_t *ctx_f;
    void *ctx_arg;
    enum ctx_state_e ctx_state;
    struct ctx_s *ctx_next;
};

int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args);

void switch_to_ctx(struct ctx_s *ctx);

void start_current_ctx();

void irq_disable(); // retirer les interruptions

void irq_enable(); // réécouter les interruptions

void handler();

static void empty_it(void)
{
    return;
}

static struct ctx_s main_ctx;
static struct ctx_s *current_ctx = NULL; // Quand on est dans le main, NULL
static struct ctx_s *ring = NULL;        // Quand on n'a encore aucun contexte initialisé

int create_ctx(int stack_size, func_t f, void *args)
{
    struct ctx_s *new;
    new = (struct ctx_s *)malloc(sizeof(struct ctx_s));
    assert(new);

    init_ctx(new, stack_size, f, args);

    irq_disable();
    if (!ring)
    {
        ring = new;
        new->ctx_next = new;
    }
    else
    {
        new->ctx_next = ring->ctx_next;
        ring->ctx_next = new;
    }
    irq_enable();

    return 0;
}

void yield()
{
    if (current_ctx)
    {
        switch_to_ctx(current_ctx->ctx_next);
    }
    else if (ring)
    {
        switch_to_ctx(ring);
    }
    else
    {
        irq_disable();
        // On rend la main au main()
        asm("movl %0, %%esp"
            "\n\t"
            "movl %1, %%ebp"
            :
            : "r"(main_ctx.ctx_esp), "r"(main_ctx.ctx_ebp));
        irq_enable();
    }
}

// Initialise le contexte d'exécution en paramètre avec une pile d'exécution à taille {{stack_size}}
int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args)
{

    // Init le champ magic
    ctx->ctx_magic = CTX_MAGIC;
    // Le state du contexte est en INIT (pas encore exécuté)
    ctx->ctx_state = CTX_INIT;

    // Associer la fonction et ses arguments
    ctx->ctx_f = f;
    ctx->ctx_arg = args;

    // Allouer la pile d'exécution
    if ((ctx->ctx_base = malloc(stack_size)) == NULL)
    {
        perror("Error malloc \n");
        return -1;
    }

    // Mémoriser ebp/esp
    // ebp et esp doivent se trouver à l'adresse du dernier mot de la pile que l'on a allouée
    ctx->ctx_esp = ctx->ctx_ebp = ctx->ctx_base + stack_size - ARCHI_SIZE;

    return 0;
}

// appelee par switch_to_ctx
void start_current_ctx()
{
    // On lance le contexte courant
    current_ctx->ctx_state = CTX_EXEC;
    current_ctx->ctx_f(current_ctx->ctx_arg);

    // Ici, current_ctx a fini de s'exécuter
    current_ctx->ctx_state = CTX_END;

    //On passe la main au contexte suivant
    yield();
}

// Change de contexte d'exécution pour qu'un autre s'exécute
void switch_to_ctx(struct ctx_s *ctx)
{
    // Vérifier que magic est ok
    // Vérifier que le contexte n'est pas terminé (état CTX_END)
    assert(ctx->ctx_state == CTX_INIT || ctx->ctx_state == CTX_EXEC || ctx->ctx_magic == CTX_MAGIC);

    irq_disable(); // ignorer les interruptions

    while (ctx->ctx_state == CTX_END)
    {
        //
        if (ctx == ctx->ctx_next)
        {
            // On rend la main au main()
            asm("movl %0, %%esp"
                "\n\t"
                "movl %1, %%ebp"
                :
                : "r"(main_ctx.ctx_esp), "r"(main_ctx.ctx_ebp));
        }
        else
        { // S'il ne reste
            free(ctx->ctx_base);
            current_ctx->ctx_next = ctx->ctx_next;
            free(ctx);
        }
    }
    irq_enable(); // Remettre interruptions

    if (current_ctx)
    {
        irq_disable();
        asm("movl %%esp, %0"
            "\n\t"
            "movl %%ebp, %1"
            : "=r"(current_ctx->ctx_esp), "=r"(current_ctx->ctx_ebp));
        irq_enable();
    }
    else
    {
        // Contexte du main()
        // On mémorise le esp et ebp du main
        // pour le restaurer plus tard
        irq_disable();
        asm("movl %%esp, %0"
            "\n\t"
            "movl %%ebp, %1"
            : "=r"(main_ctx.ctx_esp), "=r"(main_ctx.ctx_ebp));
        irq_enable();
    }

    // ctx devient le contexte courant
    // Il faut le faire AVANT de changer esp/ebp
    // Sinon on n'y aurait plus accès
    current_ctx = ctx;

    // Déplacer esp/ebp dans le nouveau contexte
    irq_disable();
    asm("movl %0, %%esp"
        "\n\t"
        "movl %1, %%ebp"
        :
        : "r"(current_ctx->ctx_esp), "r"(current_ctx->ctx_ebp));
    irq_enable();

    // Si c'est la première fois que mon contexte doit s'exécuter
    // Je lance la fonction contenue dans current_ctx
    if (current_ctx->ctx_state == CTX_INIT)
        start_current_ctx();
}

/**
 * 
 */
void start_sched()
{
    unsigned int i;
    /* init hardware */
    if (init_hardware(HW_INI) == 0)
    {
        fprintf(stderr, "Error in hardware initialization\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 16; i++)
        IRQVECTOR[i] = empty_it;

    /* program timer */
    IRQVECTOR[TIMER_IRQ] = handler;
    _out(TIMER_PARAM, 128 + 64 + 32 + 8); /* reset + alarm on + 8 tick / alarm */
    _out(TIMER_ALARM, 0xFFFFFFFE);        /* alarm at next tick (at 0xFFFFFFFF) */

    /* allows all IT */
    _mask(1);
    yield();
}

void irq_disable() {
    _mask(15);
}

void irq_enable() {
    _mask(1);
}

void handler() {
    _out(TIMER_ALARM, 0xFFFFFFFE);        /* alarm at next tick (at 0xFFFFFFFF) */
    yield();
}