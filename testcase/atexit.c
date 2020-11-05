#include <stdio.h>
#include <stdlib.h>
// 声明将在 main 函数结束后执行的函数. 
// 这样的函数前置声明很特别
void fn1(void), fn2(void), fn3(void), fn4(void);

int main(void)
{
    // 注册需要在 main 函数结束后执行的函数.  
    // 请注意它们的注册顺序和执行顺序 
    atexit(fn1);
    atexit(fn2);
    atexit(fn3);
    atexit(fn4);
    
    // 这条输出语句具有参照性，它可不是最后一句输出. 
    puts("This is executed first."); 
    return EXIT_SUCCESS;
}

void fn1(void)
{
   printf("next.\n"); 
}

void fn2(void)
{
    printf("executed "); 
}

void fn3(void)
{
    printf("is "); 
}

void fn4(void)
{
    printf("This ");
} 