/*

Autor: Alexandra-Ioana Varga
Problema 1.1, Laborator 8 - server
Data ultimei modificari: 12.05.2021
Stadiu: Rezolvata

*/
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLINE 20

int nr1;///primul nr dat ca argument la executabil
int nr2;///al doilea nr dat ca argument la executabil

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("UTILIZARE: ./nume_executabil nume_dir nume_fisier\n", argv[0]);
        exit(0);
    } 
    ///preiau argumentele din linia de comanda
    nr1=atoi(argv[1]);//conversie la intreg
    nr2=atoi(argv[2]);//conversie la intreg

    if (strcmp(argv[3],"+")==0)///in functie de argumentul operand verific ce operate fac
    {
        printf("Rezultat adunarii este: %d\n", nr1 + nr2);//suma numere
    }
    else
    {
        printf("Rezultatul scaderii este: %d\n", nr1 - nr2);//diferente numere
    }
   
}
