#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)
//pa4
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

uint32_t vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  if(PTE_ADDR(addr) != PTE_ADDR(addr + len -1)) {
    printf("error: the data pass two pages:addr=0x%x, len=%d!\n", addr, len);
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if(PTE_ADDR(addr) != PTE_ADDR(addr + len -1)){
    printf("error: the data pass two pages:addr=0x%x, len=%d!\n", addr, len);
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, len, data);
  }
  //paddr_write(addr, len, data);
}
