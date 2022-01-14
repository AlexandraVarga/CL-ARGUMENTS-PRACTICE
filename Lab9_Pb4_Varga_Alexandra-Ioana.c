/*

Autor: Alexandra-Ioana Varga
Problema 4, Laborator 9 - problema NaCl 
Stadiu: Rezolvata
Ultima modificare: 14.05.2021

*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX 10000

///structura pentru a sti cate molecule vreau sa creez
typedef struct
{
    int number;
    int val[MAX];
} resursa;

/// AU RAMAS DENUMIRILE DE SEMAFOARE DE LA PROBLEA CU H20 (2 MOLECULE DE O SI UNA DE H)
/// AICI AM DOAR 1 MOLECULA DE NA SI O MOLECULA DE CL => NACL 
sem_t sem_H;
sem_t sem_2O_1; 
sem_t sem_H_2O; 
sem_t sem_2O_H;
sem_t sem_print_H_2O;

///thread pentru creare NaCl din Na
void *create_H(void *args)
{
    sem_wait(&sem_H_2O);//sleep Na_Cl
    sem_post(&sem_H);//wake up Na
    sem_wait(&sem_2O_1);///sleep Cl
    sem_wait(&sem_print_H_2O);//sleep Na_Cl from Na

    printf("%d NaCl from Na\n", pthread_self());

    sem_post(&sem_print_H_2O);//wake up printf
    sem_post(&sem_H_2O);//sleep Na_Cl

    return NULL;
}

///thread pentru creare NaCl din Cl
void *create_2O(void *args)
{
    sem_wait(&sem_2O_H);//sleep Cl
    sem_post(&sem_2O_1);//wake up Na
    sem_wait(&sem_H);//sleep C;
    sem_wait(&sem_print_H_2O);//sleep Na_Cl from Cl

    printf("%d NaCl from Cl\n", pthread_self());

    sem_post(&sem_print_H_2O);//sleep printf
    sem_post(&sem_2O_H);//sleep Na_Cl from Cl

    return NULL;
}
int main(int argc, char *argv[])
{
    int n;
    //int number = 0;
    //int val[MAX];
    int numar = 0;

    sem_init(&sem_print_H_2O, 0, 1);
    sem_init(&sem_H, 0, 0);
    sem_init(&sem_H_2O, 0, 1);
    sem_init(&sem_2O_1, 0, 0);
    sem_init(&sem_2O_H, 0, 1);

    srand(time(NULL));

    pthread_t thread;
    resursa res;//vector in care retin val
    res.number=0;
    for (int i = 0; i < 10; i++)//10 molecule create
    {
        numar = rand() % 2; //0 sau 1
        res.number++;//cate am creat
        res.val[res.number] = res.number;//pun id-ul
        if (numar == 0)
        {
            ///incerc sa creez Na 
            printf("%d Na is created\n", pthread_self());
            pthread_create(&thread, NULL, create_H, &res.val[res.number]);
        }
        else
        {
            ///incerc sa creez Cl
            printf("%d Cl is created \n", pthread_self());
            pthread_create(&thread, NULL, create_2O, &res.val[res.number]);
        }
    }
    sem_destroy(&sem_print_H_2O);
    sem_destroy(&sem_H);
    sem_destroy(&sem_H_2O);
    sem_destroy(&sem_2O_1);
    sem_destroy(&sem_2O_H);
}