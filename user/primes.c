//
// Created by univero on 25-3-8 in ECNU.
//
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define FIRST_NUM 2
#define LAST_NUM  35

void sieve(const int* left);

int
main(int argc, char* argv[])
{
    // define and initialise the pipe
    int p[2];
    pipe(p);

    if (fork() > 0) // the parent process
    {
        // write 2 to 35 into the pipe
        for (int i = FIRST_NUM; i <= LAST_NUM; i++)
        {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        close(p[0]);
        wait(0);
        exit(0);
    }
    // the child process
    close(p[1]);
    sieve(p);
    close(p[0]);
    exit(0);
}

void
sieve(const int* left)
{
    int buf;
    int right[2];

    // 读取第一个数作为素数
    if (read(left[0], &buf, sizeof(int)) == 0)
    {
        close(left[0]);
        return;
    }

    int prime = buf;
    printf("prime %d\n", prime);

    pipe(right);

    if (fork() > 0)
    {
        close(right[0]);

        while (read(left[0], &buf, sizeof(int)) > 0)
        {
            if (buf % prime != 0)
            {
                write(right[1], &buf, sizeof(int));
            }
        }
        close(right[1]);
        close(left[0]);
        wait(0);
        exit(0);
    }
    close(right[1]);
    close(left[0]);
    sieve(right);
    close(right[0]);
    exit(0);
}
