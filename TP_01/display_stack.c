 #include <stdio.h>

void aff_reg(int* x, int* y);
void a();
void b();

/*
 * Le registre ESP pointe le sommet de la frame dans la pile, il est normal que ce soit l'adresse la plus petite
 * Le registre ebp pointe la base de la frame, son adresse est donc la plus grande
 * les variables locales
 */

int main(int argc, char const *argv[]) {
   int x = 3;
   int y = 5;
   printf("x:%p\ny :%p\n----------\n", &x, &y);
   aff_reg(&x, &y);
   a();
   return 0;
}

 void aff_reg(int* x, int* y){
   void* esp;
   void* ebp;

   int* i;
   int* j;

   asm("mov %%rsp, %0""\n\t""mov %%rbp, %1"::"r"(esp),"r"(ebp));

   printf("esp:%p\nebp:%p\n----------\n", esp, ebp);
   printf("i:%p\nj :%p\n----------\n", i, j);
   printf("x:%p\ny :%p\n----------\n", x, y);
 }

 void a(){
   int x = 3;
   int y = 5;
   printf("x:%p\ny :%p\n----------\n", &x, &y);
   aff_reg(&x, &y);
   b();
 }

 void b(){
    int x = 3;
    int y = 5;
    printf("x:%p\ny :%p\n----------\n", &x, &y);
    aff_reg(&x, &y);
 }
