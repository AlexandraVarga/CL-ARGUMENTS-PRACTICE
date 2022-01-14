/*

Problema 2, Laborator 4
Autor: Alexandra-Ioana Varga
Data creare: 23.03.2021
Ultima modificare: 04.03.2021

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

#define BUFMAXIM 561

char dirPath[BUFMAXIM];     ///path-ul spre directorul initial
char currentPath[BUFMAXIM]; ///se retine schimbarea path-urilor directorului
int size = 0;

void getMemoryDIR(char *dirName, int *size, int *sizeBlock)
{
    ///aceasta functie functioneaza pentru un fisier care se afla in directorul dat ca argument
    DIR *dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[1000];
    dir = opendir(dirName);
    if (dir == 0)
    {
        perror("Eroare deschidere director");
        exit(1);
    }
    /// caut printre intrarile din director
    while ((dirEntry = readdir(dir)) != 0)
    {
        /// evit bucla infinita din . sau ..
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0) ///evit bucle infinite
        {
            sprintf(name, "%s/%s", dirName, dirEntry->d_name);//retin numele 
            lstat(name, &inode);
            /// test the type of file
            if (S_ISREG(inode.st_mode)) ///daca este fisier ii contorizez marimea
            {
                (*size) = (*size) + inode.st_size;///maime fisiere prezente in directorul dat
                (*sizeBlock) = (*sizeBlock) + inode.st_blocks;///marime in blocuri
            }
        }
    }
}

void getMemorySUBDir(char *numeDirector, int *size, int *sizeBlock) ///calculez marimea fisierelor din director
{
    DIR *dir;                       ///pointer la director
    struct dirent *direntry = NULL; ///pointer la intrarea in directpr
    struct stat inode;              ///inode-ul
    /// directorul in care ma aflu == ca fiind directorul curent
    if (chdir(numeDirector) < 0)
    {
        printf("Nu ma pot muta din director");
        exit(5);
    }
    /// determinam calea absoluta catre directorul curent
    if (getcwd(dirPath, BUFMAXIM) == 0)
    {
        printf("Nu am putut determina calea spre directorul curent");
        exit(4);
    }
    /// deschid directorul
    dir = opendir(dirPath);
    if (dir == 0)
    {
        printf("Nu pot deschide directorul");
        exit(3);
    }
    /// caut printre intrarile din director
    while ((direntry = readdir(dir)) != NULL)
    {
        if (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, ".."))
        {
            /// evit bucla infinita din . sau ..
            if (lstat(direntry->d_name, &inode) < 0)
            {
                printf("stat failed");
                exit(4);
            }
            /// daca intrarea este director => caut in subdirectoare in subdirectoare (recursiv)
            if ((S_ISDIR(inode.st_mode) != 0) && (S_ISLNK(inode.st_mode) == 0))
            {
                ///apelare recursiva in toata structura directorului
                getMemorySUBDir(direntry->d_name, size, sizeBlock);
                /// la intoarcerea din apelul recursiv, revin in directorul parinte si apoi incerc pe alta ramuri
                if (chdir("../") < 0)
                {
                    printf("Nu ma pot muta din director");
                    exit(5);
                }
            }
            else ///daca nu am gasit un director presupun ca am gasit fisiere cautate pe carora le contorizez marimea
            {
                if (getcwd(currentPath, BUFMAXIM) == 0)
                {
                    printf("Nu am putut determina path-ul directorului curent");
                    exit(4);
                }
                (*size) = (*size) + inode.st_size; ///maime totala fisiere gasite in subdirectoarele directorului curent
                (*sizeBlock) = (*sizeBlock) + inode.st_blocks;///marime in blocuri
            }
        }
    }
}

int main(int argc, char **argv)
{
    int sizeDir = 0;             ///memorie ocupata de fisierele unui director
    int sizeBlock = 0; ///memorie ocupata ca blocuri
    char numeDirector[BUFMAXIM]; ///extrag nume director curent
    ///verificare argumente linia de comanda corespunzatoare
    if (argc != 2)
    {
        printf("UTILIZARE: ./nume_executabil nume_dir \n", argv[0]);
        exit(0);
    }
    else
    {
        ///daca introducerea este corecta afisam argumentele introduse
        printf(" Executabil: %s\n", argv[0]);
        printf(" Director dat: %s\n", argv[1]);
        printf("\n");
    }

    ///extragere argumente linia de comanda
    strcpy(numeDirector, argv[1]);
    ///verficare existenta director altfel terminare program
    struct stat fileMetadata;
    if ((stat(numeDirector, &fileMetadata)) < 0)
    {
        printf("The directory does not exist!");
    }
    if (!(S_ISDIR(fileMetadata.st_mode)))
    {
        /// daca nu este un director existent, se afiseaza un mesaj de eroare si se termina programul
        printf("%s is not a directory!\n", numeDirector);
        exit(3);
    }
    ///afisam marimea tuturor fisierelor din directorul dat
    getMemoryDIR(numeDirector, &sizeDir, &sizeBlock);
    printf(" Marime totala fisiere director: %d\n Blocuri ocupate: %d\n\n", sizeDir, sizeBlock);

    sizeDir = sizeBlock =0; ///resetam size-ul
    getMemorySUBDir(numeDirector, &sizeDir,&sizeBlock);
    printf(" Marime totala fisiere subdirectoare: %d\n Blocuri ocupate: %d\n", sizeDir,sizeBlock);
}
