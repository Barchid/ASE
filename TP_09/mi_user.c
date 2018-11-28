 #include "mi_syscall.h"
 
 // récupérer la page physique à partir de la page virtuelle et du process
 // ON SUPPOSE que N doit être plus petit que 255
 static int ppage_of_vpage(int process, unsigned vpage) {
	 // Le processus a droit à accédder à page de 0 à N/2-1 (car il a N/2 pages)
	 if(vpage > N/2-1) {
		 return -1;
	 }
	 
	 // SI je suis le process 0
	 if(process == 0) {
		 // Quand je veux vpage 0, je dois retourner page physique 1, etc
		 // En fait, je saute juste la page 0
		 return vpage + 1;
	 }
	 // SI je suis le process 1
	 else if(process == 1){
		 return vpage + N/2 + 1;
	 }
	 // Je suis un numéro erroné
	 else {
		 return -1;
	 }
 }