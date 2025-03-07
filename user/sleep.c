//
// Created by univero on 25-3-7 in ECNU.
//
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int n;

    if (argc < 2)
    {
        fprintf(2, "Usage: sleep second\n");
        exit(1);
    }
    n = atoi(argv[1]);
    sleep(n);
    exit(0);
}
