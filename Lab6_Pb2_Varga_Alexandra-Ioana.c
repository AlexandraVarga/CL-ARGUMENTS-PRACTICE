/*

Autor: Alexandra-Ioana Varga
Problema 2, Laborator 6
Data ultimei modificari: 03.04.2021
Stadiu: Rezolvata - MERGE 

*/
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLINE 20

///functie cu care parsez argumentele comenzilor linux separate prin spatiu
void parsareArgumente (char *buf, char bufArg[MAXLINE][MAXLINE], int *cnt)
{
    char *pc = strtok(buf," ");
    while(pc)
    {
        strcpy(bufArg[(*cnt)++],pc);///salvez in acest buffer argumentele parsate
        pc=strtok(NULL," ");
    }
}
///functie afisare argumente comanda linux
void executareArgumente (char bufArg[MAXLINE][MAXLINE], int cnt)
{

    char *args[cnt];///pun argumentele comenzilor
    for (int i=0; i<cnt; i++)
    {
        args[i]=bufArg[i];///le preiau din buffer
    }
    args[cnt]=NULL;///ultimul argument e NULL pentru execvp()
    execvp(args[0], args);///se executa comanda cu argumentele date
    fprintf(stderr, "failed to execute \"%s\"\n", bufArg[0]);///pentru comanda invalida
    
}
int main(void)
{
    char buf[MAXLINE];///buffer in care citesc comanda de la tastatura
    char bufArg[MAXLINE][MAXLINE];///buffer cu argumente comanda linux
    int cnt=0;///contor numar de argumente
    pid_t p,pAsincron;///proces ce vreau sa il declansez
    int status;///status-ul pocesului
    int ok=0;
    ///resetare buffer la NULL
    for (int i=0; i<MAXLINE; i++)
    {
        for (int j=0; j<strlen(bufArg[i]); j++)
        {
            bufArg[i][j]=NULL;
        }
    }
    printf("> ");
    while (fgets(buf, MAXLINE, stdin) != NULL)///citesc de la tastatura in buffer
    {
        buf[strlen(buf)-1] = 0;///ignoram endline-ul
        parsareArgumente(buf,bufArg,&cnt);///parsez argumentele delimitate prin spatiu

        if ((p=fork()) < 0)///nu am putut crea procesul copil
        {
            perror("Eroare fork");
            exit(1);
        }
        if (p == 0)///am creat procesul copil cu succes
        {
            if (strcmp(bufArg[cnt-1],"&")==0)///vreau sa creez un subshell
            {
                pid_t t;//copil nou
                t=fork();
                if (t==0)///daca am copil
                {
                    printf("PID: %d\n",getpid());///afisez pid-ul copilului
                    executareArgumente(bufArg,cnt-1);///execut comanda linux parsata fara &
                }

            }
            else
            {
                executareArgumente(bufArg,cnt);///execut comanda linux parsata
                perror("Eroare exec");
                exit(2);
            }

        }
        else 
        {
            //pentru parinte alta comanda 
            printf("> ");
            wait(NULL);
        }
        ///resetare buffere utilizate
        memset(buf,0,100);
        cnt=0;
        for (int i=0; i<MAXLINE; i++)
        {
            for (int j=0; j<strlen(bufArg[i]); j++)
            {
                bufArg[i][j]=NULL;
            }
        }
        ///resetare buffer
        memset(buf,0,100);
        cnt=0;
        for (int i=0; i<MAXLINE; i++)
        {
            for (int j=0; j<strlen(bufArg[i]); j++)
            {
                bufArg[i][j]=NULL;
            }
        }

    }

    exit(0);
}

