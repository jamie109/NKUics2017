#include "common.h"
#include "syscall.h"
ssize_t fs_write(int fd, const void *buf, size_t len);
static inline uintptr_t sys_none(_RegSet *r) {
  SYSCALL_ARG1(r)=1;
  return 1;
}

static inline uintptr_t sys_exit(_RegSet *r) {
  _halt(SYSCALL_ARG2(r));
  return 1;
}

static inline uintptr_t sys_write(_RegSet *r) {
  int fd = SYSCALL_ARG2(r);
  char* buf = (char*) SYSCALL_ARG3(r);
  int len = SYSCALL_ARG4(r);

  if(fd == 1 || fd == 2){
    for(int i = 0; i < len; i++){
      _putc(buf[i]);
    }
    SYSCALL_ARG1(r) = SYSCALL_ARG4(r);
  }
  return 1;
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
      sys_write(r);
      break;
    case SYS_brk:
      SYSCALL_ARG1(r)=0;
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
