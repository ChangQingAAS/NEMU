#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ

	/* TODO: Add more token types */
	TK_NOTYPE = 256, TK_HEX, TK_DEC, TK_REG, TK_EQ, TK_NEQ, 
  	TK_AND, TK_OR,
 	TK_NEG,      //-代表负数
  	TK_POI,       //指针解引用
  	TK_LS, TK_RS, TK_BOE, TK_LOE

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level优先级 of different rules.
	 */

	//{" +",	NOTYPE},				// spaces
	//{"\\+", '+'},					// plus
	//{"==", EQ}						// equal

	
  	{"0x[0-9A-Fa-f][0-9A-Fa-f]*", TK_HEX},
  	{"0|[1-9][0-9]*", TK_DEC},
  	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip|ax|cx|dx|bx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh)", TK_REG},

	
  	{"\\+", '+'},         // 使用单引号
  	{"-", '-'},          
  	{"\\*", '*'},
  	{"\\/", '/'},
  	{"\\(", '('},
  	{"\\)", ')'},
	{" +", TK_NOTYPE},    // spaces
  	{"==", TK_EQ},         
  	{"!=", TK_NEQ},
	{"&&", TK_AND},
  	{"\\|\\|", TK_OR},
  	{"!", '!'},
  	// 注意前缀问题 >=识别应在>前面 
  	// 类似的 十进制和十六进制位置
  	{"<<", TK_LS},
  	{">>", TK_RS},
  	{">=", TK_BOE},
  	{">", '>'},
  	{"<=", TK_LOE},
  	{"<", '<'}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return 0;
}

