#include "proc.h"
#include "memory.h"

static void *pf = NULL;

#define K4(va) (((uint32_t)(va)+0xfff) & ~0xfff)

void* new_page(void) {
  assert(pf < (void *)_heap.end);
  void *p = pf;
  pf += PGSIZE;
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {
  if(current->cur_brk == 0)  current->cur_brk = current->max_brk = new_brk;
  else{
    if(new_brk > current->max_brk) {
      uint32_t brk = K4(current->max_brk);
      while(brk<new_brk) {
        _map(&current->as,(void*)brk,new_page());
        brk += PGSIZE;
      }
      current->max_brk = new_brk;
    }
    current->cur_brk = new_brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _pte_init(new_page, free_page);
}
