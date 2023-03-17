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
    // add
    wp_pool[i].expr_val = 0;

  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP* new_wp(){
  if(free_ == NULL){
		printf("# there is no free wp.\n");
		return NULL;
	}
  WP* tmp = free_;
  printf("# give you a wp its no is %d\n", tmp->NO);
  // free_ lose a wp
  // del it there because now free_ and tmp point to the same addr ,if I make tmp->next = NULL,then free_->next is also NULL
  // tmp->next = NULL;
  free_ = free_->next;
  tmp->next = NULL;
  printf("# free_ update now its no is %d", free_->NO);
  // head(tmp)---->NULL
  if(head == NULL)
    head = tmp;
  // tmp--->head......---->NULL
  else{
    tmp->next = head;
    head = tmp;
  }
  printf("# now the head no is %d\n", head->NO);
  return tmp;
}

// void free_wp(WP* wp){
//   wp_free_num ++;

// }
