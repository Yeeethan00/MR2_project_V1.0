/*************************************************************************
	> File Name: sem.h
	> Author: 
	> Mail: 
	> Created Time: 2018年11月07日 星期三 13时08分30秒
 ************************************************************************/

#ifndef _SEM_H
#define _SEM_H
#include <semaphore.h>
extern double GUI_show_data;
extern double GUI_show_data_1;
extern int finish_flag;
extern float Position[2];
extern sem_t locat_sem;//定位信号量，阻塞决策线程判断位置
void sem_all_init();
void sem_all_destory();
#endif
