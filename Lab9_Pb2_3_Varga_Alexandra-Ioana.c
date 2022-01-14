/*

Autor: Alexandra-Ioana Varga
Problema 2.3, Laborator 9 - secvente de biti din fisier + sincronizare semafoare
Stadiu: Rezolvata
Ultima modificare: 13.05.2021

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

#define MAX 600

int NR_THREADS; ///cate thread-uri folosesc
int INTERVAL_LEN;//interval pe care ma deplasez
int N=4;//n pentru histograma
char auxString[MAX];///preiau din fisier bitii
int cntAux = 0;//dimensiune biti
int frecventa[255];///frecventa poate fi masurata pana la 2^8-1 - transform in decimal

sem_t sem;

typedef struct
{
    int currentValue;       //pornim de la 0 si fiecare thread primeste urmatoarea valoare, se merge din INTERVAL_LEN in INTERVAL_LEN
} TH_PARAM;

///calculez nr din binar in decimal
int my_pow (int base, int expo)
{
    int myPow=1;
    for (int i=0;i<expo;i++)
    {
        myPow*=2;
    }
    return myPow;
}

///calculez din decimal in binar
void my_binary (int n)
{
    int binar[N];
    for (int i=0;i<N;i++)
    {
        binar[i]=0;
    }
    int x=0;
    while(n)
    {
        binar[x++]=n%2;
        n/=2;
    }
    for (int i=N-1;i>=0;i--)
        printf("%d", binar[i]);
    
    printf(" ");
}
///pentru parte thread-uri
void *th_prim(void *arg)
{
    TH_PARAM *param = (TH_PARAM *)arg; //cast pt a putea accesa membrii parametrului
    int start, end, pos;
    int secventa=0;
    int i;
    for (;;)///pentru ca fiecare thread sa poata alege alt interval din lista
    {
        ///regiune critica pe care o sincronizam cu semafoare
        sem_wait(&sem);
        start = param->currentValue; //de unde pornesc pe inverval
        end = param->currentValue + INTERVAL_LEN; // unde isi termina intervalul thread-ul curent
        param->currentValue += INTERVAL_LEN; //ma mut pe interval
        sem_post(&sem);
        
        //printf("Interval %d to %d\n", start, end);

        for (i = start; i < end; i++)
        {
            //printf("%c ", auxString[i]);
            if (auxString[i]=='1')
            {
                secventa = secventa + my_pow(2,(N-1)-(i%N));
            }
        }
        frecventa[secventa]++;
        secventa=0;
        return NULL;
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int from;///fisier
    int nr;
    int ok = 1;
    char buf;
    char content[MAX][MAX];//extregere string din buffer
    sem_init(&sem, 0, 1);

    ///fisier de teste
     if ((from = open("/home/alexandra/Desktop/VISUAL PROJECTS/numbers2.in.txt", O_RDONLY)) < 0)
    {
        perror("Eroare deschidere fisier sursa");
        exit(1);
    }

    while (nr = read(from, &buf, sizeof(char)) != 0)///citire din buffer
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        auxString[cntAux++] = buf;
    }
    //printf("%d\n",cntAux);
    
    NR_THREADS = (cntAux-1)/N;//nr de thread-uri folosite
    INTERVAL_LEN = N;//interval pe care ma deplasez

    TH_PARAM param;
    param.currentValue = 0; // de la ce interval pornim thread-urile

    pthread_t tids[NR_THREADS];                        //vector de id-uri de thread-uri

    ///creare thread-uri cu argument functia mea care numara cate nr prime sunt in [n,m]
    for (int i = 0; i < NR_THREADS; i++)
    {
        pthread_create(&tids[i], NULL, th_prim, &param);
    }

    ///se asteapta dupa thread-uri
    for (int i = 0; i < NR_THREADS; i++)
    {
        pthread_join(tids[i], NULL);
    }
    
    ///afisam frecventele sabloanelor gasite
    for (int i=0; i< my_pow(2,N)-1; i++)
    {
        if (frecventa[i]!=0)
        {
            my_binary(i);
            printf("%d\n",frecventa[i]);
        }
    }
    sem_destroy(&sem);
}
