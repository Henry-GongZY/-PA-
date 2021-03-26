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
  TK_NEG   //- integer   
  

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

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
