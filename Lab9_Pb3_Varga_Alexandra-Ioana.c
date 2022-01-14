/*

Autor: Alexandra-Ioana Varga
Problema 3, Laborator 9 - thread-uri care acceseaza aceiasi resursa partajata
Stadiu: Rezolvata
Ultima modificare: 14.05.2021

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int sir[] = {1, -2, 3, -4, 5, -6, 7, -8, 9, -10};
sem_t wrt;
pthread_mutex_t mutex;

int cnt = 1;
int numreader = 0;

///structura acces resursa partajata
typedef struct
{
    int rd_wr; //daca sunt scriitor sau cititor
    int i_sir; //pe ce pozitie din sir
} resursa;

void afisare(int *v, int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", v[i]);
    printf("\n");
}
///thread cititor
void *writer(void *wno)
{
    //fac cast
    resursa *res = (resursa *)wno;

    sem_wait(&wrt);
    //cnt=cnt*2;
    sir[res->i_sir] = res->rd_wr; ///scriu in sir
    //printf("Writer %d modified cnt to %d at %d\n", res->rd_wr, res->i_sir, sir[res->i_sir]);
    sem_post(&wrt);
    printf("Writer %d modified cnt to %d at %d\n", res->rd_wr, res->i_sir, sir[res->i_sir]);
}
///cititor reader
void *reader(void *rno)
{
    //fac cast
    resursa *res = (resursa *)rno;

    pthread_mutex_lock(&mutex);
    numreader++;
    if (numreader == 1)
    {
        sem_wait(&wrt);
    }
    pthread_mutex_unlock(&mutex);
    printf("Reader %d modified cnt to %d at %d\n", res->rd_wr, res->i_sir, sir[res->i_sir]);

    pthread_mutex_lock(&mutex);
    numreader--;
    if (numreader == 0)
    {
        sem_post(&wrt);
    }
    pthread_mutex_unlock(&mutex);
}

int main()
{
    //afisare(sir ,N);
    pthread_t read[10], write[5];
    ///resursa partajata
    int N = sizeof(sir) / sizeof(int);
    pthread_mutex_init(&mutex, NULL);
    sem_init(&wrt, 0, 1);
    srand(time(NULL)); ///initializez generatorul de random numbers
    for (int i = 0; i < 10; i++)
    {
        ///initializare readeri
        resursa reads;
        reads.i_sir = i;
        reads.rd_wr = rand()%N;
        pthread_create(&read[i], NULL, (void *)reader, (void *)&sir[i]);
    }
    for (int i = 0; i < 5; i++)
    {
        ///initializare writeri
        resursa writes;
        writes.i_sir = i;
        writes.rd_wr = rand()%N;
        pthread_create(&write[i], NULL, (void *)writer, (void *)&sir[i]);
    }
    ///10 thread-uri cititor
    for (int i = 0; i < 10; i++)
    {
        pthread_join(read[i], NULL);
    }
    ///5 thread-uri scriitor
    for (int i = 0; i < 5; i++)
    {
        pthread_join(write[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}
