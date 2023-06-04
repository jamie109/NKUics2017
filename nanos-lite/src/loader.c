#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern void* new_page(void);
int fs_open(const char* path, int flags, int mode);
size_t fs_filesz(int fd);
ssize_t fs_read(int fd, void* buf, size_t len);
int fs_close(int fd);
off_t get_file_addr(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //size_t ramdisk_size = get_ramdisk_size();
  //ramdisk_read(DEFAULT_ENTRY, 0, ramdisk_size);

  int fd = fs_open(filename, 0, 0);
  int f_size = fs_filesz(fd);
  long addr = get_file_addr(fd);
  Log("Load %d bytes file, named %s, fd %d", f_size, filename, fd);
  fs_read(fd,(void *)addr , f_size);

  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}
