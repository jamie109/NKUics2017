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
  printf("# free_ update now its no is %d\n", free_->NO);
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

void free_wp(WP* wp){
	if (wp == NULL){
    printf("# ERROR you free a null\n");
		assert(0);
	}
	if (wp == head)
		head = head->next;
	else {
		WP* tmp = head;
    // can head
    while(tmp){
      if(tmp->next == wp)
        tmp->next = tmp->next->next;
    }
	}
	wp->next = free_;
	free_ = wp;
	free_->expr_val = 0;
  for(int k = 0; k <32; k++){
    free_->expr_str[k] = '\0';
  }

}

bool watch_wps(){
  bool is_changed = false, success = true;
  WP* tmp = head;
  // scan all the wps on head
  while(tmp){
    uint32_t cur_res = expr(tmp->expr_str, &success);
    if(cur_res != expr(tmp->expr_str, &success)){
      is_changed = true;
      printf("# NOTICE the [wp.no = %d]s value changed.[old_value = %d], [new_value = %d]\n", tmp->NO, cur_res, expr(tmp->expr_str, &success));
      // continue;
    }
    tmp = tmp->next;
  }
  return is_changed;
}

void print_wps_info(){
  WP* tmp = head;
  if (tmp == NULL){
    printf("# there is no wp\n");
    return;
  }
  else{
    while(tmp){
      printf("wp.no = %d\texpr: %s\t value:%d\t\n", tmp->NO, tmp->expr_str, tmp->expr_val);
		  tmp = tmp->next;
    }
  }
}

void del_wp_no(int no){
  WP* tmp = head;
  bool find = false;
  WP* wp_targ = NULL;
  // del head
  if (tmp->NO == no){
    find = true;
    wp_targ = tmp;
		tmp = tmp->next;
  }

  while(tmp){
    if(tmp->next->NO == no){
      find = true;
      wp_targ = tmp->next;
      tmp->next = tmp->next->next;
    }
  }
  // donot find the wp you want to del
  if(find == false){
    printf("# ERROR you free a null\n");
		assert(0);
  }
  wp_targ->next = free_;
  free_ = wp_targ;
  free_->expr_val = 0;
  for(int k = 0; k <32; k++){
    free_->expr_str[k] = '\0';
  }
}
