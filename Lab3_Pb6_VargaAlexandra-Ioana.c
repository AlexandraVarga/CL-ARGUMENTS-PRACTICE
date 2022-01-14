/*

Autor: Alexandra-Ioana Varga
Problema 6, Laboratorul 3
Status: Incercare Implementare - IDEE PRINCIPALA
Data crearii: 23.03.2021
Data ultimei modificari: 25.03.2021

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 512

int main (int argc, char** argv)
{
    int from, nr, nw,to;///creare fisiere sursa/destinatie
    ///verific argumente linia de comanda
    int intBuFF=0;///citesc integer cu integer din fisier
    int nrExp =0;///nr medii
    int sumaMedii=0;///ca sa pot face media aritmetica
    if (argc != 3)
    {
        printf("Utilizare: %s fis_sursa fis_dest\n",
               argv[0]);
        exit(0);
    }
    ///verific fisier deschis pentru citire
    if ((from = open(argv[1], O_RDONLY)) < 0)///e in binar
    {
        perror("Eroare deschidere fisier sursa");
        exit(1);
    }
    ///creez fisier nou pentru a translata rezultatul
    if ((to = creat("medii.txt", 0666)) < 0)
    {
        perror("Eroare deschidere fisier dest.");
        exit(2);
    }
    ///citesc caracter cu caracter expresii de tip expresie
    lseek(from,1,SEEK_SET);///incerc sa sar peste primul 0
    while(nr=read(from, &intBuFF, sizeof(int)) !=0 )
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }

        ///daca ma aflu intre 2 zerou-uri - incerc sa fac media - le adun si vad cate sunt
        if(intBuFF !=0)
        {
            sumaMedii+=intBuFF;
            nrExp++;///contor medii
        }
        else ///verific daca am ajuns la al doilea 0
        {
            if (nrExp!=0)
            {
                ///retin rezultatul operatiilor
                char auxString[BUFSIZE];
                sprintf(auxString,"%lf\n",((float)sumaMedii)/nrExp);
                ///acum pun in fisier rezultatele
                if ((nw=write(to, auxString, strlen(auxString))) < 0)
                {
                    perror("Eroare scriere in fisier dest.");
                    exit(4);
                }
                ///resetez pt o noua medie aritmetica
                sumaMedii=0;
                nrExp=0;

            }

        }

    }


    close(from);
}
