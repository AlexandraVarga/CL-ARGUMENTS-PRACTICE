/*

Autor: Alexandra-Ioana Varga
Problema 2, Laboratorul 3
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
    int from, nr, nw;
    char buf;///buffer - citesc caracter cu caracter
    int cntLine=0;///numar liniile
    int ok = 1;///daca termin sau nu o linie cu endline
    int nLine=atoi(argv[2]);///convertesc la intreg deoarece de la linia de comanda am argv - tip string
    int okLinie=0;///daca gasesc linia cautata
    ///verific argumente introducere corecta
    if (argc != 3)
    {
        printf("Utilizare: %s fis_sursa fis_dest\n",
               argv[0]);
        exit(0);
    }
    ///verific deschidere fisier
    if ((from = open(argv[1], O_RDONLY)) < 0)
    {
        perror("Eroare deschidere fisier sursa");
        exit(1);
    }
    ///citesc caracter cu caracter
    while(nr=read(from, &buf, sizeof(char)) !=0 )
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        if (ok == 1)
        {   ///verific daca gasesc linia cu numarul dat
            if (cntLine == nLine)
            {
                printf("%d: %c",cntLine,buf);
                okLinie=1;///o gasesc
            }
            else okLinie=0;///mai caut
            cntLine++;
        }
        else
        {
            if (okLinie ==1)///am gasit linia
            {
                if ((nw=printf("%c",buf) < 0))
                {
                    ///o afisez in fisier
                    perror("Eroare afisare din fisier sursa.");
                    exit(4);
                }

            }

        }
        if (buf =='\n')///trec la linie noua cand intalnesc caracterul '\n'
        {
            ok=1;
        }
        else ok=0;
    }
    ///caz special in care argumentul introdus nu este numeric bun
    if (nLine > cntLine-1)
        printf("Numar de linie prea mare");

    close(from);
}
