#include <stdio.h>
int m, n, sum;
int e[110][110];    //邻接矩阵
int book[110];      //记录是否访问这个点,可以用它避免形成环
int dis[110];       //已访问的点到未坊问的点的最小距离(实现方式：for + if（< ）更新)
int inf = 99999999; //用于求最小值

void Prim()
{
        int k, min; //k为即将（刚刚）加入的点的位置；min为即将（刚刚）加入的边的长度

        //初始化，即加入了第一个点
        int i,j;
        for (i = 1; i <= n; i++)
        {
                dis[i] = e[1][i];
                book[i] = 0;
        }
        dis[1] = 0;
        book[1] = 1;

        for ( i = 1; i < n; i++)
        {
                //加入新点,所以上面的循环没有等于n
                min = inf;
                for ( j = 1; j <= n; j++) //找到(更新)已访问的点到未坊问的点的最小距离
                        if (book[j] == 0 && dis[j] < min)
                        {
                                min = dis[j];
                                k = j;
                        }
                sum += min;  //加入新边
                book[k] = 1; //加入新结点

                //更新已访问的点到未访问的点的距离
                for (j = 1; j <= n; j++)
                        if (book[j] == 0 && dis[j] > e[k][j]) //dis[j]已为已访问的点到该点的距离中的最小值
                                dis[j] = e[k][j];
        }
}

int main()
{
        int i, j;
        while (scanf("%d", &n) != EOF)
        {
                sum = 0;
                for (i = 1; i <= n; i++)
                        for (j = 1; j <= n; j++)
                                scanf("%d", &e[i][j]);
                Prim();
   	 	nemu_assert(sum != 0);
    		HIT_GOOD_TRAP;
        }
        return 0;
}
