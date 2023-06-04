#include "common.h"
#include "syscall.h"
#include "fs.h"

// int fs_open(const char* path, int flags, int mode);
// ssize_t fs_write(int fd, const void *buf, size_t len);
// ssize_t fs_read(int fd, void* buf, size_t len);
// int fs_close(int fd);
// off_t fs_lseek(int fd, off_t offset, int whence);

static inline uintptr_t sys_none(_RegSet *r) {
  SYSCALL_ARG1(r)=1;
  return 1;
}

static inline uintptr_t sys_exit(_RegSet *r) {
  _halt(SYSCALL_ARG2(r));
  return 1;
}

static inline int sys_write(_RegSet *r) {
  int fd = SYSCALL_ARG2(r);
  char* buf = (char*) SYSCALL_ARG3(r);
  int len = SYSCALL_ARG4(r);

  if(fd == 1 || fd == 2){
    //Log("buffer:%s", (char*)buf);
    for(int i = 0; i < len; i++){
      _putc(buf[i]);
    }
    //SYSCALL_ARG1(r) = SYSCALL_ARG4(r);
    return len;
  }
  if(fd >= 3){
    return fs_write(fd, buf, len);
  }
  return -1;
}
static inline int sys_read(_RegSet *r) {
  int fd = SYSCALL_ARG2(r);
  char* buf = (char*) SYSCALL_ARG3(r);
  int len = SYSCALL_ARG4(r);
  return fs_read(fd, buf, len);
}
static inline int sys_open(uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  return fs_open((char *)pathname,flags,mode);
}

static inline int sys_close(uintptr_t fd) {
  return fs_close(fd);
}
static inline int sys_lseek(_RegSet *r) {
  int fd = (int)SYSCALL_ARG2(r);
  off_t offset = (off_t)SYSCALL_ARG3(r);
  int whence = (int)SYSCALL_ARG4(r);
  return fs_lseek(fd,offset,whence);
  //return NULL;
}
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none:
      //printf("syscall ID = 0,sys_none\n");
      sys_none(r);
      break;
    case SYS_exit:
      //printf("syscall ID = 4, sys_exit\n");
      sys_exit(r);
      break;
    case SYS_write:
      SYSCALL_ARG1(r)=sys_write(r);
     // sys_write(r);
      break;
    case SYS_brk:
      SYSCALL_ARG1(r)=0;
      break;
    case SYS_open:
      SYSCALL_ARG1(r)=sys_open(a[1],a[2],a[3]);
      break;
    case SYS_read:
      SYSCALL_ARG1(r)=sys_read(r);
      break;
    case SYS_close:
      SYSCALL_ARG1(r)=sys_close(a[1]);
      break;
    case SYS_lseek:
      SYSCALL_ARG1(r)=sys_lseek(r);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
