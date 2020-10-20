#include "cpu/exec/helper.h"
#include "monitor/monitor.h"

#include <time.h>
#include <setjmp.h>

static void output_string(uint32_t str)
{
    int ch;
    while (1) {
        ch = swaddr_read(str, 1, R_DS);
        if (ch == 0) break;
        putchar(ch);
        str++;
    }
//    printf("\n");
}


#define MAX_TIMING 100
static struct timespec ts_begin[MAX_TIMING], ts_end[MAX_TIMING];
static unsigned ts_id;
static double ts_minus(const struct timespec *tsp1, const struct timespec *tsp2)
{
    /* tsp1 - tsp2 (in ms)
     * note: pay attation to rounding method */
    return (tsp1->tv_sec - tsp2->tv_sec) * 1000 +
           (tsp1->tv_nsec - tsp2->tv_nsec) / 1000000.0;
}

make_helper(zby) {
    
	print_asm("zby (eax = %d)", cpu.eax);

//    panic_to_ui("ZBY instruction is disabled!");

	switch (cpu.EAX) {
	    case 100:
	        printf("ZBY: ");
	        output_string(cpu.ECX); // output string from VM
	        break;
	        
	    case 200: // return realtime-clock, EDX = tv_sec, EAX = tv_nsec;
	        do {
	            struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                cpu.EDX = ts.tv_sec;
                cpu.EAX = ts.tv_nsec;
	        } while (0);
	        break;
	    
	    case 300: // start timing
	        ts_id = cpu.EDX; if (ts_id > MAX_TIMING) ts_id = 0;
	        clock_gettime(CLOCK_REALTIME, &ts_begin[ts_id]);
	        break;
	    case 301: // stop timing, and output string is in ECX
	        ts_id = cpu.EDX; if (ts_id > MAX_TIMING) ts_id = 0;
	        clock_gettime(CLOCK_REALTIME, &ts_end[ts_id]);
	        printf("%s%s  [timing %d]: ", c_blue, c_bold, ts_id);
	        if (cpu.ECX) output_string(cpu.ECX);
	        printf(" - %.2f ms%s\n", ts_minus(&ts_end[ts_id], &ts_begin[ts_id]), c_normal);
	        break;
	    
	    default:
	        panic_to_ui("\33[1;31mnemu: INVALID ZBY INSTRUCTION, EAX = 0x%08x\33[0m at eip = 0x%08x\n\n",
					cpu.eax, cpu.eip);
	}

	return 1;
}
