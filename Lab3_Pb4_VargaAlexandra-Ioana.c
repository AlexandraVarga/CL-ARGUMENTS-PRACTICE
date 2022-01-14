/*

Autor: Alexandra-Ioana Varga
Problema 4, Laboratorul 3
Status: Rezolvata Partial - ineficient
Data crearii: 22.03.2021
Data ultimei modificari: 25.03.2021

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 512
///rezultat dupa introducere secventa in buffer
void seeResult (char *buf, int dim)
{
    for (int i=0; i<dim; i++)
    {
        printf("%c",buf[i]);
    }
}
int main (int argc, char** argv)
{
    int from, to, nr, nw;
    char buf;
    char aux[BUFSIZE];
    ///verificare argumente
    if (argc != 5)
    {
        printf("Utilizare: %s fis_sursa fis_dest\n",
               argv[0]);
        exit(0);
    }
    //argument Insert
    if (strcmp(argv[1],"Insert")!=0)
    {
        perror("Comanda Invalida");
        exit(4);
    }
    //argument fisier
    if ((from = open(argv[2], O_RDONLY)) < 0)
    {
        perror("Eroare deschidere fisier sursa");
        exit(1);
    }

    if ((to = creat("dest.txt", O_WRONLY)) < 0)
    {
        perror("Eroare deschidere fisier dest.");
        exit(2);
    }
    ///argument pozitie
    int poz = atoi(argv[3]);
    ///argument sir
    char *s=argv[4];
    //printf("%d\n",poz);
    //printf("%s\n",s);
    int cnt=0;
    int pAux=0;
    ///parcurg caracter cu caracter fisierul
    while((nr = read(from, &buf, sizeof(char))) != 0)
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        ///adaug in buffer pana inaintea de pozitia la care se insereaza noua secventa
        if (cnt<poz-1)
        {
            aux[pAux++]=buf;
        }
        cnt++;
    }
    ///adaug secventa
    for (int i=0; i<strlen(s); i++)
    {
        aux[pAux++]=s[i];
    }

    aux[pAux]='\0';

    ///repozitionare pointer la fisier de la pozitia de unde am ramas inainte sa adaug in buffer
    lseek(from, poz, SEEK_SET);
    while((nr = read(from, &buf, sizeof(char))) != 0)
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        aux[pAux++]=buf;

    }
    ///afisez rezultatul pe ecran deoarece am o eroare cand incerc sa scriu in fisier
    seeResult(aux,pAux);
    /*for (int i=0; i<pAux; i++)
    {
        printf("%c",aux[i]);
        if ((nw=write(to, aux[i], sizeof(char))) < 0)
        {
            perror("Eroare scriere in fisier dest.");
            exit(4);
        }
    }*/

    close(from);
    close(to);
}
