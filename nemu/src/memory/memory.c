#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)
//pa4
#include "memory/mmu.h"
#define PTXSHFT 12 //线性地址偏移量
#define PDXSHFT 22 //线性地址偏移量 

#define PTE_ADDR(pte) ((uint32_t)(pte) & ~0xfff)
#define PDX(va) (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va) (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va) ((uint32_t)(va) & 0xfff)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  //return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  //pa2 update
  int r = is_mmio(addr);
  if(r == -1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
    return mmio_read(addr,len,r);
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  //memcpy(guest_to_host(addr), &data, len);
  //pa2 update
  int r = is_mmio(addr);
  if(r == -1)
    memcpy(guest_to_host(addr), &data, len);
  else
    mmio_write(addr,len,data,r);
}
//pa4 add
paddr_t page_translate(vaddr_t addr, bool iswrite) {
  CR0 cr0 = (CR0)cpu.CR0;
  if(cr0.paging && cr0.protect_enable) {
    CR3 crs = (CR3)cpu.CR3;

    PDE *pgdirs = (PDE*)PTE_ADDR(crs.val);
    PDE pde = (PDE)paddr_read((uint32_t)(pgdirs + PDX(addr)), 4);

    PTE *ptable = (PTE*)PTE_ADDR(pde.val);
    PTE pte = (PTE)paddr_read((uint32_t)(ptable + PTX(addr)), 4);
    //printf("hhahah%x, jhhh%x\n", pte.present, addr);
    Assert(pte.present, "addr=0x%x", addr);

    pde.accessed=1;
    pte.accessed=1;
    if(iswrite) {
      pte.dirty=1;
    }
    paddr_t paddr = PTE_ADDR(pte.val) | OFF(addr);
    Log("vaddr=0x%x\tpaddr=0x%x", addr, paddr);
    return paddr;
  }
	return addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  if(PTE_ADDR(addr) != PTE_ADDR(addr + len -1)) {
    printf("data cross the page boundary [addr=0x%x, len=%d]\n", addr, len);
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if(PTE_ADDR(addr) != PTE_ADDR(addr + len -1)){
    printf("data cross the page boundary [addr=0x%x, len=%d]\n", addr, len);
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, len, data);
  }
  //paddr_write(addr, len, data);
}
