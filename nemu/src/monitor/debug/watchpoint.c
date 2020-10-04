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
        if(free_ == NULL) //静态链表中没有空闲的位置了
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

void free_watchpoint(int watchpointNO){
	assert(watchpointNO >= 0);
	 WP *toDeletePoint = head;
	 WP *previousToDeletePoint = head;
	 //找到结点位置必定需要N
	 //而watchpointNO==0时，无法先求previous（watchpointNO-1）,再求delete。
	 //但watchpointNO==0是特殊位置可以并到后面的指针变动中
	 //当watchpointNO>0时，先求previous（watchpointNO-1）,再求delete
	 if(watchpointNO > 0){
		//找到被删结点的前一个结点
		while(previousToDeletePoint != NULL){
        	if(previousToDeletePoint->NO == watchpointNO-1){
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
    if(head == toDeletePoint)//N == 0的情况
        head = head->next;
    else{
		previousToDeletePoint->next = toDeletePoint->next;
    }
    toDeletePoint->next = free_;
    free_ = toDeletePoint;
    printf("Free watchpoint %d\n", watchpointNO);

}

void show_watchpoint(){
	
    WP *temp = head;
	if(temp != NULL)
		printf("Number\t\tValue\t\tAddress\t\tExpression\n");
	else
		printf("There is no watchpoints\n");
	
    while(temp!=NULL)
    {
        printf("%d\t\t0x%08x\t0x%08x\t%s\n",temp->NO, temp->value, temp->address,temp->expression);
        temp = temp->next;
    }
}

bool check_watchpoint(){
	//有监视点变动，则返回true
	WP *temp = head;
    bool change = false;
    while(temp!=NULL)
    {	//检查每一个监视点

        bool *success = false;
        uint32_t newAddress = expr(temp->expression,success); 
        if(newAddress != temp->address)
        {
            printf("Watchpoint %d: %s\n",temp->NO,temp->expression);
            printf("Old value = 0x%08x\nNew value = 0x%08x\n",temp->address,newAddress);
            temp->address = newAddress;//赋新值
            change = true; 
        }
        temp = temp->next;
    }
    if(change)
        return true;
    return false;
}