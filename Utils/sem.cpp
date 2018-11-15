/*************************************************************************
	> File Name: sem.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年11月13日 星期二 19时46分19秒
 ************************************************************************/

#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include "sem.h"
using namespace std;
int finish_flag=0;
sem_t locat_sem;//定位信号量，阻塞决策线程判断位置
void sem_all_init(){
    //初始化信号量，其初值为0 
    if(sem_init(&locat_sem, 0, 0) == -1) { 
        perror("semaphore intitialization failed\n"); 
        exit(EXIT_FAILURE); 
    } 
}
void sem_all_destory(){
    sem_destroy(&locat_sem);
}
