/*

Autor: Alexandra-Ioana Varga
Problema 2, Laborator 7 - frecventa sablon in fisiere date
Stadiu: Am implementat thread-urile, segmentation fault la lacate :)
Ultima modificare: 12.05.2021 

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define NR_THREADS 6 ///folosesc 10 thread-uri

#define MAX 600
#define MAX_MAT 600

char bigBuf[MAX][MAX]; ///cuvinte parsate din fisier
int dim = 0;
char cuvant[MAX]; ///cuvant cautat

int INTERVAL_LEN;

typedef struct
{
    int res;                //cate nr prime am numarat in intervalul [n,m]
    int currentValue;       //pornim de la 0 si fiecare thread primeste urmatoarea valoare, se merge din INTERVAL_LEN in INTERVAL_LEN
    pthread_mutex_t *lock1; //pointer nu copie
    pthread_mutex_t *lock2; //pointer nu copie

} TH_PARAM;

///functie de exragere cuvinte
void parsareArgumente(char *buf, char bufArg[MAX][MAX], int *cnt) ///se parseaza sabloanele din sir pentru a le numara
{
    char *pc = strtok(buf, " ./;'[,*&\n");
    while (pc)
    {
        strcpy(bufArg[(*cnt)++], pc); ///salvez in acest buffer argumentele parsate
        pc = strtok(NULL, " ./;'[,*&\n");
    }
}

///parte de thread-uri
void *th_prim(void *arg)
{
    TH_PARAM *param = (TH_PARAM *)arg; //cast pt a putea accesa membrii parametrului

    int start, end, pos;
    int i;
    for (;;) ///pentru ca fiecare thread sa poata alege alt interval din lista
    {
        //pthread_mutex_lock(param->lock1); //blocam lacat pt a proteja regiunea critica
        ///daca mai multe thread-uri pornesc de la acelasi currentValue => regiune critica (foloseste variabile comune)
        ///sincronizam cu lacat pt a excepta regiunea critica

        start = param->currentValue;              //de unde pornesc pe inverval
        end = param->currentValue + INTERVAL_LEN; // unde isi termina intervalul thread-ul curent
        param->currentValue += INTERVAL_LEN;      //ma mut pe interval

        //printf("Numb of primes from %d to %d\n", start, end);
        //pthread_mutex_unlock(param->lock1); //aici il deblocam => sincronizare efectuata
        for (i = start; i < end; i++)
        {
            if (strcmp(bigBuf[i], cuvant) == 0) ///daca gasesc cuvantul il contorizez
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
    int from; ///fisiere ce vor fi deschise
    int nr;
    int ok = 1;
    char auxString[MAX]; ///unde salvez auxiliar
    int cntAux = 0;
    char buf;
    char content[MAX][MAX];
    ///executabil, cuvant, argv-2 fisiere
    if (argc != strlen(argv) - 2) ///argumente incorecte
    {
        printf("UTILIZARE ARGUMENTE:%d %d\n", argc, strlen(argv));
        exit(0);
    }
    strcpy(cuvant, argv[1]); //cuvant cautat
    for (int i = 2; i < strlen(argv) - 2; i++)
    {
        if ((from = open(argv[i], O_RDONLY)) < 0) ///incerc sa deschid fisier
        {
            printf("%s\n", argv[i]);
            perror("Eroare deschidere fisier sursa");
            exit(1);
        }
        while (nr = read(from, &buf, sizeof(char)) != 0)
        {
            if (nr < 0)
            {
                perror("Eroare citire din fisier sursa");
                exit(3);
            }
            auxString[cntAux++] = buf; ///sir auxiliar unde salvez tot buffer-ul din fisier
        }
        auxString[cntAux++] = '\n';
        strcpy(content[dim++], auxString); ///retin liniile in buffer
        close(from);
    }

    parsareArgumente(auxString, bigBuf, &dim); ///parsez buffer-ul astfel incat sa caut cuvinetele sablon

    //printf("%d\n", searchWord(bigBuf, dim, cuvant)); ///in tot fisierul
    // printf("%d\n",dim);
    // for (int i=0;i<dim;i++)
    // {
    //     printf("%s\n",bigBuf[i]);
    // }
    ///parte de thread-uri

    TH_PARAM param;

    INTERVAL_LEN = dim / NR_THREADS; //in cate intervale impartim intervalul nostru - n thread-uri

    //printf("%d\n",INTERVAL_LEN);

    param.res = 0;                                     //cate nr prime avem
    param.currentValue = 0;                            // de la ce interval pornim thread-urile
    pthread_t tids[NR_THREADS];                        //vector de id-uri de thread-uri
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

    printf("Numar cuvinte cautate: %d\n", param.res);

    ///deinitializare lacate
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
}
