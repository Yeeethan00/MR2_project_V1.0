/*************************************************************************
	> File Name: location.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年11月13日 星期二 19时05分39秒
 ************************************************************************/

#include "location.h"
using namespace std;
using namespace qrk;

void* loaction_thread_func(void *arg)   //子线程：定位函数
{
    band_cpu(2);
    char *a[2];
    a[1]=(char *)"-e";
    Urg_func urg_demo;
    urg_demo.init_change(2, a);
    while(!finish_flag)
    {        

        urg_demo.get_position(2, a);//printf("get_position\r\n");
        //把信号量加1
        sem_post(&locat_sem);
    }
}
void loaction_thread_create()
{
   pthread_t thread;
    //创建线程，并把NULL作为线程函数的参数 
    if(pthread_create(&thread, NULL, loaction_thread_func, NULL) != 0) { 
        perror("pthread_create failed\n"); 
        exit(EXIT_FAILURE); 
    }
}

