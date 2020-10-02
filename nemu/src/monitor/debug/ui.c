#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "common.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
        static char *line_read = NULL;

        if (line_read) {
                free(line_read);//readline返回值由malloc分配，释放需要free
                line_read = NULL;
        }

        line_read = readline("(nemu) ");

        if (line_read && *line_read) {
                add_history(line_read);//行编辑的常用功能 历史记录
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
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);

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
        { "info", "Print regs' or watchpoint's state", cmd_info },
        { "x", "Scan the memory", cmd_x },


};

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
                                        printf("%s\t\t0x%08x\n",regsl[j],cpu.gpr[j]._32);
                                for(j=0;j<8;j++) 
                                        printf("%s\t\t0x%04x\n",regsw[j],cpu.gpr[j]._16);
                                for(j=0;j<8;j++) 
                                        printf("%s\t\t0x%02x\n",regsb[j],cpu.gpr[j%4]._8[j/4]);
                                return 0;
                        }
                        else if(strcmp(arg, subcmd[1]) == 0) //'w'打印监视点信息
                        {
                                // show_wp();
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

static int cmd_si(char *args){
        // printf("have enter the cmd_si\n");
        if(args == NULL|| !(args[0]>='0'&&args[0]<='9')){
                args = "1";
        }
        int singleStepRunNum = atoi(args);//atoi字符转数字
        printf("%-10d\n",singleStepRunNum);
        int i;
        for(i = 0; i<singleStepRunNum;i++)
                cpu_exec(1);
        
        return 0;
}

static int cmd_x(char *args) {
        int N;
        char *arg = strtok(NULL, " ");
        if(arg == NULL)
        {
                printf("Lack of parameter!\n");
                return 0;
        }
        N = atoi(arg);
        if(N==0){
                printf("Unknown command '%s'\n",arg);
                return 0;  //N=0时无法输出
        }
        printf("N is %d\n",N);//测试能否输出N

		char* expr = strtok(NULL, " ");
        if(expr == NULL)
        {
                printf("Lack of parameter!\n");
                return 0;
        }
         printf("expr is %s\n",expr);//测试能否输出expr

		//PA1  stage2
		char *str;
		hwaddr_t addr = strtol(expr,&str,16);
        int i;
        for( i=0;i<N;i++)
        {
                printf("0x%08x:\t0x%08x\n",addr,hwaddr_read(addr,4));
                addr = addr+4;
        }
		
        return 0;
}
// bool *success=false;

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