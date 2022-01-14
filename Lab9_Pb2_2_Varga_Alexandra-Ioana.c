/*

Autor: Alexandra-Ioana Varga
Problema 2.2, Laborator 9 scriere medie numere in fisiere, sincronizare
Stadiu: Rezolvata partial - fara thread-uri adunate in main
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
#define MAX_MAT 600

int st[MAX];//membru stanga
int dr[MAX];//membru drept
double medie[MAX];//media celor 2 membrii

int to1, to2; //fisier
char bigBuf[MAX][MAX];//extragere linie cu linie din fisier
int dim = 0;//dimensiune linii fisier
char cuvant[MAX];
int INTERVAL_LEN;//interval pe care mergem cu thread-urile
int NR_THREADS = 0;//nr de thread-uri
int sz1 = 0, sz2 = 0;//contoare pt dimeniuni vectori extragere mebrii stang-drept

sem_t sem;

typedef struct
{
    int currentValue; //pornim de la 0 si fiecare thread primeste urmatoarea valoare, se merge din INTERVAL_LEN in INTERVAL_LEN

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
        if (i % 2 == 1)
        {
            st[sz1++] = atoi(content[i]);//membru stanga
        }
        else
        {
            dr[sz2++] = atoi(content[i]);//membru dreapta
        }
    }
}
///se face media si se scrie in fisier
void medie_fisier(int *st, int *dr, double *medie, int cnt, int to1, int to2, int *tids)
{
    char s[MAX];
    for (int i = 0; i < cnt; i++)
    {
        medie[i] = (st[i] + dr[i]) / 2.0;//se face media
    }
    for (int i = 0; i < cnt; i++)
    {
        //sprintf(s, "%d %d %d %lf\n",tids[i], st[i], dr[i], medie[i]);
        sprintf(s, "%d %d %d %lf\n",(int)pthread_self(), st[i], dr[i], medie[i]);//se scrie in fisier cu id-ul thread-ului
        write(to2, s, strlen(s));
    }
}
///functie pentru thread
void *th_prim(void *arg)
{
    TH_PARAM *param = (TH_PARAM *)arg; //cast pt a putea accesa membrii parametrului

    int start, end, pos;//se calculeaza intervalele pe care merge thread-urile
    char s[MAX];
    int i;
    //for (;;) ///pentru ca fiecare thread sa poata alege alt interval din lista
    {
        ///regiune critica pe care o sincronizam cu semafoare
        sem_wait(&sem);
        start = param->currentValue;              //de unde pornesc pe inverval
        end = param->currentValue + INTERVAL_LEN; // unde isi termina intervalul thread-ul curent
        param->currentValue += INTERVAL_LEN;      //ma mut pe interval
        sem_post(&sem);
        //printf("Intervale: %d %d\n", start, end);

        for (i = start; i < end; i++)
        {
            medie[i] = (st[i] + dr[i]) / 2.0;
            sprintf(s, "%d %d %d %lf\n",(int)pthread_self(), st[i], dr[i], medie[i]);//id thread + membrii + medie
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
    sem_init(&sem, 0, 1);

    if ((from = open("/home/alexandra/Desktop/VISUAL PROJECTS/numbers1.in.txt", O_RDONLY)) < 0)
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
        auxString[cntAux++] = buf;//extrag din fisier linie cu linie
    }
    //initializare buffer
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < strlen(content[i]); j++)
        {
            content[i][j] = NULL;
        }
    }
    strcpy(content[dim++], auxString);//se pune content-ul din fisier
    ///se parseaza si se delimiteaza numerele pentru media aritmetica
    parsareArgumente(auxString, bigBuf, &dim);//se parseaza argumentele
    searchNumbers(bigBuf, dim, cuvant);//se face media
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
    ///thread-uri
    TH_PARAM param;
    NR_THREADS = (dim - 1) / 2;//cate thread-uri avem nevoie - cate unul pt fiecare 2 membrii scris in fiser
    pthread_t tids[NR_THREADS];
    ///am nevoie de un thread pe fiecare interval
    INTERVAL_LEN = 1; //in cate intervale impartim intervalul nostru - n thread-uri
    param.currentValue = 0;                            // de la ce interval pornim thread-urile
    ///creare thread-uri cu argument functia mea 
    for (int i = 0; i < NR_THREADS; i++)
    {
        pthread_create(&tids[i], NULL, th_prim, &param);
    }
    //medie_fisier(st, dr, medie, sz1, to1, to2, tids);
    ///se asteapta dupa thread-uri
    for (int i = 0; i < NR_THREADS; i++)
    {
        pthread_join(tids[i], NULL);
    }
    ///thread principal
    medie_fisier(st, dr, medie, sz1, to1, to2, tids);
    sem_destroy(&sem);

    close(from);
}
