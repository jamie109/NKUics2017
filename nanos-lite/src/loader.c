#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern void* new_page(void);
int fs_open(const char* path, int flags, int mode);
size_t fs_filesz(int fd);
ssize_t fs_read(int fd, void* buf, size_t len);
int fs_close(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //size_t ramdisk_size = get_ramdisk_size();
  //ramdisk_read(DEFAULT_ENTRY, 0, ramdisk_size);

  int fd = fs_open(filename, 0, 0);
  int f_size = fs_filesz(fd);

  Log("Load %d bytes file, named %s, fd %d", f_size, filename, fd);
  
  void* pa = DEFAULT_ENTRY;
  void* va = DEFAULT_ENTRY;
  Log("pa va ");
  return (uintptr_t)DEFAULT_ENTRY;
  while(f_size > 0){
    pa = new_page();
    _map(as, va, pa);
    fs_read(fd, pa, PGSIZE);
    va += PGSIZE;
    f_size -= PGSIZE;
  }
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}
