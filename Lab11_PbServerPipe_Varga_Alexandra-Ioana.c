/*

Autor: Alexandra-Ioana Varga
Problema 1, Laborator 11 - cu pipe
Data ultimei modificari: 12.05.2021
Stadiu: Rezolvata

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#define MAXLINE 20

int nr1; ///primul nr dat ca argument la executabil
int nr2; ///al doilea nr dat ca argument la executabil

int main(int argc, char **argv)
{
    int result = 0;
    if (argc != 4)
    {
        printf("UTILIZARE: ./nume_executabil nume_dir nume_fisier\n", argv[0]);
        exit(0);
    }
    ///preiau argumentele din linia de comanda
    nr1 = atoi(argv[1]); //conversie la intreg
    nr2 = atoi(argv[2]); //conversie la intreg
    ///pipe-uri
    int fd[2];          // fd[0] - client , fd[1] - server
    if (pipe(fd) == -1) ///in caz ca nu se poate executa pipe-ul
    {
        fprintf(stderr, "Error Pipe");
        return 1;
    }
    
    if (strcmp(argv[3], "+") == 0) ///in functie de argumentul operand verific ce operate fac
    {
        result = nr1 + nr2;
        //printf("Rezultat adunarii este: %d\n", nr1 + nr2); //suma numere
        write(fd[1], &result, sizeof(int));//punem in pipe rezulatul
    }
    else
    {
        result = nr1 - nr2;
        //printf("Rezultatul scaderii este: %d\n", nr1 - nr2); //diferente numere
        write(fd[1], &result, sizeof(int));//punem in server rezulatul
    }
    close(fd[1]);

    int res = 0;
    read(fd[0], &res, sizeof(int));//citesc din server
    printf("Rezultatul operatiei = %d\n", res);

    close(fd[1]);
}
