#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
enum {
  // 空格串 TK_NOTYPE
  TK_NOTYPE = 256, TK_EQ,
  /* TODO: Add more token types */
  TK_ADD, TK_SUB, TK_MUL, TK_DIV,
  TK_LP, TK_RP, 
  /* -9 负号*/
  TK_MUNIS,
  TK_HEX, TK_DEC, TK_REG,
  TK_LOGOR, TK_LOGAND,
  TK_OR, TK_AND, /*TK_XOR, */TK_NOT,
  TK_NEQ,
  TK_POINTER

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
  {"!=", TK_NEQ},        // not-equal
  // my regex and token
  {"-", TK_SUB},      // sub
  {"\\*", TK_MUL},      // mul
  {"/", TK_DIV},        // divide
  {"\\(", TK_LP},       // (
  {"\\)", TK_RP},       // )

  /*DEC should be behind od HEX*/

  {"0[xX][a-fA-F0-9]+", TK_HEX},//hex
  {"[1-9][0-9]*|0", TK_DEC}, // decimal
  {"\\$[a-zA-Z]+", TK_REG}, // registers

  {"\\|\\|", TK_LOGOR},      // log-or
  {"&&", TK_LOGAND},         // log-and

  {"\\|", TK_OR},        // calc-or
  {"&", TK_AND},         // calc-and
  // {"^", TK_XOR},         // calc -xor
  {"!", TK_NOT},         // log-not

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

// hlep funct for finding munis--------it is not number or ) 
bool hlep_find_munis(int my_type){
  if (my_type == TK_DEC || my_type == TK_RP || TK_HEX || TK_REG)
    return false;
  return true;
}

// priority for token 
// the num is less the priority is lower
int token2priority(Token tk){
  switch(tk.type){
    case TK_LOGOR:{
      return 1;
    }
    case TK_LOGAND:{
      return 2;
    }
    case TK_OR:{
      return 3;
    }
    // case TK_XOR:{
    //   return 4;
    // }
    case TK_AND:{
      return 5;
    }
    case TK_EQ:
    case TK_NEQ:{
      return 6;
    }
    /*may be will add < > <+ >= save 7 for them*/
    case TK_SUB:
    case TK_ADD:{
      return 8;
    }
    case TK_MUL:
    case TK_DIV:{
      return 9;
    }
    case TK_POINTER:
    case TK_MUNIS:
    case TK_NOT:{
      return 10;
    }

    default:{
      // today is 230317
      return 17;
    }
  }
}
// judge whether a token is operator
// help to find pointer as well
bool is_operator(Token my_token){
  // optimized because add some other operators
  // if (my_token.type >= 258 && my_token.type <= 261)
  if(token2priority(my_token) < 11)
    return true;

  return false;
}

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
          printf("@ make_token: Error! too long for a token.\n");
          assert(0);
        }
        //清空
        for (int j = 0; j < 32; j++){ 
					tokens[nr_token].str[j] = '\0';
				}
        // 可以匹配了，接下来将识别出的 token 信息记录下来
        // 
        switch (rules[i].token_type) {
           case TK_ADD: {
            tokens[nr_token].type = TK_ADD;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye num is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token += 1;
            break;
          }
          case TK_SUB: {
            tokens[nr_token].type = TK_SUB;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token += 1;
            break;
          }
          case TK_MUL: {
            tokens[nr_token].type = TK_MUL;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token += 1;
            break;
          }
          case TK_DIV: {
            tokens[nr_token].type = TK_DIV;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token += 1;
            break;
          }
          case TK_LP: {
            tokens[nr_token].type = TK_LP;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token += 1;
            break;
          }
          case TK_RP: {
            tokens[nr_token].type = TK_RP;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            // tokens[nr_token].str[substr_start + substr_len] = '\0';
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token += 1;
            break;
          }
          case TK_DEC: {
            tokens[nr_token].type = TK_DEC;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            // tokens[nr_token].str[ substr_start + substr_len] = '\0';
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_NOTYPE: {
            printf("@ oh it is blank ignore it\n");
            break;
          }
          case TK_EQ:{
            tokens[nr_token].type = TK_EQ;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_NEQ:{
            tokens[nr_token].type = TK_NEQ;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_HEX:{
            tokens[nr_token].type = TK_HEX;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_REG:{
            tokens[nr_token].type = TK_REG;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_LOGOR:{
            tokens[nr_token].type = TK_LOGOR;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_LOGAND:{
            tokens[nr_token].type = TK_LOGAND;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_OR:{
            tokens[nr_token].type = TK_OR;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          // case TK_XOR:{
          //   tokens[nr_token].type = TK_XOR;
          //   strncpy(tokens[nr_token].str, substr_start, substr_len);
          //   printf("@ match ^ its tpye is %d\n",/* tokens[nr_token].str, */tokens[nr_token].type);
          //   nr_token++;
          //   break;
          // }
          case TK_AND:{
            tokens[nr_token].type = TK_AND;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          case TK_NOT:{
            tokens[nr_token].type = TK_NOT;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("@ match %s its tpye is %d\n", tokens[nr_token].str, tokens[nr_token].type);
            nr_token++;
            break;
          }
          default: {
            // TODO();
            printf("@ make_token: ERROR cannot match the substring.\n");
            assert(0);
          }
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }

  }

    // find munis 
    // if - in tokens[0] or there is no number or ) just ahead of - for example 7- (-8+9), 9--8
    for (int k = 0; k < nr_token; k++){
      if (k == 0 && tokens[k].type == TK_SUB)
        tokens[k].type = TK_MUNIS;
      else if (k != 0 && tokens[k].type == TK_SUB && hlep_find_munis(tokens[k-1].type)){
        tokens[k].type = TK_MUNIS;
      }
      // * pointer
      if (k == 0 && tokens[k].type == TK_MUL)
        tokens[k].type = TK_POINTER;
      else if(k != 0 && tokens[k].type == TK_MUL && hlep_find_munis(tokens[k-1].type))
        tokens[k].type = TK_POINTER;
    }

  // printf("that is all, what a happy match trip ^-^~~\n");
  printf("@ for test lex, all the macth result in tokens is ");
  for (int k = 0; k < nr_token; k++)
    printf("%s ", tokens[k].str);
  printf("\n");
  return true;
}


// 判断表达式是否被一对匹配的括号包围着,同时检查表达式的左右括号是否匹配
// p和q 指示这个子表达式的开始位置 结束位置
bool check_parentheses(int p, int q){
	int j = 0, k = 0;
  // (balabala)
	if (tokens[p].type == TK_LP && tokens[q].type == TK_RP){
		for (int i = p; i <= q; i++){
			if (tokens[i].type == TK_LP)
				j++;

			if (tokens[i].type == TK_RP)
				k++;
      // p位置的（有匹配的）但不是q位置的
      // or not exist ( in position p
			if (i != q && j == k ){
        if (j == 0){
          printf("@ check_parentheses: NOT exist ( in position p\n");
        
          return false;
        }
        printf("@ check_parentheses: EXPR ERROR! ( in positon p does not match ) in position q. now i is %d\n then will enter next part [dominant operator]\n", i);
       
        return false;
        }			
		}
    // loop end and it is good
		if (j == k){
      printf("@ check_parentheses: loop end and check success!\n");
			return true;
    }			
    else{
      printf("@ check_parentheses: EXPR ERROR! the number of ( != the number of ) \n");
      assert(0);
    }
  }
	return false;
}





// find dominant operator, scan all the tokens
int dominant_operator(int p, int q){

  // dominant_operator position 
  int domt_op_idx = -1;
  int domt_op_type = -1;
  int last_priority = 99;
  int LP_num = 0;
	for (int i = p; i <= q; i++){
    // meet (
    if (tokens[i].type == TK_LP){
      LP_num += 1;
      continue;
    }
    // meet )
    else if (tokens[i].type == TK_RP){
      LP_num -= 1;
      // num of ( is less than num of )  ERROR
      if (LP_num < 0){
         printf("@ dominant_operator: EXPR ERROR! num of ( is less than num of )\n");
         assert(0);
      }
      continue;
    }
    // the token in () is not dominant_operator
    if (LP_num > 0){
      continue;
    }
    // move this LP_num < 0 part to front
    // // num of ( is less than num of )  ERROR
    // else if (LP_num < 0){
    //   // if there is a ) in position q and it happens that LP_num < 0
    //   // won't exec this because the funct continued before
    //   printf("@ dominant_operator: EXPR ERROR! num of ( is less than num of )\n");
    // }
    else {
      if(is_operator(tokens[i])){
        /*
        int tmp_type = domt_op_type;
        int tmp_idx = domt_op_idx;
        domt_op_type = tokens[i].type;
        domt_op_idx = i;
        // if (tmp_type == -1){
        //   domt_op_idx = i;
        // }
        // now meet * / but have met + - so choose + -
        if (domt_op_type == 260 || domt_op_type == 261)// * /
          if (tmp_type == 258 || tmp_type == 259){// + -
            domt_op_type = tmp_type;
            domt_op_idx = tmp_idx;
          }
          */
        // optimized because add some other operators
        int priority_i = token2priority(tokens[i]);
        // now priority on tokens[i] is lower than before or equal
        if (priority_i <= last_priority){
          domt_op_idx = i;
          domt_op_type = tokens[i].type;
          last_priority = priority_i;
        }
      }
    }
    // printf("@ now i = %d\n",i);
	}
  printf("@ dominant_operator: SUCCESS find dominant_operator its type is %d and index of it is %d\n", domt_op_type, domt_op_idx);
	return domt_op_idx;
}
// evaluate
uint32_t eval(int p, int q){
  uint32_t res;
  if (p > q){
    printf("@ eval: ERROR! p > q.\n");
    res = 0;
  }
  else if (p == q){// it is a number 
    // str to unsigned long
    printf("@ eval : now enter p==q part\n");
    switch (tokens[p].type){
      case TK_DEC:
      case TK_HEX:{
        res = strtoul(tokens[p].str,NULL,0);
      }
      case TK_REG:{
        // eip
        if(strcmp(tokens[p].str, "$eip") == 0) 
          res = cpu.eip;
        // eax ecx 
        else{
          for(int i = 0; i < 8; i++)
            if(strcmp(&tokens[p].str[1], reg_name(i, 4)) == 0) 
              res = reg_l(i);
            else if(strcmp(&tokens[p].str[1], reg_name(i, 2)) == 0) 
              res = reg_w(i);
            else if(strcmp(&tokens[p].str[1], reg_name(i, 1)) == 0) 
              res = reg_b(i);
        }
      
      }
      // default:{
      //   assert(0);
      // }
    }
  }
  else if (check_parentheses(p, q)){
    printf("@ eval : now enter check_parentheses(p, q) == true part\n");
    res = eval(p+1, q-1);
  }
  else {
    printf("@ eval : now enter else parttttt\n");
    int op_idx = dominant_operator(p, q);
    if(op_idx == p && tokens[p].type == TK_MUNIS){
      printf("@ eval : - expr!\n");
      res = -1 * eval(p + 1, q);
    }
    else if (op_idx == p && tokens[p].type == TK_NOT){
      printf("@ eval : ! expr!\n");
      res = ! eval(p + 1, q);
    }
    else if (op_idx == p && tokens[p].type == TK_POINTER){
      printf("@ eval : *pointer\n");
      res = vaddr_read(eval(p + 1, q), 4);
    }
    /*
    // there is munis like -expr
    if (op_idx == -1 && tokens[p].type == TK_MUNIS){
      printf("@ eval : - expr!\n");
      res = -1 * eval(p + 1, q);
    }
    // ! expr
    else if (op_idx == -1 && tokens[p].type == TK_NOT){
      printf("@ eval : ! expr!\n");
      res = ! eval(p + 1, q);
    }
    */
    else{
    uint32_t val1 = eval(p, op_idx - 1);
    uint32_t val2 = eval(op_idx + 1, q);
    int op_type = tokens[op_idx].type;
    switch (op_type){
      case TK_ADD:{
        res = val1 + val2;
        break;
      }
      case TK_SUB:{
        res = val1 - val2;
        break;
      }
      case TK_MUL:{
        res = val1 * val2;
        break;
      }
      case TK_DIV:{
        if (val2 == 0){
          printf("@ eval : ERROR divide 0 now the p = %d and q = %d\n", p, q);
          assert(0);
        }
        res = val1 / val2;
        break;
      }
      case TK_LOGAND:{
        res = val1 && val2;
        break;
      }
      case TK_LOGOR:{
        res = val1 || val2;
        break;
      }
      case TK_EQ:{
        res = (val1 == val2);
        break;
      }
      case TK_NEQ:{
        res = (val1 != val2);
        break;
      }
      case TK_AND:{
        res = val1 & val2;
        break;
      }
      case TK_OR:{
        res = val1 | val2;
        break;
      }
      // case TK_XOR:{
      //   res = val1 ^ val2;
      //   break;
      // }
      default:
        // assert(0);
        res = 0;
    }
    }

  }
  printf("@ eval : now finish an eval part [p = %d, q = %d] the result is %d\n", p, q, res);
  return res;
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  *success = true;
  return eval(0, nr_token - 1);
}

void for_expr_test( char *e){

  // check 词法分析
  if (make_token(e)== true){
    printf("@ from for_expr_test funct make_token success!!!\n");
  }
 
  
  // check check_parentheses
  // if (check_parentheses(0, nr_token-1)== true){
  //   printf("@ from for_expr_test funct check_parentheses success!!!\n");
  // }
  // // check dominant_operator
  // if (dominant_operator(0, nr_token-1)== true){
  //   printf("@ from for_expr_test funct dominant_operator success!!!\n");
  // }
  // // check eval
  uint32_t result = eval(0, nr_token - 1);
  printf("@ from for_expr_test funct eval success!!! the final result is %d\n",result);
}