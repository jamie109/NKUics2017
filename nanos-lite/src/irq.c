#include "common.h"
_RegSet* do_syscall(_RegSet *r);
static _RegSet* do_event(_Event e, _RegSet* r) {
  // switch (e.event) {
  //   default: panic("Unhandled event ID = %d", e.event);
  // }
  switch (e.event) {
    case _EVENT_SYSCALL:
      do_syscall(r);
      break;
    case _EVENT_TRAP:
      printf("this is self-trap\n"); 
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
