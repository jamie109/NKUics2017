#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x8048000)
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern void* new_page(void);
// int fs_open(const char* path, int flags, int mode);
// size_t fs_filesz(int fd);
// ssize_t fs_read(int fd, void* buf, size_t len);
// int fs_close(int fd);
// off_t get_file_addr(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //size_t ramdisk_size = get_ramdisk_size();
  //ramdisk_read(DEFAULT_ENTRY, 0, ramdisk_size);

  int fd = fs_open(filename, 0, 0);
  int f_size = fs_filesz(fd);
  //long addr = get_file_addr(fd);
  Log("Load %d bytes file, named %s, fd %d", f_size, filename, fd);
  //fs_read(fd,(void *)DEFAULT_ENTRY, f_size);
  //pa4
  int ppnum = f_size / PGSIZE;
  if(f_size % PGSIZE != 0) {
    ppnum++;
  }
  void *pa = NULL;
  void *va = DEFAULT_ENTRY;
  for(int i = 0; i < ppnum; i++) {
    pa = new_page();
    _map(as, va, pa);
    fs_read(fd, pa, PGSIZE);
    va += PGSIZE;
  }
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}
