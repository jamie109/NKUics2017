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

static inline uintptr_t sys_write(uintptr_t fd, uintptr_t buf, uintptr_t len) {
    return fs_write(fd,(void *)buf,len);
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
      SYSCALL_ARG1(r)=sys_write(a[1],a[2],a[3]);
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
