#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	//NOTYPE = 256, EQ

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

	{" +", TK_NOTYPE},    // spaces
  	{"0x[0-9A-Fa-f][0-9A-Fa-f]*", TK_HEX},
  	{"0|[1-9][0-9]*", TK_DEC},
  	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip|ax|cx|dx|bx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh)", TK_REG},

	
  	{"\\+", '+'},         // 使用单引号
  	{"-", '-'},          
  	{"\\*", '*'},
  	{"\\/", '/'},

  	{"\\(", '('},
  	{"\\)", ')'},
	
  	{"==", TK_EQ},         
  	{"!=", TK_NEQ},

	{"&&", TK_AND},
  	{"\\|\\|", TK_OR},
	//where is TK_NEG fushu
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
int nr_token;//已识别出的token数量

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

				// switch(rules[i].token_type) {
				// 	default: panic("please implement me");
				// }

				//i以取出，不需要switch，直接赋值即可
				if(rules[i].token_type == TK_NOTYPE) //空格直接舍弃
				break;
				if(substr_len>31)  //str溢出 false报错
					assert(0);
				memset(tokens[nr_token].str,'\0',32); //以防万一
				strncpy(tokens[nr_token].str, substr_start, substr_len);// 类似上面的%.*s

				tokens[nr_token].type = rules[i].token_type;
				// Log("Save in type=%d, str=%s",tokens[nr_token].type,tokens[nr_token].str);
				nr_token = nr_token + 1;
				
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

uint32_t eval(int p,int q){
    if(p>q){   
        // printf("Bad expression\n");
        return 0;
        // assert(0);
    }
    else if(p==q){
        // Single token.
		// For now this token should be a number.
		// Return the value of the number.
        }
    }
    else if(check_parentheses(p,q) == true){
		// The expression is surrounded by a matched pair of parentheses.
		// If that is the case, just throw away the parentheses.
        return eval(p+1,q-1);
    }
    else{
		// We should do more things here
		//op = the position of dominant operator in the token expression;
		val1 = eval(p, op - 1);
		val2 = eval(op +1, q);
		switch(op_type){
			case '+': return val1 + val2;
			case '-';
			default: assert(0);
		}
	}
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

