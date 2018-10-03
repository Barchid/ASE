#include <stdio.h>
#include <stdlib.h>
#include "context.h"

#define COUNTER 17

void f_ping(void *args);
void f_pong(void *args);
void f_pang(void *args);

int main(int argc, char *argv[])
{

    create_ctx(16384, f_ping, NULL);
    create_ctx(16384, f_pong, NULL);
    create_ctx(16384, f_pang, NULL);
    // Initialisation de la librairie hardware
    start_sched();

    exit(EXIT_SUCCESS);
}

void f_ping(void *args)
{
    int i;
    while (1)
    {
        printf("A");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
        printf("B");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
        printf("C");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
    }
}

void f_pong(void *args)
{
    int i;
    while (1)
    {
        printf("1");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
        printf("2");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
    }
}

void f_pang(void *args)
{
    int i;
    while (1)
    {
        printf("[");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
        printf("-");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
        printf("]");
        for (i = 0; i < (1 << COUNTER); i++)
            ;
    }
}
