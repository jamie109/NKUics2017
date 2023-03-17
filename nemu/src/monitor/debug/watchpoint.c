#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

static int wp_free_num = 32;

WP* new_wp(){
  if(wp_free_num){
    printf("@ there is no free wp.\n");
    return NULL;
  }
  WP* tmp = free_;
  printf("@ oh give you a wp its no is %d\n", tmp->NO);
  tmp->next = head;
  head = tmp;
  free_ = free_->next;
  wp_free_num --;

  return tmp;
}

void free_wp(WP* wp){
  wp_free_num ++;

}
