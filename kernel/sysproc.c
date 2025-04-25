#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;

  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 va, buf;
  int n;
  uint tmp = 0;
  struct proc* p = myproc();
  // 解析参数: 开始的虚拟地址, 需要检查的页数, 用户缓冲区地址(unsigned int*)
  if (argaddr(0, &va) < 0)
    return -1;
  if (argint(1, &n) < 0 || n < 0 || va+n*PGSIZE >= MAXVA) // 限制了最大值
    return -1;
  if (argaddr(2, &buf) < 0)
    return -1;
  // 统计访问
  for (uint i=0;i<n;i++){
    // 找到PTE
    pte_t* pte = walk(p->pagetable,va+i*PGSIZE,0);
    if(pte) {
      // 校验PTE_A
      if ((*pte>>6 & 1) ==1){
        tmp |= 1<<i;
      }
      *pte = *pte&~(PTE_A);
    }
  }
  // 复制到用户缓冲区中
  copyout(p->pagetable,buf,(char *)&tmp,sizeof(uint));
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
