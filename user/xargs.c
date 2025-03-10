//
// Created by univero on 25-3-10 in ECNU.
//

#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void xargs(int argc, char* xargv[], char* buf);

int
main(int argc, char* argv[])
{
    char buf[512], *xargv[MAXARG];
    for (int i = 1; i < argc; i++)
        xargv[i - 1] = argv[i];

    while (gets(buf, 512) && buf[0] != '\0')
    {
        xargs(argc-1, xargv, buf);
    }
    while (wait(0) != -1);
    exit(0);
}

void
xargs(int argc, char* xargv[], char* buf)
{
    xargv[argc++] = buf;

    for (char* p = buf; *p; p++)
    {
        if (*p == ' ')
        {
            *p = '\0';
            xargv[argc++] = p + 1;
        }
        else if (*p == '\n')
            *p = '\0';
    }

    if (fork() == 0)
    {
        exec(xargv[0], xargv);
        exit(1);
    }
}
