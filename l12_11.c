/*

Autor: Alexandra-Ioana Varga
Problema 1, Laborator 12 - alternanta incrementare contor cu memorie partajata
Stadiu: Rezolvata
Ultima modificare: 24.05.2021

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAX 200

typedef struct to_share
{
    int counter;
    sem_t sem;
} to_share_type;

static long round_up(long n, long mult)
{
    return ((n + mult - 1) / mult) * mult;
}

void increment(to_share_type *ptr)
{

    while (1)
    {
        sem_wait(&ptr->sem);
        if (ptr->counter >= MAX)
        {

            printf("Process %d releasing the semaphore and exiting! \n", 0);
            sem_post(&ptr->sem);
            exit(0);
            return;
        }
        if (ptr->counter % 2 == 0) //pentru pare
        {
            ptr->counter++;
            printf("Process %d: Counter: %d \n", 0, ptr->counter);
        }

        sem_post(&ptr->sem);
    }
}

int main(void)
{
    long pagesize;
    long sharesize;
    to_share_type *ptr;
    int shm_fd, pid;
    
    shm_unlink("/Memo1");

    pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize == -1)
    {
        perror("sysconf(_SC_PAGESIZE)");
        return 1;
    }

    shm_fd = shm_open("/Memo1", O_CREAT | O_RDWR, S_IRWXU);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    sharesize = round_up(sizeof(int) + sizeof(sem_t), pagesize);
    if (ftruncate(shm_fd, sharesize) == -1)
    {
        perror("ftruncate");
        return 1;
    }

    ptr = mmap(0, sharesize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }
    if (sem_init(&ptr->sem, 1, 1))
    {
        perror("sem_init");
        return 1;
    }

    increment(ptr);

    wait(NULL);

    printf("Process %d: Final counter value: %d \n", pid, ptr->counter);

    sem_destroy(&ptr->sem);
    shm_unlink("/Memo");
    return 0;
}