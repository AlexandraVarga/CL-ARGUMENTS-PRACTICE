/*
    Autor: Alexandra-Ioana Varga
    Problema 3, Laborator 6 - numarare fork()-uri
    Data modificare: 03.04.2021
    Stadiu: Rezolvata

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
int main()
{
    int fd[2];///file descriptor
    ///f[0] - pentru citire , f[1] pentru scriere - cu pipe

    int cntWrite = 0;///cate procese se declanseaza cu fork pe care le-am scris in f[1]
    int cntRead = 0;///cate procese am gasit citindu-le din f[0]

    if(pipe(fd)==-1)///in caz ca nu se poate executa pipe-ul
    {
        fprintf(stderr, "Error Pipe" );
        return 1;
    }

    for(int i=0; i<=10; i++)
    {
        if(fork() == 0)///daca s-a putut crea copilul
        {
            write(fd[1], "*", strlen("*"));///scriem stelute in pipe f[1]
            cntWrite++;///contorizam ce am scris in fd[1]
        }
    }
    close(fd[1]);  ///am terminat de scris

    if(cntWrite == 0)///procesul initial va citi de aici valoare scrisa in descriptor
    {
        while(read(fd[0], &cntWrite, strlen("*")) != 0)///citim stelutele din pipe
        {
            cntRead++;///numarul real de procese efectuate
        }
        printf( "%d procese declansate", cntRead);///rezultat final
    }

    close(fd[2]);///am terminat de citit

    return 0;
}
