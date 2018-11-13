/*************************************************************************
	> File Name: sem.h
	> Author: 
	> Mail: 
	> Created Time: 2018年11月07日 星期三 13时08分30秒
 ************************************************************************/

#ifndef _SEM_H
#define _SEM_H
#include <semaphore.h>
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
#endif
