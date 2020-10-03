#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#include<stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_watchpoint(){
	if(head == NULL){
        head = free_;
        free_=free_->next;
        head->next = NULL;
        return head;
    }
    else{
        if(free_ == NULL) //没有空闲的了
            assert(0);
        WP* temp = head;
        while(temp->next!=NULL){
            temp = temp->next;
        }
        temp->next = free_;
        free_ = free_->next;
        temp = temp->next;
        temp->next = NULL;
        return temp;
    }
}

void free_watchpoint(int N){
	show_watchpoint();
    WP *p, *q;
	p = head;
	int j = 0;
	while(p->next && j < N-1){
		p = p->next;
		j++;
	}
	if(!(p->next) || j > N){
		printf("Position error\n");
		assert(0);
	}
		
	q = p->next;
	p->next = q->next;//删除并释放结点
	free(q);
}

void show_watchpoint(){
	printf("Number\t\tValue\t\tAddress\t\tExpression\n");
    WP *temp = head;
    while(temp!=NULL)
    {
        printf("%d\t\t0x%08x\t0x%08x\t%s\n",temp->NO, temp->value, temp->address,temp->expression);
        temp = temp->next;
    }
}

bool check_watchpoint(){

	return true;
}

