#include "common.h"
#include "device/port-io.h"

#define PORT_IO_SPACE_MAX 65536//端口 I/O 空间的最大值
#define NR_MAP 8//maps 数组的大小

/* "+ 3" is for hacking, see pio_read() below */
static uint8_t pio_space[PORT_IO_SPACE_MAX + 3];//静态数组，用于存储端口 I/O 的数据
// 结构用于记录一个端口 I/O 映射的关系
typedef struct {
  ioaddr_t low;//端口 I/O 的低地址、高地址
  ioaddr_t high;
  pio_callback_t callback;//回调函数
} PIO_t;

static PIO_t maps[NR_MAP];//记录端口 I/O 映射的关系
static int nr_map = 0;//当前 maps 数组中已经映射的数量
//是根据给定的地址、长度和操作类型，在 maps 数组中查找对应的映射关系，并调用相应的回调函数
static void pio_callback(ioaddr_t addr, int len, bool is_write) {
  int i;
  for (i = 0; i < nr_map; i ++) {
    if (addr >= maps[i].low && addr + len - 1 <= maps[i].high) {//找到满足地址范围的映射关系
      maps[i].callback(addr, len, is_write);
      return;
    }
  }
}

/* device interface 
向 maps 数组中添加新的端口 I/O 映射关系，并返回相应的内存地址*/
void* add_pio_map(ioaddr_t addr, int len, pio_callback_t callback) {
  //映射的数量不超过上限。映射的地址范围不超过 PORT_IO_SPACE_MAX。
  assert(nr_map < NR_MAP);
  assert(addr + len <= PORT_IO_SPACE_MAX);
  maps[nr_map].low = addr;
  maps[nr_map].high = addr + len - 1;
  maps[nr_map].callback = callback;
  nr_map ++;
  return pio_space + addr;
}


/* CPU interface 
从端口 I/O 中读取数据*/
uint32_t pio_read(ioaddr_t addr, int len) {
  assert(len == 1 || len == 2 || len == 4);
  assert(addr + len - 1 < PORT_IO_SPACE_MAX);
  pio_callback(addr, len, false);		// prepare data to read
  uint32_t data = *(uint32_t *)(pio_space + addr) & (~0u >> ((4 - len) << 3));
  return data;
}

void pio_write(ioaddr_t addr, int len, uint32_t data) {
  assert(len == 1 || len == 2 || len == 4);
  assert(addr + len - 1 < PORT_IO_SPACE_MAX);
  memcpy(pio_space + addr, &data, len);
  pio_callback(addr, len, true);
}

