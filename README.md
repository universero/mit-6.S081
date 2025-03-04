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

## Clone the repository

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

- `make qemu-gdb` to start the gdb server,  and then click the button of debug in CLion.  When the debugger connect successfully, you can input `ls grep` in the shell, and you can find the programme stop at the breakpoint

Finally, the environment is ready and you can start to do the lab.