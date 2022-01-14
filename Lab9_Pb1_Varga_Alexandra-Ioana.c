/*

Autor: Alexandra-Ioana Varga
Problema 1, Laborator 9
Stadiu: Rezolvata
Ultima modificare: 13.05.2021

*/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

int MAX;//interval maxim de incrementari
int count;//counter
sem_t sem;
///functie thread 1
void *incrementare_th1(void *args)
{
	int id = (int)args;
	for (;;)
	{
		sem_wait(&sem);
		if (count >= MAX)// se termina
		{
			//printf("Thread %d releasing the semaphore and exiting! \n", id);
			sem_post(&sem);
			pthread_exit(NULL);
		}
		count++;//incrementam counterul
		//printf("%d %d\n", id, count);

		sem_post(&sem);
	}
}
///functie thread 2
void *incrementare_th2(void *args)
{
	int id = (int)args;
	for (;;)
	{
		sem_wait(&sem);
		if (count >= MAX)//iese
		{
			//printf("Thread %d releasing the semaphore and exiting! \n", id);
			sem_post(&sem);
			pthread_exit(NULL);
		}
		count++;//incrementam counter-ul
		//printf("%d %d\n", id, count);

		sem_post(&sem);
	}
}
int main(int argc, char **argv)
{
	///se da argument de la tastatura nr maxim de iteratii
	if (argc != 2)
	{
		printf("Argumente incorecte %s", argv[0]);
	}
	MAX = atoi(argv[1]);//se retine maximul
	///2 thread-uri
	pthread_t th1, th2;
	sem_init(&sem, 0, 1);
	//creare thread 1
	if (pthread_create(&th1, NULL, &incrementare_th1, (void *)1) != 0)
	{
		perror("Error creating thread th0!\n");
		exit(1);
	}
	//creare thread 2
	if (pthread_create(&th2, NULL, &incrementare_th2, (void *)2) != 0)
	{
		perror("Error creating thread th1!\n");
		exit(2);
	}

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);

	printf("count = %d\n", count);

	sem_destroy(&sem);

	return 0;
}