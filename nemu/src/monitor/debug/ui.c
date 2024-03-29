#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "common.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <elf.h>
void cpu_exec(uint32_t);

void GetFunctionAddr(swaddr_t EIP,char* name);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
        static char *line_read = NULL;

        if (line_read) {
                free(line_read);
                line_read = NULL;
        }

        line_read = readline("(nemu) ");

        if (line_read && *line_read) {
                add_history(line_read);//行编辑的常用功能 历史记录
        }

        return line_read;
}

static int cmd_help(char *args);
static int cmd_c(char *args);
static int cmd_q(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);
static int cmd_bt(char *args);
static struct {
        char *name;
        char *description;
        int (*handler) (char *);//handler执行该操作的函数
} cmd_table [] = {
        { "help", "Display informations about all supported commands", cmd_help },
        { "c", "Continue the execution of the program", cmd_c },
        { "q", "Exit NEMU", cmd_q },
        /* TODO: Add more commands */
        {"si","Run N single steps",cmd_si},
        { "info", "Print regs' or watchpoints' state", cmd_info },
        { "x", "Scan the memory", cmd_x },
        {"p","Evaluate a expression", cmd_p},
        { "w", "Set a watchpoint", cmd_w },
        { "d", "Delete a watchpoint", cmd_d },
        { "bt", "Print stack frame chain", cmd_bt}
};

typedef struct {
	swaddr_t prev_ebp;
	swaddr_t ret_addr;
	uint32_t args[4];
}PartOfStackFrame ;


#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
        /* extract the first argument */
        char *arg = strtok(NULL, " ");//第一次用参数，第二次用NULL
        int i;

        if(arg == NULL) {
                /* no argument given */
                for(i = 0; i < NR_CMD; i ++) {
                        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                }
                
        }
        else {
                for(i = 0; i < NR_CMD; i ++) {
                        if(strcmp(arg, cmd_table[i].name) == 0) {
                                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                                return 0;
                        }
                }
                printf("Unknown command '%s'\n", arg);
        }
        return 0;
}

static int cmd_c(char *args) {
        cpu_exec(-1);
        return 0;
}

static int cmd_q(char *args) {
        return -1;
}

static int cmd_si(char *args){
        // printf("have enter the cmd_si\n");
        //当N没有给出时, 缺省为1
        if(args == NULL|| !(args[0]>='0'&&args[0]<='9')){
                args = "1";
        }
        int singleStepRunNum = atoi(args);//atoi字符转数字
        cpu_exec(singleStepRunNum);
        ///程序单步执行N条指令后暂停
        //int i;
        // for(i = 0; i<singleStepRunNum;i++)
        //         cpu_exec(1);
        
        return 0;
}

static int cmd_info(char *args) {
        char *subcmd[] = {"r","w"};
        // int i;
        char *arg = strtok(NULL, " ");
        if (arg != NULL)
        {
                        if(strcmp(arg, subcmd[0]) == 0)//"r"打印寄存器状态
                        {
                                int j;
                                for(j=0;j<8;j++)
                                        printf("%s\t\t0x%08x\t\t%d\n",regsl[j],cpu.gpr[j]._32,cpu.gpr[j]._32);
                                printf("%s\t\t0x%08x\t\t%d\n", "eip", cpu.eip, cpu.eip);
                                for(j=0;j<8;j++) 
                                        printf("%s\t\t0x%04x\t\t\t%d\n",regsw[j],cpu.gpr[j]._16,cpu.gpr[j]._16);
                                for(j=0;j<8;j++) 
                                        printf("%s\t\t0x%02x\t\t\t%d\n",regsb[j],cpu.gpr[j%4]._8[j/4],cpu.gpr[j%4]._8[j/4]);
                                return 0;
                        }
                        else if(strcmp(arg, subcmd[1]) == 0) //'w'打印监视点信息
                        {
                                show_watchpoint();
                                return 0;
                        }
                        else
                        {
                                printf("Unknown command '%s'\n", arg);
                                return 0;
                        }       
        }
        printf("Lack of parameter!\n");
        return 0;
}

static int cmd_x(char *args) {
        char *arg = strtok(NULL, " ");
        if(arg == NULL)
        {
                printf("Lack of parameter!\n");
                return 0;
        }
        int printNumber = atoi(arg);
        if(printNumber == 0){
                printf("Unknown command '%s'\n",arg);
                return 0;  //N=0时无法输出
        }
       
	char* expression = strtok(NULL, " ");
        if(expression == NULL)
        {
                printf("Lack of parameter!\n");
                return 0;
        }
        //  printf("expression is %s\n",expression);//测试能否输出expr

	// bool *success=false;
	// swaddr_t addr = expr(expression, success);
        bool success = true;
        swaddr_t addr = expr(expression, &success);
	if(!success)
                {printf("Wrong Expression!\n");return 0;}
        int i;
        for( i = 0; i < printNumber; i++)
        {
                printf("0x%08x:\t0x%08x\n",addr,swaddr_read(addr,4));
                addr = addr+4;
        }
        return 0;
}

static int cmd_p(char *args) {
        if(args == NULL){
                printf("Lack of parameter!\n");
                return 0;
        }
        // uint32_t computedResult = expr(args, success);
        bool success = true;
        uint32_t computedResult = expr(args, &success);
	if(!success)
                {printf("Wrong Expression!\n");return 0;}
        printf("0x%08x(%d)\n", computedResult, computedResult); 
        return 0;
}

static int cmd_w(char *args){
         if(args == NULL){
                printf("Lack of parameter!\n");
                return 0;
        }
        WP* newWatchpoint =  new_watchpoint();
        strcpy(newWatchpoint->expression,args);

        bool success = true;
        newWatchpoint->old_address = expr(args, &success);
	if(!success)
                {printf("Wrong Expression!\n");return 0;}

        newWatchpoint->value = swaddr_read(newWatchpoint->old_address,4);
        printf("Set watchpoint NO.%d on 0x%08x\n",newWatchpoint->NO,newWatchpoint->old_address);
        return 0;
}

static int cmd_d(char *args){
        if(args == NULL){
                printf("Lack of parameter!\n");
                return 0;
        }
        int watchpointNO = atoi(args);
        free_watchpoint(watchpointNO);
        return 0;
}

static int cmd_bt(char* args){
	if (args != NULL){
		printf("Wrong Command!");
		return 0;
	}
	PartOfStackFrame EBP;
	char name[32];
	int cnt = 0;
	EBP.ret_addr = cpu.eip;
	swaddr_t addr = cpu.ebp;
	// printf("%d\n",addr);
	int i;
	while (addr){
		GetFunctionAddr(EBP.ret_addr,name);
		if (name[0] == '\0') break;
		printf("Number: %d\t0x%08x\t",cnt++,EBP.ret_addr);
		printf("%s",name);
		EBP.prev_ebp = swaddr_read(addr,4);
		EBP.ret_addr = swaddr_read(addr + 4, 4);
		printf("(");
		for (i = 0;i < 4;i ++){
			EBP.args[i] = swaddr_read(addr + 8 + i * 4, 4);
			printf("0x%x",EBP.args[i]);
			if (i == 3) printf(")\n");else printf(", ");
		}
		addr = EBP.prev_ebp;
	}
	return 0;
}

void ui_mainloop() {
        while(1) {
                char *str = rl_gets();
                char *str_end = str + strlen(str);

                /* extract the first token as the command */
                char *cmd = strtok(str, " ");
                if(cmd == NULL) { continue; }

                /* treat the remaining string as the arguments,
                 * which may need further parsing//分析
                 */
                char *args = cmd + strlen(cmd) + 1;
                if(args >= str_end) {
                        args = NULL;
                }

#ifdef HAS_DEVICE
                extern void sdl_clear_event_queue(void);
                sdl_clear_event_queue();
#endif

                int i;
                for(i = 0; i < NR_CMD; i ++) {
                        if(strcmp(cmd, cmd_table[i].name) == 0) {
                                if(cmd_table[i].handler(args) < 0) { return; }
                                break;
                        }
                }

                if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
        }
}
