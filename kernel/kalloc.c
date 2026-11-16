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

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void
kinit()
{
  char *which_cpu[8] = {"kmem1","kmem2","kmem3","kmem4","kmem5","kmem6","kmem7","kmem8"}; // MAX cpus - 8 , can be lowered !.
  for(int i = 0 ; i < NCPU ; i ++){ 
    initlock(&kmem[i].lock, which_cpu[i]);
  }
  freerange(end, (void*)PHYSTOP);
}

void
kinit_free(void *pa , int ncpu)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kinit_free");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem[ncpu].lock);
  r->next = kmem[ncpu].freelist;
  kmem[ncpu].freelist = r;
  release(&kmem[ncpu].lock);
}


void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  int core = 0 ;
  p = (char*)PGROUNDUP((uint64)pa_start);

    for(; p + PGSIZE <= (char*)pa_end ; p += PGSIZE){

      kinit_free((void *)p , core);
      core = (core + 1) % NCPU; 
    }
}


// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;


  push_off();
  int id = cpuid();

  
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  pop_off();
  acquire(&kmem[id].lock);
  r->next = kmem[id].freelist;
  kmem[id].freelist = r;
  release(&kmem[id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int id = cpuid();
  
  int count = 0;
  r = 0;
  
  while(count < NCPU) {
    acquire(&kmem[id].lock);
    r = kmem[id].freelist;
    if(r)
      kmem[id].freelist = r->next;
    release(&kmem[id].lock);
    if(r)
      break;

    id = (id + 1) % NCPU;
    count ++;
  }
  pop_off();
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;

}
