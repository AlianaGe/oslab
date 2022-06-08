#define __LIBRARY__
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#define BUFFSIZE 5
#define N 10
#define M 511
#define K 10

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t *, sem);
_syscall1(int, sem_post, sem_t *, sem);
_syscall1(int, sem_unlink, const char *, name);

_syscall2(int, shmget, key_t, key, size_t, size);
_syscall1(void *, shmat, int, shmid);

int in;
sem_t *space, *item, *mutex;
int page_id;
int *page_start;

int main()
{
    int j;
    if ((space = sem_open("/space", K)) == SEM_FAILED)
    {
        printf("space error!");
    }
    if ((item = sem_open("/item", 0)) == SEM_FAILED)
    {
        printf("item error!");
    }
    if ((mutex = sem_open("/mutex", 1)) == SEM_FAILED)
    {
        printf("mutex error!");
    }
    page_id = shmget(666, 1);
    page_start = (int*)shmat(page_id);
    printf("producer:page_id=%d,0x%08x\n", page_id,page_start);
    fflush(stdout);

    for (j = 0; j < M; j++)
    {
        sem_wait(space);
        sem_wait(mutex);
        page_start[in] = j;
        in = (in + 1) % K;
        sem_post(mutex);
        sem_post(item);
        printf("%d:%d\n", getpid(), page_start[in-1]);
        fflush(stdout);
    }

    printf("producer end\n");
    fflush(stdout);
    sem_unlink("/space");
    sem_unlink("/item");
    sem_unlink("/mutex");

    return 0;
}
