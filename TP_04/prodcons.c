#include "sem.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 5                        /* nombre de places dans le tampon */
#define COUNTER 20

void produire_objet();
void utiliser_objet();
void mettre_objet();
void retirer_objet();
void consommateur (void* arg);
void producteur (void* arg);

static struct sem_s mutex, vide, plein;

static int obj_produis = 0;
static int obj_consomme = 0;

void producteur (void* arg)
{
  while (1) {
    produire_objet();           /* produire l'objet suivant */
    sem_down(&vide);                  /* dec. nb places libres */
    sem_down(&mutex);                 /* entree en section critique */
    mettre_objet();              /* mettre l'objet dans le tampon */
    sem_up(&mutex);                   /* sortie de section critique */
    sem_up(&plein);                   /* inc. nb place occupees */
  }
}

void consommateur (void* arg)
{
  while (1) {
    sem_down(&plein);                 /* dec. nb emplacements occupes */
    sem_down(&mutex);                 /* entree section critique */
    retirer_objet ();           /* retire un objet du tampon */
    sem_up(&mutex);                   /* sortie de la section critique */
    sem_up(&vide);                    /* inc. nb emplacements libres */
    utiliser_objet();            /* utiliser l'objet */
  }
}

void produire_objet(){
    int i;
    puts("Je produis un objet");
    printf("Mutex : %d, Vide : %d, Plein :%d, Sum : %d\n\n", mutex.sem_cpt, vide.sem_cpt, plein.sem_cpt, vide.sem_cpt + plein.sem_cpt);
    for(i = 0; i < (1 << COUNTER); i++);
}

void mettre_objet(){
    int i;

    obj_produis++;
    puts("Je mets un objet");
    printf("Produis : %d, Consommé : %d, Delta :%d\n", obj_produis, obj_consomme, obj_produis - obj_consomme);
    printf("Mutex : %d, Vide : %d, Plein :%d, Sum : %d\n\n", mutex.sem_cpt, vide.sem_cpt, plein.sem_cpt, vide.sem_cpt + plein.sem_cpt);
    for(i = 0; i < (1 << COUNTER); i++);
}

void retirer_objet(){
    int i;
    obj_consomme++;

    puts("Je retire un objet");
    printf("Produis : %d, Consommé : %d, Delta :%d\n", obj_produis, obj_consomme, obj_produis - obj_consomme);
    printf("Mutex : %d, Vide : %d, Plein :%d, Sum : %d\n\n", mutex.sem_cpt, vide.sem_cpt, plein.sem_cpt, vide.sem_cpt + plein.sem_cpt);
    for(i = 0; i < (1 << COUNTER); i++);
}

void utiliser_objet(){
    int i;
    puts("J'utilise un objet");
    printf("Mutex : %d, Vide : %d, Plein :%d, Sum : %d\n\n", mutex.sem_cpt, vide.sem_cpt, plein.sem_cpt, vide.sem_cpt + plein.sem_cpt);
    for(i = 0; i < (1 << COUNTER); i++);
}

int main() {
    srand(time(NULL));

    sem_init(&mutex, 1);                /* controle d'acces au tampon */
    sem_init(&vide, N);                 /* nb de places libres */
    sem_init(&plein, 0);                /* nb de places occupees */

    create_ctx(16384, producteur, NULL);
    create_ctx(16384, consommateur, NULL);

    start_sched();

    exit(EXIT_SUCCESS);
}