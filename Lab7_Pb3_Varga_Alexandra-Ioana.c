/*

Autor: Alexandra-Ioana Varga
Problema 3, Laborator 7 - frecventa sablon in fisiere de date - numarul liniei unde apare sablonul
Stadiu: Rezolvata cu thread-uri
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

#define MAX 600
#define MAX_MAT 600

char bigBuf[MAX][MAX]; ///cuvinte parsate din fisier
#define NR_THREADS 6   ///folosesc 10 thread-uri
int dim = 0;           //dimensiune matrice in care extrag cuvintele din fisiere
char cuvant[MAX];      ///cuvant cautat
int to;                ///fisier in care scriu thread-urile

int INTERVAL_LEN;

typedef struct
{
    int currentValue;       //pornim de la 0 si fiecare thread primeste urmatoarea valoare, se merge din INTERVAL_LEN in INTERVAL_LEN
    pthread_mutex_t *lock1; //pointer nu copie
    pthread_mutex_t *lock2; //pointer nu copie

} TH_PARAM;

///functie de parsare argumente fisier
void parsareArgumente(char *buf, char bufArg[MAX][MAX], int *cnt)
{
    char *pc = strtok(buf, "\n");
    while (pc)
    {
        strcpy(bufArg[(*cnt)++], pc); ///salvez in acest buffer argumentele parsate
        pc = strtok(NULL, "\n");
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

        start = param->currentValue;              //de unde pornesc pe inverval
        end = param->currentValue + INTERVAL_LEN; // unde isi termina intervalul thread-ul curent
        param->currentValue += INTERVAL_LEN;      //ma mut pe interval

        //printf("Numb of primes from %d to %d\n", start, end);
        //pthread_mutex_unlock(param->lock1); //aici il deblocam => sincronizare efectuata
        for (i = start; i < end; i++)
        {
            if (strstr(bigBuf[i], cuvant) != 0)
            {
                ///daca e primul cuvant cautat
                if (strstr(bigBuf[i], cuvant) - bigBuf[i] == 0 && strchr(" .,/'][;-\n", bigBuf[i][(strstr(bigBuf[i], cuvant) - bigBuf[i]) + strlen(cuvant)]) != 0)
                {
                    //printf("%d ", i - 1);
                    sprintf(s, "%d ", i - 1); ///se linia gasita in fisier
                    write(to, s, strlen(s));
                    //printf("%s", content[i]);
                    //printf("\n");
                }
                ///daca este undeva in interiorul propozitiei
                else if (strchr(" .,/'][;-", bigBuf[i][(strstr(bigBuf[i], cuvant) - bigBuf[i]) - 1]) != 0 && strchr(" .,/'][;-\n", bigBuf[i][(strstr(bigBuf[i], cuvant) - bigBuf[i]) + strlen(cuvant)]) != 0)
                //printf("%d\n",strstr(content[i], cuvant)-content[i]);
                {
                    //printf("%d ", i - 1);
                    sprintf(s, "%d ", i - 1); ///se linia gasita in fisier
                    write(to, s, strlen(s));
                    //printf("%s", content[i]);
                    //printf("\n");
                }
                else if (strlen(bigBuf[i]) - strlen(cuvant) == strstr(bigBuf[i], cuvant) - bigBuf[i]) ///daca e la finalul propozitiei
                {
                    //printf("%d ", i - 1);
                    sprintf(s, "%d ", i - 1); ///se linia gasita in fisier
                    write(to, s, strlen(s));
                    //printf("%s", content[i]);
                    //printf("\n");
                }
            }
        }
        //printf("\n");
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
    char s[MAX];
    ///executabil, cuvant, argv-2 fisiere
    if (argc != strlen(argv) - 2)
    {
        printf("UTILIZARE ARGUMENTE:%d %d\n", argc, strlen(argv));
        exit(0);
    }
    strcpy(cuvant, argv[1]);
    if ((to = creat("rezultat.txt", 0666)) < 0)
    {
        perror("Eroare deschidere fisier dest.");
        exit(2);
    }
    for (int i = 2; i < strlen(argv) - 2; i++)
    {
        if ((from = open(argv[i], O_RDONLY)) < 0)
        {
            perror("Eroare deschidere fisier sursa");
            exit(1);
        }
        //printf("\n%s\n", argv[i]); //fisierele
        sprintf(s, "\n%s\n", argv[i]); //fisierele
        write(to, s, strlen(s));
        while (nr = read(from, &buf, sizeof(char)) != 0)
        {
            if (nr < 0)
            {
                perror("Eroare citire din fisier sursa");
                exit(3);
            }
            auxString[cntAux++] = buf;
        }
        strcpy(content[dim++], auxString);
        ///pentrue fiecare fisier deschis caut sablonul
        parsareArgumente(auxString, bigBuf, &dim);

        //searchWord(bigBuf, dim, cuvant, to);
        ///parte de thread-uri
        TH_PARAM param;

        INTERVAL_LEN = dim / NR_THREADS; //in cate intervale impartim intervalul nostru - n thread-uri

        //printf("%d\n",INTERVAL_LEN);

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

        //printf("Numar cuvinte cautate: %d\n", param.res);

        ///deinitializare lacate
        pthread_mutex_destroy(&lock1);
        pthread_mutex_destroy(&lock2);

        dim = 0;
        cntAux = 0;
        memset(auxString, 0, 100);
        for (int i = 0; i < MAX; i++)
        {
            for (int j = 0; j < strlen(content[i]); j++)
            {
                content[i][j] = NULL;
            }
        }
        close(from);
    }
}
