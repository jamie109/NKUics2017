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

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

off_t fs_filesz(int fd){
  return file_table[fd].size;
}

off_t get_file_addr(int fd){
  return file_table[fd].disk_offset;
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
  Log("fs_read fd=%d",fd);
  if(fd>=NR_FILES)
    assert(0);
  if(fd < 3){
    Log("fd(%d) < 3",fd);
    return 0;
  }
    
  ssize_t f_size = fs_filesz(fd);
  if(file_table[fd].open_offset + len >= f_size){
    len = f_size - file_table[fd].open_offset;
  }
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  Log("read finish");
  return len;
}

ssize_t fs_write(int fd, const void* buf, size_t len){
  // Log("write %s,open_offset:%d,disk_offset:%d,len:%d",
  //     file_table[fd].name,
  //     file_table[fd].open_offset,
  //     file_table[fd].disk_offset,
  //     len);
  ssize_t f_size = fs_filesz(fd);
  switch(fd){
    case FD_STDOUT:
    case FD_STDERR:{
      // stdout stderr
      for(int i=0;i<len;i++){
        _putc(((char*)buf)[i]);
      }
      break;
    }
    case FD_FB:{
      // frame buffer
      fb_write(buf, file_table[fd].open_offset, len);
      file_table[fd].open_offset+=len;
      break;
    }
    default:{
      if(file_table[fd].open_offset + len > f_size){
        len = f_size - file_table[fd].open_offset;
      }
      ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      Log("write finish");
      break;
    }
  }
  return len;
}

int fs_close(int fd){
  Log("Close %s with fd:%d..",
    file_table[fd].name,
    fd);
  file_table[fd].open_offset = 0;
  return 0;
}

off_t fs_lseek(int fd, off_t offset, int whence){
  off_t ret = -1;
  switch(whence){
    case SEEK_SET:{
      if(offset>=0 && offset<=file_table[fd].size){
        file_table[fd].open_offset = offset;
        ret = file_table[fd].open_offset;
      }
      break;
    }
    case SEEK_CUR:{
      if(offset+file_table[fd].open_offset>=0 && offset+file_table[fd].open_offset<=file_table[fd].size){
        file_table[fd].open_offset += offset;
        ret = file_table[fd].open_offset;
      }
      break;
    }
    case SEEK_END:{
      file_table[fd].open_offset = file_table[fd].size + offset;
      ret = file_table[fd].open_offset;
      break;
    }
    default:{
      Log("undefined whence..");
      assert(0);
    }
  }
  Log("finish fs_lseek fd=%d",
    fd);
  return ret;
}