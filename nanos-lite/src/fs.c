#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;//目前文件操作的位置
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  //stdin,stdout 和 stderr 的占位表项,它们只是为了保证我们的简易文件系统和约定的标准输 入输出的文件描述符保持一致
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void *buf, off_t offset, size_t len);
extern void fb_write(const void *buf, off_t offset, size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern size_t events_read(void *buf, size_t len);
extern void getScreen(int *width, int *height) ;
void init_fs() {
  // TODO: initialize the size of /dev/fb
  int width = 0;
  int height = 0;
  getScreen(&width, &height);
  file_table[FD_FB].size = width * height * sizeof(u_int32_t);
  Log("set FD_FB size = %d", file_table[FD_FB].size);
}
//help functions
size_t fs_filesz(int fd){
  return file_table[fd].size;
}

off_t disk_offset(int fd){
  return file_table[fd].disk_offset;
}

off_t get_open_offset(int fd){
  return file_table[fd].open_offset;
}

void set_open_offset(int fd, off_t n){
  if(n > file_table[fd].size) {
    n = file_table[fd].size;
    }
  file_table[fd].open_offset = n;
}

int fs_open(const char* path, int flags, int mode){
  Log("Pathname: %s", path);
  for(int i = 0; i<NR_FILES; i++){
    if(strcmp(file_table[i].name, path) == 0){
      Log("find it, i=%d",i);
      return i;
    }
  }
  assert(0);
  return -1;
}

ssize_t fs_read(int fd, void* buf, size_t len){
  assert(fd >= 0 && fd < NR_FILES); 
  if(fd < 3) { 
    Log("arg invalid:fd<3"); 
    return 0; 
    } 
  int n = fs_filesz(fd)-file_table[fd].open_offset; 
  if(n > len) { 
    n = len;
  }
  ramdisk_read(buf, disk_offset(fd) + file_table[fd].open_offset, n);
  file_table[fd].open_offset = file_table[fd].open_offset + n;
  Log("Read %s from %d. open_offset:%d,disk_offset:%d,len:%d",
      file_table[fd].name,
      file_table[fd].disk_offset + file_table[fd].open_offset,
      file_table[fd].open_offset,
      file_table[fd].disk_offset,
      len);
  return n;
}
extern void fb_write(const void *buf, off_t offset, size_t len);

ssize_t fs_write(int fd, const void* buf, size_t len){
  if(fd < 3) {
  Log("arg invalid:fd<3");
  return 0;
  }
  int f_offset = file_table[fd].open_offset;
  int n = fs_filesz(fd) - f_offset;
  if(n > len) {
    n = len;
  }
  if(fd == FD_FB){
    Log("FD_FB");
    fb_write(buf, get_open_offset(fd), n);
  }
 else{
    ramdisk_write(buf, file_table[fd].disk_offset + f_offset, n);
 }
  file_table[fd].open_offset = f_offset + n;

  Log("Write %s. open_offset:%d,disk_offset:%d,len:%d",
      file_table[fd].name,
      file_table[fd].open_offset,
      file_table[fd].disk_offset,
      len);
  return len;
}

int fs_close(int fd){
  Log("Close %s with fd:%d..",
    file_table[fd].name,
    fd);
  assert(fd >= 0 && fd < NR_FILES);
  //file_table[fd].open_offset = 0;
  return 0;
}

off_t fs_lseek(int fd, off_t offset, int whence){
  switch(whence) {
    case SEEK_SET:
      set_open_offset(fd, offset);
      return get_open_offset(fd);
    case SEEK_CUR:
      set_open_offset(fd, get_open_offset(fd) + offset);
      return get_open_offset(fd);
    case SEEK_END:
      set_open_offset(fd, fs_filesz(fd) + offset);
      return get_open_offset(fd);
    default:
      panic("Unhandled whence ID = %d", whence);
      return -1;
  }
  Log("fs_lseek");
}