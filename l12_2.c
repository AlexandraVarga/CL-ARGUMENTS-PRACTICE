/*

Autor: Alexandra-Ioana Varga
Problema 2, Laborator 12 - alternanta incrementare contor cu memorie partajata
Stadiu: NERezolvata
Ultima modificare: 22.05.2021

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAX 10
#define N 4
typedef struct to_share
{
    int counter;
    sem_t sem;
} to_share_type;

to_share_type *ptr;
int shm_fd, pid1, pid2;

static long round_up(long n, long mult)
{
    return ((n + mult - 1) / mult) * mult;
}

void increment2(int pid2)
{
    //pid2=fork();
    int id = pid2 ? 2 : 1;
    while (1)
    {
        //sem_wait(&ptr->sem);
        if (ptr->counter >= MAX)
            return;
        ptr->counter++;
        printf("%d Process %d: Counter: %d \n", getpid(), id, ptr->counter);

        sem_wait(&ptr->sem);
        if ((ptr->counter + 1) % 2 != 0)
        {

            //printf("Process %d releasing the semaphore and exiting! \n", id);
            sem_post(&ptr->sem);
            increment1(pid2);
        }
        sem_post(&ptr->sem);
    }
}

void increment1(int pid1)
{
    //pid1=fork();
    int id = pid1 ? 1 : 2;
    while (1)
    {
        //sem_wait(&ptr->sem);
        if (ptr->counter >= MAX)
            return;

        ptr->counter++;
        printf("%d Process %d: Counter: %d \n", getpid(), id, ptr->counter);

        sem_wait(&ptr->sem);
        if ((ptr->counter + 1) % 2 == 0)
        {

            //printf("Process %d releasing the semaphore and exiting! \n", id);
            sem_post(&ptr->sem);
            increment2(pid1);
        }

        sem_post(&ptr->sem);
    }
}

int main(void)
{
    long pagesize;
    long sharesize;

    pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize == -1)
    {
        perror("sysconf(_SC_PAGESIZE)");
        return 1;
    }

    shm_fd = shm_open("/Memo", O_CREAT | O_RDWR, 0660);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 2;
    }

    sharesize = round_up(sizeof(int) + sizeof(sem_t), pagesize);
    if (ftruncate(shm_fd, sharesize) == -1)
    {
        perror("ftruncate");
        return 3;
    }

    ptr = mmap(0, sharesize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return 4;
    }
    if (sem_init(&ptr->sem, 1, 1))
    {
        perror("sem_init");
        return 5;
    }

    for (int i = 0; i < N; i++)
    {
        pid1=fork();
        increment2(pid1);
        wait(NULL);
    }

    printf("\n");
    printf("PID: %d Final counter value: %d \n", getpid(), ptr->counter);

    sem_destroy(&ptr->sem);
    munmap(ptr, sharesize);
    shm_unlink("/Memo");
    return 0;
}