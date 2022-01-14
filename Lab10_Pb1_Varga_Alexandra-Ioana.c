/*

Autor: Alexandra-Ioana Varga
Problema 1, Laborator 10 - alternanta 1-100
Stadiu: Rezolvata
Ultima modificare: 15.05.2021

*/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

int MAX = 100;//final value
int count=0;//initial value
//sem_t sem;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //lacat
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //var cond - sincronizare thread-uri
///functie thread 1
void *incrementare_th1(void *args)
{
	int id = (int)args;//id-ul thread-ului
	for (;;)
	{
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
		//sem_wait(&sem);
		printf("TH ID: %d %d\n", id, count);
        count++;//incrementez
        pthread_mutex_unlock(&mutex);
        if (count >= MAX)//se termina
		{
			//printf("Thread %d releasing the semaphore and exiting! \n", id);
			//sem_post(&sem);
			pthread_exit(NULL);
		}
		//sem_post(&sem);
	}
}
///functie thread 2
void *incrementare_th2(void *args)
{
	int id = (int)args;
	for (;;)
	{
        pthread_mutex_lock(&mutex);
        if (count % 2 != 0)//la counter impar
        {
            pthread_cond_signal(&cond);//trimit semnal
        }
		else
        {
            if (count <= MAX)//caz mai afisez counter
            {
                printf("TH ID: %d %d\n", id, count);
                count++;
            }
            
        }
        pthread_mutex_unlock(&mutex);
        if (count > MAX)
		{
            //printf("TH ID: %d %d\n", id, count);
			//printf("Thread %d releasing the semaphore and exiting! \n", id);
			//sem_post(&sem);
			pthread_exit(NULL);
		}
		//sem_post(&sem);
	}
}
int main(int argc, char **argv)
{
	pthread_t th1, th2;

	//sem_init(&sem, 0, 1);
	///creare thread 1 cu id 1
	if (pthread_create(&th1, NULL, &incrementare_th1, (void *)1) != 0)
	{
		perror("Error creating thread th0!\n");
		exit(1);
	}
	///creare thread 2 cu id 2
	if (pthread_create(&th2, NULL, &incrementare_th2, (void *)2) != 0)
	{
		perror("Error creating thread th1!\n");
		exit(2);
	}

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);

	//sem_destroy(&sem);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

	return 0;
}