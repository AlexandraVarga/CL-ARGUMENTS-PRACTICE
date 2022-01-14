/*

Autor: Alexandra-Ioana Varga
Problema 4, Laborator 7 - media aritmetica a unor numere din fisier
Stadiu: Nu am reusit sa implementez thread-urile
Ultima modificare: 15.04.2021

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define MAX 600
#define MAX_MAT 600

int st[MAX];
int dr[MAX];
double medie[MAX];

int to1, to2; //fisier
char bigBuf[MAX][MAX];
int dim = 0;
char cuvant[MAX];
int INTERVAL_LEN;
int NR_THREADS = 0;
int sz1 = 0, sz2 = 0;

typedef struct
{
    int currentValue; //pornim de la 0 si fiecare thread primeste urmatoarea valoare, se merge din INTERVAL_LEN in INTERVAL_LEN
    //int *st, *dr;           //capat stanga, dreapta
    //int sz1 ;
    //int sz2 ;
    double *medie;          //medie
    pthread_mutex_t *lock1; //pointer nu copie
    pthread_mutex_t *lock2; //pointer nu copie

} TH_PARAM;

///parsare fisier
void parsareArgumente(char *buf, char bufArg[MAX][MAX], int *cnt)
{
    char *pc = strtok(buf, " \n");
    while (pc)
    {
        strcpy(bufArg[(*cnt)++], pc); ///salvez in acest buffer argumentele parsate
        pc = strtok(NULL, " \n");
    }
}
///cautam numerele pentru a le face media
void searchNumbers(char content[MAX][MAX], int dim, char *cuvant)
{
    int cnt = 0;
    for (int i = 1; i < dim; i++)
    {
        //printf("%d %d\n", i, atoi(content[i]));
        if (i % 2 == 1)
        {
            st[sz1++] = atoi(content[i]);
            //printf("%d\n", atoi(content[i]));
        }
        else
        {
            dr[sz2++] = atoi(content[i]);
            //printf("%d\n", atoi(content[i]));
        }
    }
}
///se face media si se scrie in fisier
void medie_fisier(int *st, int *dr, double *medie, int size, int to1, int to2)
{
    char s[MAX];
    for (int i = 0; i < size; i++)
    {
        medie[i] = (st[i] + dr[i]) / 2.0;
    }
    for (int i = 0; i < size; i++)
    {
        //printf("%d %d %lf\n",st[i],dr[i], medie[i]);
        sprintf(s, "%d %d %d %lf\n",(int)pthread_self(), st[i], dr[i], medie[i]);
        write(to2, s, strlen(s));
    }
}
void afisare(int *v, int n)
{
    ///functie de afisare sa vad daca am extras bine numerele din fisier
    for (int i = 0; i < n; i++)
    {
        printf("%d ", v[i]);
    }
}
void *th_prim(void *arg)
{
    TH_PARAM *param = (TH_PARAM *)arg; //cast pt a putea accesa membrii parametrului

    int start, end, pos;
    char s[MAX];
    int i;
    for (;;) ///pentru ca fiecare thread sa poata alege alt interval din lista
    {
        //pthread_mutex_lock(param->lock1); //blocam lacat pt a proteja regiunea critica
        ///daca mai multe thread-uri pornesc de la acelasi currentValue => regiune critica (foloseste variabile comune)
        ///sincronizam cu lacat pt a excepta regiunea critica

        //pthread_mutex_lock(param->lock1); //blocam lacat pt a proteja regiunea critica

        start = param->currentValue;              //de unde pornesc pe inverval
        end = param->currentValue + INTERVAL_LEN; // unde isi termina intervalul thread-ul curent
        param->currentValue += INTERVAL_LEN;      //ma mut pe interval

        //printf("Intervale: %d %d\n", start, end);
        //pthread_mutex_unlock(param->lock1); //aici il deblocam => sincronizare efectuata

        for (i = start; i < end; i++)
        {
            medie[i] = (st[i] + dr[i]) / 2.0;
            sprintf(s, "%d %d %d %lf\n",(int)pthread_self(), st[i], dr[i], medie[i]);
            write(to1, s, strlen(s));
            //printf("%d %d\n", st[i], dr[i]);
        }
        return NULL;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    int from;
    int nr;
    int ok = 1;
    char auxString[MAX];
    int cntAux = 0;
    char buf;
    char content[MAX][MAX];

    if ((from = open("/home/alexandra/Desktop/VISUAL PROJECTS/numbers.in.txt", O_RDONLY)) < 0)
    {
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
        auxString[cntAux++] = buf;
    }

    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < strlen(content[i]); j++)
        {
            content[i][j] = NULL;
        }
    }
    strcpy(content[dim++], auxString);
    ///se parseaza si se delimiteaza numerele pentru media aritmetica
    parsareArgumente(auxString, bigBuf, &dim);
    searchNumbers(bigBuf, dim, cuvant);
    if ((to1 = creat("result_threads.out", 0666)) < 0)
    {
        perror("Eroare deschidere fisier dest.");
        exit(2);
    }
    if ((to2 = creat("results_main.out", 0666)) < 0)
    {
        perror("Eroare deschidere fisier dest.");
        exit(2);
    }
    ///medie_fisier(st, dr, medie, sz1, to1, to2);

    ///parte de thread-uri
    //medie_fisier(st, dr, medie, sz1, to1, to2);
    ///thread-uri
    TH_PARAM param;
    NR_THREADS = (dim - 1) / 2;
    pthread_t tids[NR_THREADS];
    //printf("%d\n", NR_THREADS);
    ///am nevoie de un thread pe fiecare interval
    INTERVAL_LEN = 1; //in cate intervale impartim intervalul nostru - n thread-uri
    //printf("%d\n",INTERVAL_LEN);
    //printf("%d\n", INTERVAL_LEN);

    param.currentValue = 0;                            // de la ce interval pornim thread-urile
    //pthread_t tids[NR_THREADS];                        //vector de id-uri de thread-uri
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
        //write(to1, tids[i], sizeof(int));
        //printf("%d ", tids[i]);
    }

    medie_fisier(st, dr, medie, sz1, to1, to2);
    //printf("Numar cuvinte cautate: %d\n", param.res);

    ///deinitializare lacate
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);

    close(from);
}

