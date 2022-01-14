/*

Autor: Alexandra-Ioana Varga
Problema 3.1, Laborator 10 - Problema formare H20
Stadiu: Partial Rezolvata partial - rezultatele nu cred ca sunt bune
Ultima modificare: 15.05.2021

*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX 10000
//structura pentru a retine cate molecule dorim sa cream 
typedef struct
{
    int number;//index
    int val[MAX];//vector
} resursa;

sem_t sem_H;
sem_t sem_2O_1; 
sem_t sem_2O_2;
sem_t sem_H_2O; 
sem_t sem_2O_H;
sem_t sem_print_H_2O;

///incercare solutie cu semafoare
void *create_H(void *args)
{
    sem_wait(&sem_H_2O);//sleep h20
    sem_post(&sem_H);//wake up h
    sem_wait(&sem_2O_1);//sleep o
    sem_wait(&sem_2O_2);//sleep o
    sem_wait(&sem_print_H_2O);//sleep printf

    printf("%d H2O from H\n", pthread_self());

    sem_post(&sem_print_H_2O);//wake up printf
    sem_post(&sem_H_2O);//wake up h20

    return NULL;
}
///incercare solutie cu semafoare
void *create_2O(void *args)
{
    sem_wait(&sem_2O_H);//sleep 2oh
    sem_post(&sem_2O_1);//wake up o
    sem_wait(&sem_2O_2);//sleep o
    sem_wait(&sem_H);//sleep h
    sem_wait(&sem_print_H_2O);//sleep h2o

    printf("%d H2O from 2O\n", pthread_self());

    sem_post(&sem_print_H_2O);//wake up printf
    sem_post(&sem_2O_H);//wake up o2h

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
    sem_init(&sem_2O_2, 0, 1);
    sem_init(&sem_2O_H, 0, 1);

    srand(time(NULL));

    pthread_t thread;
    resursa res;//vector in care retin val
    res.number=0;
    for (int i = 0; i < 10; i++)//10 molecule create
    {
        numar = rand() % 2; //0 sau 1
        res.number++;
        res.val[res.number] = res.number;
        if (numar == 0)
        {
            printf("%d H is created\n", pthread_self());
            pthread_create(&thread, NULL, create_H, &res.val[res.number]);
        }
        else
        {
            printf("%d 2O is created \n", pthread_self());
            pthread_create(&thread, NULL, create_2O, &res.val[res.number]);
        }
    }
    sem_destroy(&sem_print_H_2O);
    sem_destroy(&sem_H);
    sem_destroy(&sem_H_2O);
    sem_destroy(&sem_2O_1);
    sem_destroy(&sem_2O_2);
    sem_destroy(&sem_2O_H);
}