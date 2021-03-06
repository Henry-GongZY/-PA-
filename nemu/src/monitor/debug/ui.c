#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

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

static int cmd_si(char *args) {
  int step;
  if(args == NULL) step = 1;
  else sscanf(args,"%d",&step);
  cpu_exec(step);
  return 0;
}

static int cmd_info(char *args){
  char *s=strtok(NULL," ");
	if(s!=NULL){
	  char t=s[0];
	  if(t=='r'){
	    //register display
		int i;
		for(i=0;i<8;i++)
		  printf("%s: 0x%08x\n",regsl[i],cpu.gpr[i]._32); //length 32 regs
		printf("eip: 0x%08x\n",cpu.eip);
	} //TODO:Watchpoint
          else if (t == 'w'){
          //watchpoint here
                list_wp();      
        }
	  else{
		printf("Unknown command '%s'.\n",s);
	}
	return 0;
    } else {
      printf("Wrong command.\n");
    }
}

static int cmd_x(char *args) {
  
  if(args == NULL) {
  	return 0;
  }

  char *args_end = args + strlen(args);
  char *n, *expression;
  unsigned int num, result;
  n = strtok(args," ");
  if(n == NULL) {
    return 0;
  }
  expression = n + strlen(n) + 1;
  if(expression >= args_end)
		  return 0;
  num = atoi(n);
  bool success = true;
  result = expr(expression, &success);
  if(!success) {
		  printf("Expression syntax error.\n");
		  return 0;
  }

  //format output as GDB
  unsigned int row = num/4;
  unsigned int left = num%4;
  
  for(unsigned int i=0;i<row;i++) {
	printf("0x%x:",result);
  	for(unsigned int j=0;j<4;j++) {
	  	printf("%12x",vaddr_read(result,4));
		result += 4;
	}
	printf("\n");
  }
  
  if(left != 0){
    printf("0x%x:",result);
    for(unsigned int i=0;i<left;i++) {
      printf("%12x",vaddr_read(result,4));
	  result += 4;
    }
    printf("\n");
  }

  return 0; 
}

static int cmd_w(char* args){
        char *expr=strtok(NULL," ");
        int NO;
	if(expr){
		NO = set_wp(expr);
                printf("Set watchpoint %d\n",NO);
        }
	return 0; 
}

static int cmd_d(char* args) {
        int t=0;
	char *s=strtok(NULL," ");
	if(s){
		t=atoi(s);
		free_wp_by_no(t);
	}
	else
		printf("Wrong command.\n");
	return 0;
}

static int cmd_p(char* args) {
    if(args == NULL) {
  	return 0;
  }
  bool success = true;
  uint32_t result = expr(args, &success);
  if(!success){
		  printf("Expression syntax error.\n");
  } else {
		  printf("%d\n",result);
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute the program by sentenses", cmd_si},
  { "info", "Print info of registers or watchpoints", cmd_info},
  { "x", "Scan main memory from the given place", cmd_x},
  /* TODO: Add more commands */
  {"d","Free watchpoint",cmd_d},
  {"w","Set watchpoint",cmd_w},
  {"p","Expression evaluation",cmd_p}
};

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
