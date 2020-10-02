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
	//TK_NEG and TK_POI is implement in expr();
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

// char *deleteSpaceInString(char *str){
// 		char *p=str;
// 		int i=0;
// 		while(*p)
// 		{
// 			if(*p!=' ')
// 			str[i++] = *p;
// 			p++;
// 		}
// 		str[i]='\0';
// 		printf("after delete space, expression is %s\n",p);
// 		return p;
// }

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
				Log("Save in type=%d, str=%s",tokens[nr_token].type,tokens[nr_token].str);
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

bool check_parentheses(int p, int q){
	if((tokens[p].str[0]=='(') && (tokens[q].str[0]==')')){
		int leftBracketCount = 0;
		int rightBracketCount =0;
		int i;
		for(i = p; i<=q; i++){
			if(tokens[i].str[0] == '(' ){
				leftBracketCount++;
			}
			else if(tokens[i].str[0] == ')'){
				rightBracketCount++;
				if(leftBracketCount == rightBracketCount && i != q){
					printf("leftBracketCount isn't equal to rightBracketCount!\n");
					return false;	
				}
			}
		}
		if(leftBracketCount == rightBracketCount){
			if(tokens[q].str[0] == '('){
				printf("Wrong parenteses!\n");
				return false;
			}
			else{
				return true;
			}
		}
		return false;
	}
	else{
		// printf("The whole expression isn't surrounded by a pair of bracket!\n");
		return false;
	}
}

uint32_t eval(int p,int q){
    if(p>q){   //缺省:9+   ;    --9
        // printf("Bad expression\n");
        return 0;
        // assert(0);
    }
    else if(p==q){
		// Single token.
		// For now this token should be a number.
		// Return the value of the number.
		uint32_t result;
		if(tokens[p].type == TK_HEX)
			sscanf(tokens[p].str,"%x",&result);
		else if(tokens[p].type == TK_DEC)
			sscanf(tokens[p].str,"%d",&result);
		else if(tokens[p].type == TK_REG){
            char tmp[3] = {tokens[p].str[1],tokens[p].str[2],tokens[p].str[3]};
            int i;
			for( i=0;i<8;i++)
                if(!strcmp(tmp,regsl[i])){return cpu.gpr[i]._32;}
            for( i=0;i<8;i++)
                if(!strcmp(tmp,regsw[i])){return cpu.gpr[i]._16;}
            for( i=0;i<8;i++) 
                if(!strcmp(tmp,regsb[i])){return cpu.gpr[i%4]._8[i/4];}
	    	char teip[3]="eip";
	    	if(strcmp(tmp,teip))return cpu.eip;
        }
        else assert(0);
		return result;
    }
    else if(check_parentheses(p,q) == true){
		// The expression is surrounded by a matched pair of parentheses.
		// If that is the case, just throw away the parentheses.
        return eval(p+1,q-1);
    }
    else{
		// We should do more things here
		//op = the position of dominant operator in the token expression;
		// 思路：找到op（根据符号优先级,注意括号的判断,此时表达式已不被一个大括号完全包住）
		// 用op将表达式分成两个子表达式,递归求值后，
		// 按op的类型进行运算(switch)
		int op = 0, op_type = 0;
		bool leftBracket = false;//用于判断是否有左括号(即括号），最终目的是为了把表达式内的括号部分排除，以便于查找op
		int currentTokenPriority = 100;//符号的优先级大概有10个，设置一个远大于10的数字用于比较并不断改变优先级
		int i;
		for(i = p; i<=q; i++){
			if(tokens[i].str[0] == ')'){
				leftBracket = false;
				continue;
			}	
			if(leftBracket){
				continue;
			}
			if(tokens[i].str[0] == '(' ){
				leftBracket = true;
				continue;
			}
			switch(tokens[i].type){
                case TK_OR:
					if(currentTokenPriority>1){currentTokenPriority=1;op=i;op_type=TK_OR;continue;}
                case TK_AND:
					if(currentTokenPriority>2){currentTokenPriority=2;op=i;op_type=TK_AND;continue;}
                case TK_NEQ:
					if(currentTokenPriority>3){currentTokenPriority=3;op=i;op_type=TK_NEQ;continue;}
                case TK_EQ:
					if(currentTokenPriority>3){currentTokenPriority=3;op=i;op_type=TK_EQ;continue;}
                case TK_LOE:
					if(currentTokenPriority>4){currentTokenPriority=4;op=i;op_type=TK_LOE;continue;}
                case TK_BOE:
					if(currentTokenPriority>4){currentTokenPriority=4;op=i;op_type=TK_BOE;continue;}
                case '<':
					if(currentTokenPriority>4){currentTokenPriority=4;op=i;op_type='<';continue;}
                case '>':
					if(currentTokenPriority>4){currentTokenPriority=4;op=i;op_type='>';continue;}
                case TK_RS:
					if(currentTokenPriority>5){currentTokenPriority=5;op=i;op_type=TK_RS;continue;}
                case TK_LS:
					if(currentTokenPriority>5){currentTokenPriority=5;op=i;op_type=TK_LS;continue;}
                case '+':
					if(currentTokenPriority>6){currentTokenPriority=6;op=i;op_type='+';continue;}
                case '-':
					if(currentTokenPriority>6){currentTokenPriority=6;op=i;op_type='-';continue;}
                case '*':
					if(currentTokenPriority>7){currentTokenPriority=7;op=i;op_type='*';continue;}
                case '/':
					if(currentTokenPriority>7){currentTokenPriority=7;op=i;op_type='/';continue;}
                case '!':
					if(currentTokenPriority>8){currentTokenPriority=8;op=i;op_type='!';continue;}
                case TK_NEG:
					if(currentTokenPriority>9){currentTokenPriority=9;op=i;op_type=TK_NEG;continue;}
                case TK_POI:
					if(currentTokenPriority>9){currentTokenPriority=9;op=i;op_type=TK_POI;continue;}
                default:continue;
            }
		}
		//分成子串，进行计算
		uint32_t val1 = eval(p, op - 1);
		uint32_t val2 = eval(op +1, q);
		switch(op_type){
			case TK_OR:return val1||val2;
            case TK_AND:return val1&&val2;
            case TK_NEQ:return val1!=val2;
            case TK_EQ:return val1==val2;
            case TK_LOE:return val1<=val2;
            case TK_BOE:return val1>=val2;
            case '<':return val1<val2;
            case '>':return val1>val2;
            case TK_RS:return val1>>val2;
            case TK_LS:return val1<<val2;
            case '+':return val1+val2;
            case '-':return val1-val2;
            case '*':return val1*val2;
            case '/':return val1/val2;
            case '!':return !val2;
            case TK_NEG:return -1*val2; 
            case TK_POI:return swaddr_read(val2,4);
            default:assert(0);
		}
	}
}



uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	if(nr_token!=1){  //只有一个符号时没必要区分
		int i;
    	for( i=0;i<nr_token;i++)  //负号的判断 当其为第一个符号，或左边为(时,或按照讲义左边可能为负号(--1)
        {
				if(tokens[i].type == '-' &&(i==0||tokens[i-1].type == '('||tokens[i-1].type == TK_NEG
                                                                 ||tokens[i-1].type == '-'
                                                                 ||tokens[i-1].type == '+'
                                                                 ||tokens[i-1].type == '*'
                                                                 ||tokens[i-1].type == '/'))
				tokens[i].type = TK_NEG;
		}

    	for(i=0;i<nr_token;i++)
        	if(tokens[i].type == '*' &&(i==0||(tokens[i-1].type!=TK_DEC && tokens[i-1].type!=TK_HEX && tokens[i-1].type!=')')))
               tokens[i].type = TK_POI;
	}
  return eval(0, nr_token-1);
}

