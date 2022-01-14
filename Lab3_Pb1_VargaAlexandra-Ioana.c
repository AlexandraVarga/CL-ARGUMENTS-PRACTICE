/*

Autor: Alexandra-Ioana Varga
Problema 1, Laboratorul 3
Status: Rezolvata
Data crearii: 22.03.2021
Data ultimei modificari: 25.03.2021

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 512
int main (int argc, char** argv)
{
    int from, nr, nw;///fisier sursa citire
    char buf;///caracter tip buffer pentru fisier
    int cntLine=0;///numaram liniile din fisier
    int ok = 1;
    ///verific argumente intrare
    if (argc != 2)
    {
        printf("Utilizare: %s fis_sursa fis_dest\n",
               argv[0]);
        exit(0);
    }
    ///verific mod citire fisier
    if ((from = open(argv[1], O_RDONLY)) < 0)
    {
        perror("Eroare deschidere fisier sursa");
        exit(1);
    }
    ///parcurg fisierul caracter cu caracter
    while(nr=read(from, &buf, sizeof(char)) !=0 )
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        if (ok == 1)
        {   ///am trecut deja la urmatoarea linie
            printf("%d: %c",cntLine,buf);
            cntLine++;
        }
        else
        {   ///verific daca pot scrie in fisierul destinatie
            if ((nw=printf("%c",buf) < 0))
            {
                perror("Eroare scriere in fisier dest.");
                exit(4);
            }
        }
        ///am terminat de citit o linie din fisier => pot trece la urmatoarea
        if (buf =='\n')
        {
            ok=1;
        }
        else ok=0;

    }
    close(from);
}
