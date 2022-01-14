/*

Autor: Alexandra-Ioana Varga
Problema 3.2, Laborator 10 - Problema Filozofilor
Stadiu: Partial Rezolvata
Ultima modificare: 15.05.2021

*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX 5//max 5 filozofi

sem_t camera;
sem_t furculite[MAX];

void *philosopher(void *num)
{
	int phil = *(int *)num;

	sem_wait(&camera);//sleep camera
	printf("\nPhilosopher %d is thinking", phil);//nu fac nimic filozofii
	sem_wait(&furculite[phil]);//se asteapta sa ia unul furculita
	sem_wait(&furculite[(phil + 1) % MAX]);//se asteapta sa ia unul de langa el furculita

	printf("\nPhilosopher %d is eating", phil);//mananca

	sleep(1);//pt afisare
	printf("\nPhilosopher %d has finished eating", phil);//termina de mancat

	sem_post(&furculite[(phil + 1) % MAX]);//sleep furculita de langa
	sem_post(&furculite[phil]);//sleep furculita filozofului ales
	sem_post(&camera);//sleep camera
}

int main()
{
	int i, id[MAX];
	pthread_t tid[MAX];

	sem_init(&camera, 0, 4);
	//initializare semafoare furculite
	for (i = 0; i < MAX; i++)
	{
		sem_init(&furculite[i], 0, 1);
	}
	//thread filozofi cu id-uri
	for (i = 0; i < MAX; i++)
	{
		id[i] = i;
		pthread_create(&tid[i], NULL, philosopher, (void *)&id[i]);
	}
	for (i = 0; i < MAX; i++)
	{
		pthread_join(tid[i], NULL);
	}
	sem_destroy(&camera);
	for (int i=0 ; i< MAX; i++)
    {
		sem_destroy(&furculite[i]);
	}
}