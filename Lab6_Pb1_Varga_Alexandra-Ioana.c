/*

Autor: Alexandra-Ioana Varga, 30223
Problema 1, Laborator 6 - ierarhie de procese
Data ultimei modificari: 03.03.2021
Stadiu: Problema rezolvata.

*/
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main()
{
    /*
        fork() == 0 (am creeat copilul)
        fork() > 0 (ma intorc la parinte - id-ul celui mai nou copil creeat in proces)
        fork() < 0 (nu am creeat copilul)

    */

    printf("Parintele radacina: %d\n", getpid());///parintele tuturor - radacina p1

    pid_t p2,p3,p4,p5,p6;///procesele ce vreau sa le pun in ierarhie

    p2=fork();///copilul p2 al parintelui p1

    if (p2 == 0)///am creat cu succes copilul p2 al parintelui p1
    {
        printf("Copilul %d al parintelui %d\n", getpid(), getppid());///afisez copilul p2 al radacinii - p1
        p4=fork();///copilul p4 al parintelui p2
        if (p4 == 0)///am creat cu succes copilul p4 al parintelui p2
        {
            printf("Copilul %d al parintelui %d\n", getpid(), getppid());
        }

    }
    if (p2 > 0)///incerc sa ma intorc in ierarhie la parintele p1
    {
        p3=fork(); ///copilul p3 al parintelui p1
        if (p3 == 0)///am creat cu succes copilul p3 al parintelui p1
        {
            printf("Copilul %d al parintelui %d\n", getpid(), getppid());
            p5=fork();///copilul p5 al parintelui p3
            if (p5==0)
            {
                printf("Copilul %d al parintelui %d\n", getpid(), getppid());
            }
            else ///ma intorc la parintele p3 si creez copilul p6
            {
                p6=fork();///copilul p6 al parintelui p3
                if (p6 == 0)///am creeat copilul p6 al parintelui p3
                {
                    printf("Copilul %d al parintelui %d\n", getpid(), getppid());
                }
            }

        }

    }

    ///astept pana se schimba starea fiecarui copil creat
    for (int i=0; i<4; i++)
    {
        wait(NULL);
    }
}
