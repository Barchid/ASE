#include "try.h"
#include <stdio.h>

static struct ctx_s ctx;

static int mul(int depth)
{
    int i;
    switch (scanf("%d", &i)) {
        case EOF :
            return 1;
        case 0 :
            return mul(depth+1);
        case 1 :
            if (i)
                return i * mul(depth+1);
            else
                return throw(&ctx, 0);
    }
}

int main(int argc, char const *argv[])
{
    int product;
    printf("A list of int, please\n");
    printf("product = %d\n", try(&ctx, mul, 0));
}
