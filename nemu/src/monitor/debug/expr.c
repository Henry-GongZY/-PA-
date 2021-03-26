#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, 
  TK_EQ,  //==

  /* TODO: Add more token types */
  TK_LB,  //'('
  TK_RB,  //')'
  TK_NEQ, //!=
  TK_HEX, //0x
  TK_DEC, //decimal
  TK_REG_32, 
  TK_REG_16, 
  TK_REG_8, //3 types of register 
  TK_AND,   //&&
  TK_OR,   //||
  TK_NOT,  //!
  TK_NEG,   //- integer   
  TK_DEREF

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  
  {" +", TK_NOTYPE},    // spaces
  {"\\!=", TK_NEQ},     // not equal
  {"==", TK_EQ},        // equal
  {"\\(",TK_LB},          // lb
  {"\\)",TK_RB},          // rb
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"\\/", '/'},         // divide
  {"0[x,X][0-9a-fA-F]+",TK_HEX},    //hex
  {"[0-9]+", TK_DEC},   //decimal
  {"&&",TK_AND},        //lg_and
  {"\\|\\|",TK_OR},     //lg_or
  {"!",TK_NOT},         //lg_not
  {"(\\$eax|\\$ecx|\\$edx|\\$ebx|\\$esp|\\$ebp|\\$esi|\\$edi|\\$eip)",TK_REG_32},
  {"(\\$ax|\\$cx|\\$dx|\\$bx|\\$sp|\\$bp|\\$si|\\$di)",TK_REG_16},
  {"(\\$al|\\$cl|\\$dl|\\$bl|\\$ah|\\$ch|\\$dh|\\$bh)",TK_REG_8} //registers
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

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

//change expression into token
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
				case TK_NOTYPE:
						break;
				case '+':
						tokens[nr_token].type = '+';
                        nr_token++;
						break;
				case '-':
						tokens[nr_token].type = '-';
                        nr_token++;
						break;
				case '*':
					        tokens[nr_token].type = '*';
                        nr_token++;
				   	        break;
			case '/':
						tokens[nr_token].type = '/';
                        nr_token++;
						break;
                        case TK_EQ:
						tokens[nr_token].type = TK_EQ;
                        nr_token++;						
						break;
				case TK_NEQ:
						tokens[nr_token].type = TK_NEQ;
                        nr_token++;
						break;
				case TK_LB:
						tokens[nr_token].type = TK_LB;
                        nr_token++;
						break;
				case TK_RB:
						tokens[nr_token].type = TK_RB;
                        nr_token++;
						break;
				case TK_DEC:
						tokens[nr_token].type = TK_DEC;	

						memcpy(tokens[nr_token].str, substr_start, substr_len); 
						nr_token++;
						break;
				case TK_HEX:
						tokens[nr_token].type = TK_HEX;
						memcpy(tokens[nr_token].str, substr_start, substr_len);
						nr_token++;
						break;
				case TK_AND:
						tokens[nr_token].type = TK_AND;
                        nr_token++;
						break;
				case TK_OR:
						tokens[nr_token].type = TK_OR;
                        nr_token++;
						break;
				case TK_NOT:
						tokens[nr_token].type = TK_NOT;
                        nr_token++;
						break;
				case TK_REG_32:
						tokens[nr_token].type = TK_REG_32;
						memcpy(tokens[nr_token].str, substr_start, substr_len);
						nr_token++;
						break;
				case TK_REG_16:
						tokens[nr_token].type = TK_REG_16;
						memcpy(tokens[nr_token].str, substr_start, substr_len);
						nr_token++;
						break;
				case TK_REG_8:
						tokens[nr_token].type = TK_REG_8;
						memcpy(tokens[nr_token].str, substr_start, substr_len);
						nr_token++;
						break;

		  		default: break;                                                                                                                                                             break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

//transfer register into direct index
int getreg(char *str, int type) {
	if(type == TK_REG_32) {
		if(strcmp(str,"$eax")==0)
				return 0;
		else if(strcmp(str,"$ecx")==0)
				return 1;
		
		else if(strcmp(str,"$edx")==0)
				return 2;
	
		else if(strcmp(str,"$ebx")==0)
				return 3;

		else if(strcmp(str,"$esp")==0)
				return 4;

		else if(strcmp(str,"$ebp")==0)
				return 5;

		else if(strcmp(str,"$esi")==0)
				return 6;
	
		else if(strcmp(str,"$edi")==0)
				return 7;
		else {
			printf("Not a reg(len32)!\n");
			assert(0);
		}	
	}
	else if(type == TK_REG_16) {
	
		if(strcmp(str,"$ax")==0)
				return 0;
		else if(strcmp(str,"$cx")==0)
				return 1;
		
		else if(strcmp(str,"$dx")==0)
				return 2;
	
		else if(strcmp(str,"$bx")==0)
				return 3;

		else if(strcmp(str,"$sp")==0)
				return 4;

		else if(strcmp(str,"$bp")==0)
				return 5;

		else if(strcmp(str,"$si")==0)
				return 6;
	
		else if(strcmp(str,"$di")==0)
				return 7;

		else {
			printf("Not a reg(len16)!\n");
			assert(0);
		}	
	}
	else if(type == TK_REG_8) {
		
		if(strcmp(str,"$al")==0)
				return 0;
		else if(strcmp(str,"$cl")==0)
				return 1;
		
		else if(strcmp(str,"$dl")==0)
				return 2;
	
		else if(strcmp(str,"$bl")==0)
				return 3;

		else if(strcmp(str,"$ah")==0)
				return 4;

		else if(strcmp(str,"$ch")==0)
				return 5;

		else if(strcmp(str,"$dh")==0)
				return 6;
	
		else if(strcmp(str,"$bh")==0)
				return 7;

		else {
			printf("Not a reg(len8)!\n");
			assert(0);
		}	
	}
	else{
		printf("Reg error!\n");
		assert(0);
	}	
}

//check brace
//TODO:Remains to be seen
bool check_parentheses(int p,int q,bool *success){
	if(tokens[p].type != TK_LB || tokens[q].type != TK_RB)
		return false;
	int flag=0, i;
	for(i=p;i<=q;i++) {
		if(tokens[i].type == TK_LB)
				flag++;
		else if(tokens[i].type == TK_RB)
				flag--;
	}	
	if(flag!=0){
			*success = false;
			return false;
	}
	return true;
}

static struct Prior{
	int type;
	int priority;
}table[]={
  //op   //level
	{TK_NEG,2},
	{TK_NOT,2},
	{'/',3},
	{'*',3},
	{'+',4},
	{'-',4},
	{TK_NEQ,6},
	{TK_EQ,6},
	{TK_AND,10},
	{TK_OR,11},
};

int priorop(int p, int q) {
	int i;
  int index = p;
  int priority = 0;

	bool isInParen = false;
	for(i=p;i<=q;i++) {
		switch(tokens[i].type) {
				case TK_LB:
						isInParen = true;
						break;
				case TK_RB:
						isInParen = false;
						break;
				case '+':{
						 	if(isInParen)
									break;
							else{
								if(priority<=4)
								{
										priority = 4;
										index = i;
								}
								break;
							}
						 }
				case '-':{
						 	if(isInParen)
									break;
							else{
								if(priority<=4)
								{
									priority = 4;
									index = i;	
								}	
								break;
							}
						 }
				case '*':{
						 	if(isInParen)
									break;
							else{
								if(priority<=3)
								{
									index = i;
									priority = 3;	
								}
								break;	
							}
						 }
				case '/':{
						 	if(isInParen)
									break;
							else{
								if(priority<=3)
								{
										index = i;
										priority = 3;
								}
								break;
							}
						 }
				case TK_NOT:{
								if(isInParen)
										break;
								if(priority<2)
								{
										index = i;
										priority = 2;
								}
								break;
							}
				case TK_NEG:{
								if(isInParen)
										break;
								if(priority<2)
								{
										index = i;
										priority = 2;
								}
								break;
							}
				case TK_EQ:{
						   		if(isInParen)
										break;
								if(priority<=7)
								{
										index = i;
										priority = 7;
								}
								break;
						   }
				case TK_NEQ:{
								if(isInParen)
										break;
								if(priority<=7)
								{
										index = i;
										priority = 7;
								}
								break;
							}
				case TK_AND:{
									if(isInParen)
											break;
									if(priority<=11)
									{
											index = i;
											priority = 11;
									}
									break;
							}
                                case TK_DEREF:{
								if(isInParen)
										break;
								if(priority<2)
								{
										index = i;
										priority = 2;
								}
								break;
							  }
				case TK_OR:{
								   if(isInParen)
										   break;
								   if(priority<=12)
								   {
										   index = i;
										   priority = 12;
								   }
								   break;
						   }
				default:
						 break;
		}	
	}
	return index;
}

bool check_type(int type) {
	if(type!=TK_REG_32&&type!=TK_REG_16&&type!=TK_REG_8&&type!=TK_HEX&&type!=TK_DEC&&type!=TK_RB)
			return true;
	else
			return false;
}

uint32_t eval(int p, int q, bool* success) {
	bool matched = true;
	int a;
	if(p > q){
		*success = false;
		return 0;
	}
	else if(p == q){
		switch(tokens[p].type){
				case TK_DEC:
						return atoi(tokens[p].str);
				case TK_HEX:
						sscanf(tokens[p].str,"%x",&a);
						return a;
				case TK_REG_32:
						if(strcmp(tokens[p].str, "$eip")==0)
								return cpu.eip;
						else
							return reg_l(getreg(tokens[p].str,TK_REG_32));
				case TK_REG_16:	
						return reg_w(getreg(tokens[p].str,TK_REG_16));
				case TK_REG_8:
						return reg_b(getreg(tokens[p].str,TK_REG_8));
				default:
						*success = false;
						return 0;
		}	
	}
	else if(check_parentheses(p,q,&matched) == true) {
		uint32_t result =  eval(p+1,q-1,success);
		if(*success)
				return result;
		else
				return 0;
	}
	else{
		if(!matched){
			*success = false;
			return 0;
		}
		int op = priorop(p,q);
		
		uint32_t val2 = eval(op+1,q,success);
		if(!*success) {
			return 0;
		}

		switch(tokens[op].type) {
			
			case TK_NOT:return !val2;
			case TK_NEG:return -val2;
			case TK_DEREF:return vaddr_read(val2,4);
			default:break;
		}


		uint32_t val1 = eval(p,op-1,success);
		if(!*success) {
			return 0;
		}


		switch(tokens[op].type) {
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':{
						if(val2 == 0){
								*success = false;
								return 0;
						} else{
								return val1/val2;
						}
					 }
			case TK_EQ: return (val1==val2);
			case TK_NEQ: return (val1!=val2);
			case TK_AND: return (val1&&val2);
			case TK_OR: return (val1||val2);	 
			default:assert(0);
		}
	}
			
}

int32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  //printf_token();
  //return 0;
  
  /* TODO: Insert codes to evaluate the expression. */
 
  int i;
  for(i=0;i<nr_token;i++) {
  	if(tokens[i].type=='*'||tokens[i].type=='-'){
		if(i==0||check_type(tokens[i-1].type))
		{
				if(tokens[i].type=='*')
						tokens[i].type=TK_DEREF;
				else
						tokens[i].type=TK_NEG;
		}
	}
  }

bool evalSuccess = true;
uint32_t result = eval(0, nr_token-1,&evalSuccess);

for(i=0;i<nr_token;i++) {
		memset(tokens[i].str,0,strlen(tokens[i].str));
}

if(!evalSuccess) {
	  *success = false;
	  return 0;
} else{
	    *success = true;
	    return result;
}
}
