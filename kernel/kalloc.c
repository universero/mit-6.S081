// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.
// 计数器
int count[(PHYSTOP-KERNBASE)/PGSIZE];
struct spinlock cntlock;

#define CNTPA(pa) count[((uint64)pa-KERNBASE)/PGSIZE]

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&cntlock, "cnt");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&cntlock);
  // 释放后还有指向这个页的引用, 则不实际释放
  if (--CNTPA(pa)<=0)
  {
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
  release(&cntlock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char *)r, 5, PGSIZE); // fill with junk
    // 初次分配, 将计数器置为1
    CNTPA(r) = 1;
  }
  return (void*)r;
}

void *kcopy(void*pa)
{
  acquire(&cntlock);
  if (CNTPA(pa)<=1){ // 1个引用, 无需复制
    release(&cntlock);
    return pa;
  }

  uint64 npa;
  if ((npa = (uint64)kalloc())==0){
    release(&cntlock);
    return 0;
  }
  memmove((void *)npa, pa, PGSIZE);
  --CNTPA(pa);
  release(&cntlock);
  return (void *)npa;
}

void kcntpa(void* pa)
{
  acquire(&cntlock);
  ++CNTPA(pa);
  release(&cntlock);
}