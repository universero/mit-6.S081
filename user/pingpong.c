//
// Created by univero on 25-3-7 in ECNU.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int p[2];
    char buf[1];

    // Create a pipe
    if (pipe(p) < 0)
    {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    // Fork a child process
    if (fork() == 0)
    {
        // Child process
        // Read from the pipe
        if (read(p[0], buf, sizeof(buf)) != sizeof(buf))
        {
            fprintf(2, "read failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        // Write to the pipe
        if (write(p[1], buf, sizeof(buf)) != sizeof(buf))
        {
            fprintf(2, "write failed\n");
            exit(1);
        }
    }
    else
    {
        // Parent process
        // Write to the pipe
        buf[0] = 'a'; // Send a single byte
        if (write(p[1], buf, sizeof(buf)) != sizeof(buf))
        {
            fprintf(2, "write failed\n");
            exit(1);
        }

        // Wait for the child to finish
        wait(0);

        // Read from the pipe
        if (read(p[0], buf, sizeof(buf)) != sizeof(buf))
        {
            fprintf(2, "read failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}
