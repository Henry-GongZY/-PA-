#include "common.h"
#include "syscall.h"


  
static inline _RegSet* sys_none(_RegSet *r){
  SYSCALL_ARG1(r) = 1;
  return NULL;
}

static inline _RegSet* sys_exit(_RegSet *r){
  _halt(SYSCALL_ARG2(r)); 
  return NULL;
}

static inline _RegSet* sys_write(_RegSet *r){
  
  int fd = (int)SYSCALL_ARG2(r);
  char *buf = (char *)SYSCALL_ARG3(r);
  int len = (int)SYSCALL_ARG4(r);

  if(fd == 1 || fd == 2){
    for(int i = 0; i < len; i++) {
        _putc(buf[i]);
    }
    SYSCALL_ARG1(r) = len;
  }

  return NULL;
}


_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
    //pa3.1
    case SYS_none: sys_none(r); break;
    case SYS_exit: sys_exit(r); break;
    //pa3.2
    case SYS_write: sys_write(r); break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}