/*

Autor: Alexandra-Ioana Varga
Problema 7, Laboratorul 3
Status: Incercare Implementare - IDEE PRINCIPALA
Data crearii: 23.03.2021
Data ultimei modificari: 25.03.2021

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 512

///structura pentru citire expresii fisier binar forma data
struct expresie
{
    char stringExp;
    int oper1, oper2;
};

int main (int argc, char** argv)
{
    int from, nr, nw,to;///creare fisiere sursa/destinatie
    ///definire structura de tip expresie
    struct expresie expr;
    ///verific argumente linia de comanda
    int nrExp =0;
    if (argc != 2)
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
    if ((to = creat("rezultat.txt", 0666)) < 0)
    {
        perror("Eroare deschidere fisier dest.");
        exit(2);
    }
    ///citesc caracter cu caracter expresii de tip expresie
    while(nr=read(from, &expr, sizeof(struct expresie)) !=0 )
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        ///retin rezultatul operatiilor
        char auxString[BUFSIZE];
        ///operatiile cu cei 2 operanzi
        if (expr.stringExp =='+')
        {
            sprintf(auxString,"%d: %d\n",nrExp, expr.oper1 + expr.oper2);
        }
        else if (expr.stringExp =='-')
        {
            sprintf(auxString,"%d: %d\n",nrExp, expr.oper1 - expr.oper2);
        }
        else if (expr.stringExp =='*')
        {
            sprintf(auxString,"%d: %d\n",nrExp, expr.oper1 * expr.oper2);
        }
        else if (expr.stringExp =='/')
        {
            sprintf(auxString,"%d: %d\n",nrExp, expr.oper1 / expr.oper2);
        }
        ///acum pun in fisier rezultatele
        if ((nw=write(to, auxString, strlen(auxString))) < 0)
        {
            perror("Eroare scriere in fisier dest.");
            exit(4);
        }
        nrExp++;
    }


    close(from);
}
