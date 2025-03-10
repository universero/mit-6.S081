//
// Created by univero on 25-3-9 in ECNU.
//


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* path, char* target);
char* fname(char* path);

int
main(int argc, char* argv[])
{
    if (argc < 3)
    {
        fprintf(2, "[usage]: find dirname filename");
        exit(1);
    }

    char* dir = argv[1];
    char* target = argv[2];

    find(dir, target);
    exit(0);
}

void
find(char* path, char* target)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    // if open a file, check whether it's the target
    case T_FILE:
        {
            char* filename = fname(path);
            if (strcmp(filename, target) == 0)
            {
                printf("%s\n", path);
            }
        }
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, target);
        }
        break;
    }
    close(fd);
}

char*
fname(char* path)
{
    char* p;
    int n = 1;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        n++;
    p++;

    char* buf = malloc(n);
    memmove(buf, p, n);
    return buf;
}
