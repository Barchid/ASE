#include <stdio.h>
#include <stdlib.h>

#include "context.h"

struct ctx_s ctx_ping;
struct ctx_s ctx_pong;
struct ctx_s ctx_pang;

void f_ping(void *args);
void f_pong(void *args);
void f_pang(void *args);

int main(int argc, char *argv[])
{
    init_ctx(&ctx_ping, 16384, f_ping, NULL);
    init_ctx(&ctx_pong, 16384, f_pong, NULL);
	//init_ctx(&ctx_pang, 16384, f_pang, NULL); // TEST avec pang
    switch_to_ctx(&ctx_ping);

    exit(EXIT_SUCCESS);
}

void f_ping(void *args)
{
	//init_ctx(&ctx_pang, 16384, f_pang, NULL); // TEST où on initialise le contexte de pang dans une autre fonction
    while(1) {
        printf("A") ;
        switch_to_ctx(&ctx_pong);
        printf("B") ;
        switch_to_ctx(&ctx_pong);
		// switch_to_ctx(&ctx_pang); // TEST avec pang
        printf("C") ;
        switch_to_ctx(&ctx_pong);
    }
}

void f_pong(void *args)
{
    while(1) {
        printf("1") ;
        switch_to_ctx(&ctx_ping);
        printf("2") ;
        switch_to_ctx(&ctx_ping);
		// switch_to_ctx(&ctx_pang); // TEST avec pang
    }
}

void f_pang(void *args){
	while(1) {
		printf("[");
		switch_to_ctx(&ctx_pong);
		printf("-");
		switch_to_ctx(&ctx_ping);
		printf("]");
		switch_to_ctx(&ctx_pong);
	}
}
