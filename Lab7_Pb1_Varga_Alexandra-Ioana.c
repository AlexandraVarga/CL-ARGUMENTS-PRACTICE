/*

Autor: Alexandra-Ioana Varga
Problema 1, Laborator 7 - numere prime din interval [n,m]
Stadiu: Rezolvata cu mutex-uri
Ultima modificare: 11.05.2021

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define NR_THREADS 5 ///folosesc 10 thread-uri

int INTERVAL_LEN;
int n; //capat interval n
int m; //capat interval m

typedef struct
{
    int res;                //cate nr prime am numarat in intervalul [n,m]
    int currentValue;       //pornim de la 0 si fiecare thread primeste urmatoarea valoare, se merge din INTERVAL_LEN in INTERVAL_LEN
    pthread_mutex_t *lock1; //pointer nu copie
    pthread_mutex_t *lock2; //pointer nu copie
} TH_PARAM;

int prim(int n) ///functie care determina daca un numar este prim
{
    if (n < 2)
        return 0;
    if (n > 2 && n % 2 == 0)
        return 0;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0)
            return 0;
    return 1;
}

///pentru parte thread-uri
void *th_prim(void *arg)
{
    TH_PARAM *param = (TH_PARAM *)arg; //cast pt a putea accesa membrii parametrului
    int start, end, pos;
    int i;
    for (;;)///pentru ca fiecare thread sa poata alege alt interval din lista
    {
        pthread_mutex_lock(param->lock1); //blocam lacat pt a proteja regiunea critica

        ///daca mai multe thread-uri pornesc de la acelasi currentValue => regiune critica (foloseste variabile comune)
        ///sincronizam cu lacat pt a excepta regiunea critica

        start = param->currentValue; //de unde pornesc pe inverval
        end = param->currentValue + INTERVAL_LEN; // unde isi termina intervalul thread-ul curent
        param->currentValue += INTERVAL_LEN; //ma mut pe interval

        
        //printf("Numb of primes from %d to %d\n", start, end);

        pthread_mutex_unlock(param->lock1); //aici il deblocam => sincronizare efectuata

        for (i = start; i < end; i++)
        {
            //printf("%d ",i);
            if (prim(i)) //verific daca am nr prime pe interval
            {
                param->res++;
            }
            
        }
        //printf("\n");
        return NULL;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    int cntPrim = 0; ///numar cate nr prime sunt in interval
    if (argc != 3)
    {
        printf("UTILIZARE ARGUMENTE:\n", argv[0]);
        exit(0);
    }

    n = atoi(argv[1]); //n - luat ca argument
    m = atoi(argv[2]); //m - luat ca argument

    INTERVAL_LEN = ((m - n) + 1) / NR_THREADS; //in cate intervale impartim intervalul nostru - n thread-uri
    ///parte de thread-uri
    TH_PARAM param;
    param.res = 0;          //cate nr prime avem
    param.currentValue = n; // de la ce interval pornim thread-urile

    pthread_t tids[NR_THREADS];                        //vector de id-uri de thread-uri
    /*
        Am incercat sa folosesc 2 lacate deoarece am crezut ca exista 2 regiuni critice
        dupa ce am sters lacatul 2 din program am primit segmentation fault si nu am idee de ce, am renuntat si l-am lasat aici desi
        nu il folosesc.
    */
    pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER; //lacat 1 initializare - pt sincronizare zona critica punct interval comun
    pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER; //lacat 2 initializare - pt sincronizare a thread-urilor care vor sa contorizeze simultan numere prime

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
    
    printf("Numere prime: %d\n", param.res);

    ///deinitializare lacate
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
}
