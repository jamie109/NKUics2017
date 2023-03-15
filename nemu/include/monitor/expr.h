#ifndef __EXPR_H__
#define __EXPR_H__

#include "common.h"
void for_expr_test( char *);
// 被声明为static的函数、变量或代码块只在当前文件中可见，不能被其他文件所访问。难怪在ui.c中用不了它/(ㄒoㄒ)/~~
// static bool make_token(char *) ;
uint32_t expr(char *, bool *);

#endif
