/*************************************************************************
	> File Name: main.cpp
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018年10月25日 星期四 09时26分26秒 
************************************************************************/
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include "Hokuyo_urg/Urg_func.h"
#include "KinectV2/KinectV2_func.h"
#include "Limbs/limbs.h"
#include "utils/sem.h"
extern "C" {
#include "Can/can.h"
#include "utils/band_cpu.h"
}
using namespace std;
using namespace qrk;

int status=0;
int finish_flag=0;

void* decision_thread_func(void *Position)   //子线程：决策函数
{
    band_cpu(0);
    
    Limbs limbs_demo;
    double stepSize=0.1;
    double height=0.1;	
    double angle_turn=0.1;
    float *pos;
	while(1){
	    switch(status)
	    {
	    case 0:
	        {  
	        stepSize=0.1;
                height=0.1;	
	        angle_turn=0.0;
                limbs_demo.Limbs_move(stepSize, height, angle_turn);
                //把信号量减1
	            sem_wait(&locat_sem);
	            pos = (float *)Position;
	            if(pos[0]==pos[1])//判断
	            {
	                status=1;
	            }
	            printf("status%d\r\n",status);
	            break;
	        }
	    case 1:
	        {
	        stepSize=0.1;
                height=0.1;	
	        angle_turn=0.758;
                limbs_demo.Limbs_move(stepSize, height, angle_turn);
                //把信号量减1
	            sem_wait(&locat_sem);
	            pos =  (float *)Position;
	            if(pos[0]==pos[1])//判断
	            {
	                status=2;
	            }
	            printf("status%d\r\n",status);
	            break;
	        }
	    case 2:
	        {
	        stepSize=0.1;
                height=0.1;	
	        angle_turn=0.758;
                limbs_demo.Limbs_move(stepSize, height, angle_turn);
                //把信号量减1
	            sem_wait(&locat_sem);
	            pos =  (float *)Position;
	            if(pos[0]==pos[1])//判断
	            {
	                status=3;
	            }
	            printf("status%d\r\n",status);
	            break;
	        }
	    case 3:
	        {
	            finish_flag=1;
	            printf("status%d\r\n",status);
	            //退出线程
	            pthread_exit(NULL);
	            break;
	        }
	    }   
    }
   
}
void decision_thread_create(){
    pthread_t thread;
    //创建线程，并把Position作为线程函数的参数 
    if(pthread_create(&thread, NULL, decision_thread_func, Position) != 0) { 
        perror("pthread_create failed\n"); 
        exit(EXIT_FAILURE); 
    }
}
void loaction_thread_create(int argc, char *argv[])
{
    band_cpu(2);
    Urg_func urg_demo;
    while(!finish_flag)
    {        
	char *a[2];
	a[1]=(char *)"-e";
        urg_demo.get_position(2, a);//printf("get_position\r\n");
        //把信号量加1
        sem_post(&locat_sem);
    }
}

int main(int argc, char *argv[])
{
    //初始化信号量
    sem_all_init();
    /*决策线程*/
    decision_thread_create();
    /*CAN发送线程*/
    can_snd_thread_create();
    /*定位线程*/
    loaction_thread_create(argc, argv);
    //can_recv_create();
    
    // kinectv2_get_picture_demo();    //kinect demo
    //清理信号量
    sem_all_destory();
    exit(EXIT_SUCCESS);
    return 0;
}

