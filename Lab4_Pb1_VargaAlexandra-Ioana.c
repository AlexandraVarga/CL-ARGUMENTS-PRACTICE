/*

Problema 1, Laborator 4
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
///Am pornit de la resursa de laborator care are ca program
///afisarea continutului unui director cu specificarea tipului fiecarui element ce se gaseste in director
///extragere argumente linia de comanda

char dirPath[BUFMAXIM];///path-ul spre directorul initial
char currentPath[BUFMAXIM];///se retine schimbarea path-urilor directorului 
int size = 0;///calculez si marimea fisierului

///functie care cauta un fisier intr-un director dat
void searchDIR(char *dirName, char *file)
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
               if (strcmp(dirEntry->d_name, file) == 0)///daca fisierul gasit are acelasi nume cu fisierul argument din linia de comanda
				{
					///printez datele despre acest fisier deoarece l-am gasit cu succes
					if (getcwd(currentPath, BUFMAXIM) == 0)
					{
						printf("Nu am putut determina path-ul directorului curent");
						exit(4);
					}
					printf("inode: %ld, size: %ld, path: %s/%s/%s\n\n", inode.st_ino, inode.st_size, currentPath, dirName, dirEntry->d_name);
					size += inode.st_size;
				}
            }
        }
    }
}

///functie care cauta in toata structura directorului
void searchSUBDIR(char *numeDirector, char *numeFisier)
{
	DIR *dir;						///pointer la director
	struct dirent *direntry = NULL; ///pointer la intrarea in directpr
	struct stat inode;				///inode-ul
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
				searchSUBDIR(direntry->d_name, numeFisier);
				/// la intoarcerea din apelul recursiv, revin in directorul parinte si apoi incerc pe alta ramuri
				if (chdir("../") < 0)
				{
					printf("Nu ma pot muta din director");
					exit(5);
				}
			}
			else ///daca nu am gasit un director presupun ca am gasit fisierul cautat
			{
				if (strcmp(direntry->d_name, numeFisier) == 0)///daca fisierul gasit are acelasi nume cu fisierul argument din linia de comanda
				{
					if (getcwd(currentPath, BUFMAXIM) == 0)
					{
						printf("Nu am putut determina path-ul directorului curent");
						exit(4);
					}
					///printez datele despre acest fisier deoarece l-am gasit cu succes
					printf("inode: %ld, size: %ld, path: %s/%s\n\n", inode.st_ino, inode.st_size, currentPath, direntry->d_name);
					size += inode.st_size;
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	char executabil[BUFMAXIM];	 ///extrag nume executabil
	char numeDirector[BUFMAXIM]; ///extrag nume director curent
	char numeFisier[BUFMAXIM];	 ///extrag nume fisier cautat
	struct stat fileMetadata;
	///verificare argumente linia de comanda corespunzatoare
	if (argc != 3)
	{
		printf("UTILIZARE: ./nume_executabil nume_dir nume_fisier\n", argv[0]);
		exit(0);
	}
	else
	{
		///daca introducerea este corecta afisam argumentele introduse
		printf("Executabil: %s\n", argv[0]);
		printf("Director dat: %s\n", argv[1]);
		printf("Fisier cautat: %s\n", argv[2]);
		printf("\n");
	}
	///preiau argumentele din linia de comanda
	strcpy(executabil, argv[0]);
	strcpy(numeDirector, argv[1]);
	strcpy(numeFisier, argv[2]);
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
	///apelare functie care cauta fisierul intr-un director dat
	searchDIR(numeDirector, numeFisier);
	///apelare functie care cauta un fisier intr-un director dat cat si subdirectoarele acestuia
	searchSUBDIR(numeDirector, numeFisier);
}
