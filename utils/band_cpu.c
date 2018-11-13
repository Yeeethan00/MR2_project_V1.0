/*************************************************************************
	> File Name: band_cpu.c
	> Author: 
	> Mail: 
	> Created Time: 2018年11月05日 星期一 19时54分36秒
 ************************************************************************/
#define _GNU_SOURCE

#include "band_cpu.h"

void band_cpu(int cpu_num)
{
    cpu_set_t mask;
    
    CPU_ZERO(&mask);
    /*将0、1、2、3添加到集合中*/
    CPU_SET(cpu_num, &mask);  

    /* 设置cpu 亲和性(affinity)*/
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        fprintf(stderr, "set thread affinity failed\n");
    }     
}
