#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int port;
  if((port = is_mmio(addr)) != -1)
	    return mmio_read(addr, len, port);
  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int port;
  if((port = is_mmio(addr)) != -1)
	    mmio_write(addr, len, data, port);
  memcpy(guest_to_host(addr), &data, len);
}

paddr_t page_translate(vaddr_t addr,int rw)
{
  CR0* pcr0 = &cpu.cr0.val;
  if(!pcr0->protect_enable||!pcr0->paging)
    return addr;

  PDE* pd = (PDE*)(cpu.cr3&0xfffff000);
  int pd_index = addr>>22;
  PTE* pt;
  if(pd[pd_index].present) {
    pt = pd[pd_index].page_frame<<12;
    pd[pd_index].accessed=1;
  } else {
    Assert(0,"pde present bit is 0!");
  }

  int pt_index = (addr<<10)>>22;
  if(pt[pt_index].present) {
    addr = (pt[pt_index].page_frame<<12) | (addr&0xfff);
    pt[pt_index].accessed = 1;
    if(rw)
      pt[pt_index].dirty = 1;
  } else {
    Assert(0,"pte present bit is 0!");
  }
  return (paddr_t)addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if((addr&0xfff)+len-1 >= PAGE_SIZE)
      assert(0);
  else{
      paddr_t paddr = page_translate(addr,0);
      return paddr_read(paddr,len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if((addr&0xfff)+len-1 >= PAGE_SIZE)
    assert(0);
  else{
    paddr_t paddr = page_translate(addr,1);
    paddr_write(paddr, len, data);
  }
}
