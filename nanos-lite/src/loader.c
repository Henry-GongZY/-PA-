#include "common.h"
#include "memory.h"
#include "fs.h"

size_t get_ramdisk_size();
void ramdisk_read(void *,off_t,size_t);
#define DEFAULT_ENTRY ((void *)0x4000000)

extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_filesz(int fd);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_close(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  //pa3.1 
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());

  //pa3.2
  // int fd = fs_open("/bin/text",0,0);
  // int bytes = fs_filesz(fd); 
  // fs_read(fd,DEFAULT_ENTRY,bytes);
  // fs_close(fd);

  //pa3.3
  int fd = fs_open("/bin/events",0,0);
  int bytes = fs_filesz(fd); 
  fs_read(fd,DEFAULT_ENTRY,bytes);
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}
