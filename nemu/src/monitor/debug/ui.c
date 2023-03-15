#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

void for_expr_test( char *);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  // default is 1
	int num = 1;
  // if N exists
  if(args != NULL)
    sscanf(args, "%d", &num);

	cpu_exec(num);
  return 0;
}

static int cmd_info(char *args){
  //info r
  char s[5];
  if(args != NULL){
    int n = sscanf(args, "%s",s);
    //if s =="r" print regs
    if(n == 1 && strcmp(s, "r") == 0){
      // 32bit
      for(int i = 0; i < 8; i++)
        printf("%s\t0x%x\t%d\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
      // 16bit
      for(int i = 0; i < 8; i++)
        printf("%s\t0x%x\t%d\n", regsw[i],cpu.gpr[i]._16,cpu.gpr[i]._16);
      // 8bit
      for(int i = 0; i < 4; i++)
        printf("%s\t0x%x\t%d\n", regsb[i], cpu.gpr[i]._8[0], cpu.gpr[i]._8[0]);
      for(int i = 0; i < 4; i++)
        printf("%s\t0x%x\t%d\n", regsb[i+4], cpu.gpr[i]._8[1], cpu.gpr[i]._8[1]);
      //eip
      printf("eip\t0x%x\t%d\n", cpu.eip, cpu.eip);
    }
  }
  return 0;
}

static int cmd_x(char *args){  
    char *arg1=strtok(NULL," ");  
    char *arg2=strtok(NULL," ");  
    int n;  
    vaddr_t address;  
    // N start_address
    sscanf(arg1,"%d",&n);  
    sscanf(arg2,"%x",&address);  
      
    for(int i = 0; i < n; i++){  
      printf("0x%x:\t", address);  
      printf("%x ", vaddr_read(address,4));  
      printf("\n");
      address += 4;  
    }    
    return 0;  
}  

// for test
static int cmd_t(char *args){  
  // get expr
  char *arg = strtok(NULL, " ");
  // bool success;
  // uint32_t res = expr(arg, &success);
  // bool flag = make_token(arg);
  for_expr_test(arg);
  printf("@ from ui.c cmd_t function.\n ");  

  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {"si", "Pause execution after stepping N instructions, When N is not given, the default is 1", cmd_si},
  {"info", "Output the values of all regs.", cmd_info},
  {"x", "Scan memory", cmd_x},
  {"t", "use for test my pa1 expr", cmd_t},

};
// the number of cmd
#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
