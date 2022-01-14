/*

Autor: Alexandra-Ioana Varga
Problema 5, Laboratorul 3
Status: Rezolvata Ineficient
Data crearii: 22.03.2021
Data ultimei modificari: 25.03.2021

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 512
///doresc sa vad liniile matricei de caractere in care am transpus fisierul
void processContent (char *content, int *dim, int n)
{
    //printf("%d",n);
    //pozitia zero se duce din prima
    int eliminari=1;
    ///fac shiftare pe buffer-ul extras din fisier
    ///din n in n octeti
    for (int i=0; i<(*dim); i++)
    {
        content[i]=content[i+1];
    }
    for (int i=1; i <(*dim); i++)
    {
        //printf("%c",content[i]);
        if (i%n==0)
        {
            eliminari++;
            for (int j=i; j<(*dim); j++)
            {
                content[j]=content[j+1];
            }
        }
    }
    (*dim)=(*dim)-eliminari;
}

void seeContent (char *content, int dim)
{
    for (int i=0; i<dim; i++)
    {
        printf("%c",content[i]);
    }
}
int main (int argc, char** argv)
{
    int from, nr, nw,to;///creare fisiere sursa/destinatie
    char buf;///ma duc cu buffer caracter cu caracter
    int cntLine=0;///numar liniile
    int ok = 1;///verific daca am terminat o linie cu endline
    char auxString[BUFSIZE];///creez linia in string auxiliar
    char content[BUFSIZE][BUFSIZE];///string-ul auxiliar o sa devina fiecare linie a matricii de stringuri
    ///pe care o put parcurge invers
    int dim=0;///dimensiune matrice
    int cntAux=0;///dimensiune string aux
    int *n;
    ///verific argumente linia de comanda
    if (argc != 4)
    {
        printf("Utilizare: %s fis_sursa fis_dest\n",
               argv[0]);
        exit(0);
    }
    ///n-ul din cati in cati octeti stergem din fisier
    n=atoi(argv[1]);
    ///verific fisier deschis pentru citire
    if ((from = open(argv[2], O_RDONLY)) < 0)
    {
        perror("Eroare deschidere fisier sursa");
        exit(1);
    }
    ///creez fisier nou pentru a translata rezultatul
    if ((to = creat(argv[3], 0666)) < 0)
    {
        perror("Eroare deschidere fisier dest.");
        exit(2);
    }

    ///citesc caacter cu caracte
    while(nr=read(from, &buf, sizeof(char)) !=0 )
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        ///inainte de newline depun caracterele intr-un sir auxiliar
        auxString[cntAux++]=buf;
    }

    processContent(auxString,&cntAux,n);
    ///rezultatul il creez intr-un fisier nou
    seeContent(auxString,cntAux);

    ///scriere in fisier nou rezultat dupa eliminare - salvare rezultat
    if ((nw=write(to, auxString, strlen(auxString))) < 0)
    {
        perror("Eroare scriere in fisier dest.");
        exit(4);
    }

    close(from);
}
