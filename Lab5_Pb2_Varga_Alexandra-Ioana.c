/*

Problema 2, Laborator 5
Autor: Alexandra-Ioana Varga
Data creare: 23.03.2021
Ultima modificare: 05.03.2021
Stadiu: Rezolvata

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

#define BUFMAXIM 1000
///Am pornit de la resursa de laborator care are ca program
///afisarea continutului unui director cu specificarea tipului fiecarui element ce se gaseste in director
///extragere argumente linia de comanda

char dirPath[BUFMAXIM];     ///path-ul spre directorul initial
char currentPath[BUFMAXIM]; ///se retine schimbarea path-urilor directorului
int size = 0;               ///calculez si marimea fisierului
///toate subdirectoarele
char cale[BUFMAXIM][BUFMAXIM]; ///calea unde gasesc fisierul .c
char cExtension[BUFMAXIM][BUFMAXIM]; ///retin fisierele cu extenisia .c
int cntExt = 0;                      ///numar cate fisiere c am gasit - cautand in toate subdirectoarele
int maxHeaders = -1;                 ///caut nr maxim de headere din fisier
int maximeH[BUFMAXIM];               ///retin din fiecare fisier cate headere a avut pt maxime egale sa afisez toate fisierele
char pathCFiles[BUFMAXIM][BUFMAXIM]; ///path-urile tuturor fisierelor c
char info[BUFMAXIM];                 ///retin auxiliar fiecare path la determinarea fisierului c
///doar directorul dat
int cntExD = 0;                       ///numar cate fisiere c am gasit - cautand doar in directorul dat
char cExtensionD[BUFMAXIM][BUFMAXIM]; ///retin fisierele cu extenisia .c
int maximeHD[BUFMAXIM];               ///retin din fiecare fisier cate headere a avut pt maxime egale sa afisez toate fisierele
char pathCFilesD[BUFMAXIM][BUFMAXIM]; ///path-urile tuturor fisierelor c
char infoD[BUFMAXIM];                 ///retin auxiliar fiecare path la determinarea fisierului c
int maxHeadersD = -1;                 ///caut nr maxim de headere din fisier
///functie care cauta un fisier intr-un director dat
void searchEXT(char *dirName, char *extension)
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
            sprintf(name, "%s/%s", dirName, dirEntry->d_name); //retin numele
            lstat(name, &inode);
            /// test the type of file
            if (S_ISREG(inode.st_mode)) ///daca este fisier ii contorizez marimea
            {
                if (searchExtension(dirEntry->d_name, extension) == 1) ///daca fisierul gasit are acelasi nume cu fisierul argument din linia de comanda
                {
                    ///printez datele despre acest fisier deoarece l-am gasit cu succes
                    if (getcwd(currentPath, BUFMAXIM) == 0)
                    {
                        printf("Nu am putut determina path-ul directorului curent");
                        exit(4);
                    }
                    sprintf(infoD, "inode: %ld, size: %ld, path: %s/%s/%s\n\n", inode.st_ino, inode.st_size, currentPath, dirName, dirEntry->d_name);
                    sprintf(cale,"%s/%s/%s",currentPath, dirName, dirEntry->d_name);///path 
                    strcpy(pathCFilesD[cntExD], infoD);            ///toate path-urile fisierelor c
                    strcpy(cExtensionD[cntExD], cale); ///retin fisierele cu extensia .c pentru a le deschide
                    
                    cntExD++; ///contor nr fisiere
                    size += inode.st_size;
                }
            }
        }
    }
}

///functie care cauta in toata structura directorului
void searchEXTs(char *numeDirector, char *numeExtensie)
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
                searchEXTs(direntry->d_name, numeExtensie);
                /// la intoarcerea din apelul recursiv, revin in directorul parinte si apoi incerc pe alta ramuri
                if (chdir("../") < 0)
                {
                    printf("Nu ma pot muta din director");
                    exit(5);
                }
            }
            else ///daca nu am gasit un director presupun ca am gasit fisierul cautat
            {
                if (searchExtension(direntry->d_name, numeExtensie) == 1) ///daca am gasit extenia in numele fisierului
                {
                    if (getcwd(currentPath, BUFMAXIM) == 0)
                    {
                        printf("Nu am putut determina path-ul directorului curent");
                        exit(4);
                    }
                    ///printez datele despre acest fisier deoarece l-am gasit cu succes
                    sprintf(info, "inode: %ld, size: %ld, path: %s/%s\n\n", inode.st_ino, inode.st_size, currentPath, direntry->d_name);
                    
                    sprintf(cale,"%s/%s",currentPath, direntry->d_name);///calea fisierului

                    strcpy(pathCFiles[cntExt], info);             ///toate info-urile fisierelor c
                    strcpy(cExtension[cntExt], cale); ///retin fisierele cu extensia .c pentru a le deschide
                    cntExt++;                                     ///contor nr fisiere
                    size += inode.st_size;
                }
            }
        }
    }
}

///functie de cautare extensie in numele unui fisier
int searchExtension(char *buffer, char *extBuf)
{
    int pozExt = strstr(buffer, extBuf) - buffer; ///pozitie la care gasesc extensia
    int pozPct = strstr(buffer, ".") - buffer;    ///pozitie la care gasesc punctul
    if (pozExt - 1 == pozPct)                     ///daca inainte de pozitia inceperii extensiei am punctul
        return 1;                                 ///are extensie buna
    return 0;                                     ///nu are extensie buna
}

///functie cu care vad ce am in matricea de string-uri echivalenta cu fisierele parsate linie cu linie
int numberOfHeaders(char content[BUFMAXIM][BUFMAXIM], int dim)
{
    int cntHeader = 0; ///numar liniile ce contin headerele
    for (int i = 0; i < dim; i++)
    {
        //printf("%s",content[i]);
        if (strstr(content[i], "#include ") != NULL)      ///gasesc linie cu #include
        {                                                 ///poate fi header sau librarie
            if (strstr(content[i], "#include <") == NULL) ///daca nu e librarie il contorizez
            {
                cntHeader++;
            }
        }
    }
    ///daca contine doar headere fisierul verific si ultima linie daca e header
    if (cntHeader == dim - 1)
    {
        if (strstr(content[dim - 1], "#include ") != NULL)      ///gasesc linie cu #include
        {                                                       ///poate fi header sau librarie
            if (strstr(content[dim - 1], "#include <") == NULL) ///daca nu e librarie il contorizez
            {
                cntHeader++;
            }
        }
    }
    return cntHeader;
}
///functie care gaseste cate fisiere header avem incluse
int countHeaders(char *file)///primesc path-ul fisierului gasit
{
    //printf("%s\n",file);
    int from, nr, nw, to;             ///creare fisiere sursa/destinatie
    char buf;                         ///ma duc cu buffer caracter cu caracter
    int cntLine = 0;                  ///numar liniile
    int ok = 1;                       ///verific daca am terminat o linie cu endline
    char auxString[BUFMAXIM];         ///creez linia in string auxiliar
    char content[BUFMAXIM][BUFMAXIM]; ///string-ul auxiliar o sa devina fiecare linie a matricii de stringuri
    ///pe care o put parcurge invers
    int dim = 0;    ///dimensiune matrice
    int cntAux = 0; ///dimensiune string aux
    int headers = 0;
    ///verific sa vad daca pot deschide fisierele c
    memset(auxString, NULL, BUFMAXIM);
    if ((from = open(file, O_RDONLY)) < 0)
    {
        perror("Eroare deschidere fisier sursa"); ///NU DESCHIDE TOT TIMPUL
        exit(5);
    }
    while (nr = read(from, &buf, sizeof(char)) != 0) ///citesc caracter cu caracter
    {
        if (nr < 0)
        {
            perror("Eroare citire din fisier sursa");
            exit(3);
        }
        ///inainte de newline depun caracterele intr-un sir auxiliar
        if (ok == 1)
        {
            auxString[cntAux++] = buf;
            cntLine++;
        }
        else
        {
            auxString[cntAux++] = buf;
        }
        if (buf == '\n') ///pot depune string-ul auxiliar in matrice linie cu linie
        {
            strcpy(content[dim++], auxString);
            ok = 1;
            memset(auxString, '\0', BUFMAXIM);
            cntAux = 0;
        }
        else
            ok = 0;
    }
    headers = numberOfHeaders(content, dim);
    if (headers >= maxHeaders)
    {
        maxHeaders = headers; ///determin si maximul global de fisiere header
    }
    return headers; ///returnez nr de headere pentru fiecare fisier deschis cu succes
}
int main(int argc, char **argv)
{
    char executabil[BUFMAXIM];   ///extrag nume executabil
    char numeDirector[BUFMAXIM]; ///extrag nume director curent
    struct stat fileMetadata;
    ///verificare argumente linia de comanda corespunzatoare
    if (argc != 2)
    {
        printf("UTILIZARE: ./nume_executabil nume_dir nume_fisier\n", argv[0]);
        exit(0);
    }
    else
    {
        ///daca introducerea este corecta afisam argumentele introduse
        printf(" Executabil: %s\n", argv[0]);
        printf(" Director dat: %s\n", argv[1]);
        printf("\n");
    }
    ///preiau argumentele din linia de comanda
    strcpy(executabil, argv[0]);
    strcpy(numeDirector, argv[1]);

    ///verficare existenta director altfel terminare program
    if ((stat(numeDirector, &fileMetadata)) < 0)
    {
        printf("Directorul nu exista!");
    }
    if (!(S_ISDIR(fileMetadata.st_mode)))
    {
        /// daca nu este un director existent, se afiseaza un mesaj de eroare si se termina programul
        printf("%s nu este director!\n", numeDirector);
        exit(3);
    }
    ///functie care cauta in fisierele c ale directorului dat
    printf(" In directorul dat: \n");
    searchEXT(numeDirector,"c");
    for (int i = 0; i < cntExD; i++)
    {
        maximeHD[i] = countHeaders(cExtensionD[i]);
    }
    ///caut sa vad cate fisiere au nr maxim de headere pentru a le afisa
    for (int i = 0; i < cntExD; i++)
    {
        if (maximeHD[i] == maxHeaders) ///daca avem fisiere cu acelasi nr maxim de headere
        {
            printf(" Headere: %d\n Info Fisier: %s\n", maximeHD[i], pathCFilesD[i]); ///le afisam pe toate cu informatiile lor
        }
    }

    maxHeaders =-1; ///acum caut in toate subdirectoarele
    printf(" In toate subdirectoarele directorului dat: \n");

    ///functie care cauta in toate subdirectoarele lui director
    searchEXTs(numeDirector, "c"); ///caut fisierele cu extensia .c si le salvez in buffer
    ///numar pt fiecare fisier cate headere are incluse

    for (int i = 0; i < cntExt; i++)
    {
        maximeH[i] = countHeaders(cExtension[i]);
    }
    ///caut sa vad cate fisiere au nr maxim de headere pentru a le afisa
    for (int i = 0; i < cntExt; i++)
    {
        if (maximeH[i] == maxHeaders) ///daca avem fisiere cu acelasi nr maxim de headere
        {
            printf(" Headere: %d\n Info Fisier: %s\n", maximeH[i], pathCFiles[i]); ///le afisam pe toate cu informatiile lor
        }
    }
}