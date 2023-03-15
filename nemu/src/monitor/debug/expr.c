#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  // 空格串 TK_NOTYPE
  TK_NOTYPE = 256, TK_EQ,
  /* TODO: Add more token types */
  TK_ADD, TK_SUB, TK_MUL, TK_DIV,
  TK_LP, TK_RP,TK_DEC

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_ADD},      // plus
  {"==", TK_EQ},        // equal
  // my regex and token
  {"-", TK_SUB},      // sub
  {"\\*", TK_MUL},      // mul
  {"/", TK_DIV},        // divide
  {"\\(", TK_LP},       // (
  {"\\)", TK_RP},       // )
  {"[1-9][0-9]*|0", TK_DEC}, // decimal
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
    // 将一个正则表达式编译成一个模式缓冲区（pattern buffer），使得该正则表达式可以被快速匹配
    /*preg是一个指向regex_t结构体的指针，该结构体保存了编译后的正则表达式模式信息。
    regex是一个指向C字符串的指针，表示待编译的正则表达式。
    cflags是编译标志，用于指定编译正则表达式时的选项。
    如果编译成功，该函数返回0*/
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
// 识别出其中的token
static bool make_token(char *e) {
  int position = 0;
  int i;
  // regmatch_t结构体用于存储与正则表达式匹配的子串在目标字符串中的位置信息，包括其起始位置和结束位置
  /*
  typedef struct {
    regoff_t rm_so; // 匹配子串在目标字符串中的起始位置
    regoff_t rm_eo; // 匹配子串在目标字符串中的结束位置
  } regmatch_t;
  */
  regmatch_t pmatch;

  nr_token = 0;
  
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      // 对于输入字符串 e 中，从 position 开始的子串，是否能够使用第 i 个正则表达式进行匹配，并且匹配到的子串是否在输入字符串的起始位置
      // pmatch.rm_so为0表示匹配到的子串在输入字符串的起始位置
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        // 子串 len
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        // token.str lenth is 32 end with \0
        if (substr_len > 31) {
          printf("Error: too long for a token.\n");
          assert(0);
        }
        //清空
        for (int j = 0; j < 32; j++){ 
					tokens[nr_token].str[j] = '\0';
				}
        // 可以匹配了，接下来将识别出的 token 信息记录下来
        switch (rules[i].token_type) {
           case TK_ADD: {
            tokens[nr_token].type = TK_ADD;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye num is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            // Token t;
            // t.type = TK_ADD;
            // t.str[0] = '+';
            // t.str[1] = '\0';
            // tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_SUB: {
            tokens[nr_token].type = TK_SUB;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            // Token t;
            // t.type = TK_SUB;
            // t.str[0] = '-';
            // t.str[1] = '\0';
            // tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_MUL: {
            tokens[nr_token].type = TK_MUL;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            // Token t;
            // t.type = TK_MUL;
            // t.str[0] = '*';
            // t.str[1] = '\0';
            // tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_DIV: {
            tokens[nr_token].type = TK_DIV;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            // Token t;
            // t.type = TK_DIV;
            // t.str[0] = '/';
            // t.str[1] = '\0';
            // tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_LP: {
            tokens[nr_token].type = TK_LP;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            // Token t;
            // t.type = TK_LP;
            // t.str[0] = '(';
            // t.str[1] = '\0';
            // tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_RP: {

            // Token t;
            // t.type = TK_RP;
            // t.str[0] = ')';
            // t.str[1] = '\0';
            // tokens[nr_token] = t;
            tokens[nr_token].type = TK_RP;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            // tokens[nr_token].str[substr_start + substr_len] = '\0';
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token += 1;
            break;
          }
          case TK_DEC: {
            // Token t;
            // t.type = TK_DEC;
            // int j = 0;
            tokens[nr_token].type = TK_DEC;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            // tokens[nr_token].str[ substr_start + substr_len] = '\0';
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            // for (j = 0; j < substr_len; j++) {
            //   t.str[j] = *(substr_start + j);
            // }
            // t.str[j] = '\0';
            // tokens[nr_token] = t;
            nr_token++;
            break;
          }
          case TK_NOTYPE: {
            // printf("@ oh it is blank ignore it");
            break;
          }
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  // printf("that is all, what a happy match trip ^-^~~\n");
  return true;
}

void for_expr_test( char *e){
  if (make_token(e)== true){
    printf("@ from for_expr_test funct make_token success!!!\n");
  }
}
// 判断表达式是否被一对匹配的括号包围着,同时检查表达式的左右括号是否匹配
// p和q 指示这个子表达式的开始位置 结束位置
bool check_parentheses(int p, int q){
	int j = 0, k = 0;
  // (balabala)
	if (tokens[p].type == TK_LP || tokens[q].type == TK_RP){
		for (int i = p; i <= q; i++){
			if (tokens[i].type == TK_LP)
				j++;

			if (tokens[i].type == TK_RP)
				k++;
      // p位置的（有匹配的）但不是q位置的
			if (i != q && j == k)
				return false;			
		}
    // loop end and it is good
		if (j == k)
				return true;			
	}
	return false;
}
/*
// 找出 dominant operator, 将 token 表达式全部扫描一遍
int dominant_operator(int p, int q){
	int step = 0;
	int op = -1;
	int pri = 0;
    
	for (int i = p; i <= q; i++){
    // 出现在一对括号中的 token 不是 dominant operator.注意到这里不会出现有括号包围整个表达式的情况
		if (tokens[i].type == TK_LP)
			step++;
		
		if (tokens[i].type == TK_RP)
			step--;
	
		if (step == 0){
      if (tokens[i].type == TK_ADD || tokens[i].type == TK_SUB){
			  if (pri < 2){
				  op = i;
				  pri = 2;
			  }
		  } 
      else if (tokens[i].type == TK_MUL || tokens[i].type == TK_DIV){
			  if (pri < 1){
				  op = i;
				  pri = 1;
			  }
		  }
      // p位置没有左括号 q位置有右括号 错误
		  else if (step < 0){
			  return -2;
		  }
	  }
	}
	return op;
}
*/
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();
  // *success = true;
  // for (int i = 0; i < nr_token; i++) {
  //   if (tokens[i].type == TK_MIN && (i == 0 || (tokens[i - 1].type != TK_DEC && tokens[i - 1].type  != TK_HEX && tokens[i - 1].type  != TK_REG))) {
  //     tokens[i].type = TK_NEG;
  //   }
  //   else if (tokens[i].type == TK_MUL && (i == 0 || (tokens[i - 1].type != TK_DEC && tokens[i - 1].type  != TK_HEX && tokens[i - 1].type  != TK_REG && tokens[i - 1].type != TK_RP))) {
  //     tokens[i].type = TK_POI;
  //   }
  // }
  // int p = 0, q = nr_token - 1;
  // int value = evaluate(p, q);
  return 0;
}
