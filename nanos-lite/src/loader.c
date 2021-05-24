#include "common.h"
#include "memory.h"
#include "fs.h"

size_t get_ramdisk_size();
void ramdisk_read(void *,off_t,size_t);
#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  //pa3.1 
  ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  return (uintptr_t)DEFAULT_ENTRY;
}
