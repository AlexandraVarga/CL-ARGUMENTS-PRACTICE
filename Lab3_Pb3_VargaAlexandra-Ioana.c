/*

Autor: Alexandra-Ioana Varga
Problema 3, Laboratorul 3
Status: Rezolvata
Data crearii: 22.03.2021
Data ultimei modificari: 25.03.2021

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 512
///doresc sa vad liniile matricei de caractere in care am transpus fisierul
void seeContent (char content[BUFSIZE][BUFSIZE], int dim)
{
    for (int i= dim-1; i>=0; i--)
    {
        printf("%s",content[i]);
    }
}
///scriu in fisierul destinatie in ordina inversa liniile fisierului sursa
void writeContent (int nw, int to, char content[BUFSIZE][BUFSIZE], int dim)
{
    for (int i=dim-1; i>=0; i--)
    {
        if ((nw=write(to, content[i], strlen(content[i]))) < 0)
        {
            perror("Eroare scriere in fisier dest.");
            exit(4);
        }
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
    ///verific argumente linia de comanda
    if (argc != 3)
    {
        printf("Utilizare: %s fis_sursa fis_dest\n",
               argv[0]);
        exit(0);
    }
    ///verific fisier deschis pentru citire
    if ((from = open(argv[1], O_RDONLY)) < 0)
    {
        perror("Eroare deschidere fisier sursa");
        exit(1);
    }
    ///creez fisier nou pentru a translata rezultatul
    if ((to = creat(argv[2], 0666)) < 0)
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
        if (ok == 1)
        {
            auxString[cntAux++]=buf;
            cntLine++;
        }
        else
        {
            auxString[cntAux++]=buf;
        }
        if (buf =='\n')///pot depune string-ul auxiliar in matrice linie cu linie pentru a le inversa
        {
            strcpy(content[dim++],auxString);
            ok=1;
            memset(auxString,'\0',BUFSIZE);
            cntAux=0;
        }
        else ok=0;
    }

    //seeContent(content,dim);
    ///rezultatul il creez intr-un fisier nou
    writeContent(nw,to,content,dim);
    close(from);
}
