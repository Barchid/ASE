 #include <stdio.h>

void aff_reg(int x, int y);
void a();

/*
 * Le registre ESP pointe le sommet de la frame dans la pile, il est normal que ce soit l'adresse la plus petite
 * Le registre ebp pointe la base de la frame, son adresse est donc la plus grande
 * les variables locales
 */

int main(int argc, char const *argv[]) {
	int i = 3;
	int j = 5;
	void* esp;
	void* ebp;
    asm("mov %%rsp, %0""\n\t""mov %%rbp, %1":"=r"(esp),"=r"(ebp));
	
	printf("Fonction main()\n###################\n");
	printf("esp vaut : %p\nebp vaut :%p\n", esp, ebp);
	printf("La variable locale i vaut :%p\nla variable locale j vaut :%p\n", &i, &j);
	aff_reg(i, j);
	a();
	return 0;
}

 void aff_reg(int x, int y){
	int i = 3;
	int j = 2;
	void* esp;
	void* ebp;
    asm("mov %%rsp, %0""\n\t""mov %%rbp, %1":"=r"(esp),"=r"(ebp));
	
	printf("Fonction aff_reg()\n###################\n");
	printf("esp vaut : %p\nebp vaut :%p\n", esp, ebp);
	printf("La variable locale i vaut :%p\nla variable locale j vaut :%p\n", &i, &j);
	printf("Le paramètre x vaut :%p\nLe paramètre y vaut :%p\n###################\n\n", &x, &y);
	a();
 }

 void a(){
	static int fin = 0;
	int i = 3;
	int j = 2;
	void* esp;
	void* ebp;
	asm("mov %%rsp, %0""\n\t""mov %%rbp, %1":"=r"(esp),"=r"(ebp));

	printf("Fonction a() numero %d\n###################\n", fin);
	printf("esp vaut : %p\nebp vaut :%p\n", esp, ebp);
	printf("La variable locale i vaut :%p\nla variable locale j vaut :%p\n", &i, &j);
	printf("##################################\n\n");
	
	if(!fin) {
		fin++;
		aff_reg(i, j);
	}
 }
