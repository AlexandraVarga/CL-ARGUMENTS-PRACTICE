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
#include <sys/stat.h>

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

            printf("Process %d releasing the semaphore and exiting! \n", 1);
            sem_post(&ptr->sem);
            exit(0);
            return;
        }
        if (ptr->counter % 2 == 1) ///pentru impare
        {
            ptr->counter++;
            printf("Process %d: Counter: %d \n", 1, ptr->counter);
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
    struct stat str;
    
    pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize == -1)
    {
        perror("sysconf(_SC_PAGESIZE)");
        return 1;
    }

    shm_fd = shm_open("/Memo1", O_RDWR, 0); /* open existing shm object */
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 2;
    }

    if (fstat(shm_fd, &str) == -1)
    {
        perror("fstat");
        return 3;
    }
    sharesize = (long)str.st_size; /* determine the size of the object */

    ptr = (to_share_type *)mmap(0, sharesize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return 4;
    }

    if (close(shm_fd) == -1)
    { /* 'shm_fd' is no longer needed */
        perror("close");
        return 6;
    }

    increment(ptr);

    printf("Process %d: Final counter value: %d \n", getpid(), ptr->counter);

    munmap(ptr, sharesize);

    return 0;
}