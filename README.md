# MIT-6.S081

> [课程地址  6.S081 / Fall 2021](https://pdos.csail.mit.edu/6.828/2021/index.html)
> [参考博客 - foliet](https://zhuanlan.zhihu.com/p/547417990)
> 2025-03
___

## Foreword

在foliet学长的建议下，准备开始OS、DB等更深入的学习，并将MIT-6.S081作为第一个着手的Lab
同时，为了捡起许久没学的英语以应对六级，会尽可能的用英文记录，所以难免有中英夹杂的阅读不变之处。

该系列的笔记使用Obsidian记录，可能存在与github的markdown格式不兼容的问题(主要是连接跳转)，建议使用Obsidian阅读。
本仓库中的文档是从[Learn-Backend-Mit-6.S081](https://github.com/universero/Learn---Backend/tree/main/%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F/MIT-6.S081)中直接复制的，如果希望有比较好的阅读效果，建议在原仓库阅读

2018年及以前MIT6.828是MIT的本科生OS课程，2019开始这门课被拆成了6.S081和6.828，分别成为本科生和研究生的OS课程。6.S081的实验由原来的5个拆成了10个，总体难度降低，更加易于基础并不牢固的学生学习。

## What's Xv6

The OS used for teaching and demonstration in this lab is the OS called Xv6 which based on the RISC-V instruction set architecture, but not at the outset.

The predecessor of this courese 6.828, initiall use Unix V6 (abbreviated as V6, based on the PDP-11 hardware system and the old C language development) as the teaching OS, and in the experimental session, the students mainly implemented an OS called Jos of the "exokernel" architecture on a x86 architecture CPU.

Since the outdated architecture have been unnecessary, the course upgrade it to RISC-V in 2019.

## Environment Config

> [6.S081 - Necessay Tools](https://pdos.csail.mit.edu/6.828/2021/tools.html)

### Virtual Merchine

For convenience, I choose the wsl(Windows Subsystem for Linux) to install an Ubuntu20.04

`wsl --install Ubuntu20.04`

Also, you can use other tools to install Virtual Merchines, like VMware Workstation or Virtual Box
### Necessary Tools

You need use the command to upgrade the repository, or you will not find the package gdb-multiarch.

`sudo apt-get update`

The command aims to download necessay tools, including c/c++ compilation toolset, GNU toolset for RISC-V64 architecture, QEMU vm, git and GDB for RISC-V64.

`sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu`

### Clone the repository

`git clone git://g.csail.mit.edu/xv6-labs-2021`

It's highly recommend to use git to manage your code in your own repo.

check it with the following command
```shell
	git checkout util
	make qemu
```
if you want to exit the qemu, press `ctrl + a`, then release it and press `x`

### Use CLion to edit code

> you can apply a free license for CLion or other IDEs of JetBrains with your edu mail

- create a new project at the lab directory in wsl
- waiting for the installation of IDE, and  it may take a long time at the first time
- when it down, you can see as the following picture
  ![[CLion-WSL.png]]
- set the toolchain, mainly set the debugger
  ![[设置调试器.png]]
- set the Makefile, change the build target to qemu-gdb
  ![[Makefile.png]]
- add an remote debugger
  the tcp argument may different，depending on the output of `make qemu-gdb`， if you use wsl， it may be 26000
  ![[远程调试.png]]
- create a file called .gdbinit at ~/.gdbinit and add following content

``` bash
set confirm off
set architecture riscv:rv64
file kernel/kernel
add-symbol-file user/_ls
set disassemble-next-line auto
set riscv use-compressed-breakpoints yes
```

	to debug other program like myproc, add a extra line `add-symbol-file user/_myproc`

- `make qemu-gdb` to start the gdb server,  and then click the button of debug in CLion.  When the debugger connect successfully, you can input `ls grep` in the shell, and you can find the program stop at the breakpoint

Finally, the environment is ready and you can start to do the lab.

## Xv6 and Unix Utilities


>This chapter is corresponding to [Lab: Xv6 and Unix utilities](https://pdos.csail.mit.edu/6.828/2021/labs/util.html)
>[xv6: a simple, Unix-like teaching operating system](https://pdos.csail.mit.edu/6.828/2021/xv6/book-riscv-rev2.pdf)

___

### Boot xv6 (easy)

After the first part Overview & Environment Configuration, we can start the xv6 successfully with the command `make qemu`
And you can try the executable programs, which is included by mkfs in the initial file system, like ls.
Pay attention to that xv6 has no ps command, but if you type ctrl-p, the kernel  will print information about each process.
To quit qeum type `ctrl-a x`

### Grading

you can run `make grade` to test your solution with the grading

### Sleep (easy)

> Implement the UNIX program sleep for xv6; your sleep should pause for a user-specified number of ticks. A tick is a notion of time defined by the xv6 kernel, namely the time between two interrupts from the timer chip. Your solution should be in the file user/sleep.c.

The sleep program has following demands and hints:
- argument is padded as a string, and we need to convert it to integer using atoi(user/ulib,c)
- using the system call sleep
- seeing the implement of sleep system call in kernel/sysproc.c/sys_sleep, and user/user.h for the C definition of sleep callable from a user program, and user/usys.S for the assenmbler code that jumps from user code into the kernel for sleep
- make sure main calls exit() in order to exit
- if you want to test the single assignment, type: `./grade-lab-util sleep`, or you can type `make GRADEFLAGS=sleep grade`
- If the user forgets to pass an argument, sleep should print an error message.
#### analysis

- atoi
```c
int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}
```
get the number presented by the continuous string like '123'

- the implement of sleep system call
```c
uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  // get the nth 32-bit system call argument
  // you can follow the function call to find the implement of it
  if(argint(0, &n) < 0)
    return -1;

  // acquire the ticks lock
  acquire(&tickslock);
  // ticks is a variable that records the times of clock interrupt
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    // if the sleep proc has been killed
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    // let the current proc sleep, but the release of it is a bit confused now, may we can find it out later
    sleep(&ticks, &tickslock);
  }
  // release the ticks lock
  release(&tickslock);
  return 0;
}
```

- the C definitionof sleep `int sleep(int)`, get the number of sleep ticks and return wheter it sleep successfully
- Then see the usys.S to find the assembler code that jumps from user code into the kernel for sleep. If you not find the file under user, run `perl usys.pl > usys.S` firstly.
```shell
# marks the sleep as a global symbol, 
# making it accessible to other files or modules
.global sleep 
# defines the entry point of the sleep functino
sleep:
 # Loads the system call number SYS_sleep intp register a7
 # li stands for "load immediate", which loads a constants value into a register
 # a7 is the register used in RISC-V to pass the system call number
 li a7, SYS_sleep
 # executes an environment call, triggering the system call.
 ecall
 # returns from the function, after the system call completes, control returns to user mode.
 ret
```
- add the sleep into the UPROGS in the Makefile as following. The reason why there is a underline is that in xv6, user programs are compiled into executable files with an underscore (`_`) prefix
  ![[UPROGS-sleep.png]]
- finally implement the sleep.c in /user, the logic is simple, checking the args and call the system call sleep
```c
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
    # Here may have some error, because no function to ensure the argv[1] is actually a int, but it's enough to pass the test.
    n = atoi(argv[1]);
    sleep(n);
    exit(0);
}

```

### pingpong(easy)

>Write a program that uses UNIX system calls to ''ping-pong'' a byte between two processes over a pair of pipes, one for each direction. The parent should send a byte to the child; the child should print "`<pid>`: received ping", where `<pid>` is its process ID, write the byte on the pipe to the parent, and exit; the parent should read the byte from the child, print "`<pid>`: received pong", and exit. Your solution should be in the file user/pingpong.c.

here are some hints
- Use pipe to create a pipe
- Use fork to create a child
- Use read to read from the pipe, and write to write the pipe
- Use getpid to find the process ID of the calling process
- Add the program to UPROGS in Makefile
- You can only use the library functions in the user/ulib.c, user/printf.c and user/umalloc.c

```c
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
    if (pipe(p) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    // Fork a child process
    if (fork() == 0) {
        // Child process
        // Read from the pipe
        if (read(p[0], buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        // Write to the pipe
        if (write(p[1], buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(2, "write failed\n");
            exit(1);
        }
    } else {
        // Parent process
        // Write to the pipe
        buf[0] = 'a'; // Send a single byte
        if (write(p[1], buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(2, "write failed\n");
            exit(1);
        }

        // Wait for the child to finish
        wait(0);

        // Read from the pipe
        if (read(p[0], buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}
```
**the most important thing is that each print needs a \n**, or you will not pass the test.
And it's necessary to deal the return value of functions like pipe, read and write which indicates the function succeed or not. At the first time I do the lab, I forget to handle it.

### primes (moderate/hard)

>Write a concurrent version of prime sieve using pipes. This idea is due to Doug McIlroy, inventor of Unix pipes. The picture halfway down [this page](http://swtch.com/~rsc/thread/) and the surrounding text explain how to do it. Your solution should be in the file user/primes.c.
>Your goal is to use pipe and fork to set up the pipeline. The first process feeds the numbers 2 through 35 into the pipeline. For each prime number, you will arrange to create one process that reads from its left neighbor over a pipe and writes to its right neighbor over another pipe. Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.