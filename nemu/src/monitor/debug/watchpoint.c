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
        if(free_ == NULL) //没有空闲的位置了
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
	assert(N>=0);
	 WP *toDeletePoint = head;
	 WP *previousToDeletePoint = head;
	 //找到结点位置必定需要N，
	 //而N==0时，无法先求previous（N-1）,再求delete。
	 //故先求delete再用它求previous(->next==delete)
	 
	 if(N == 0){

	 }
	 else{
		//找到被删结点的前一个结点
		while(previousToDeletePoint != NULL){
        	if(previousToDeletePoint->NO == N-1){
            	break;
			}
        	previousToDeletePoint = previousToDeletePoint->next;
    	}
		//找到需要删除的结点
		if(previousToDeletePoint->next != NULL)
			toDeletePoint = previousToDeletePoint->next;
		else
		{
			printf("Wrong position!\n");
			return ;
		}
	 }
    if(head == toDeletePoint)
        head = head->next;
    else
    {
		previousToDeletePoint->next = toDeletePoint->next;
    }
    toDeletePoint->next = free_;
    free_ = toDeletePoint;
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

