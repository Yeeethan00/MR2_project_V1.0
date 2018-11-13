/*************************************************************************
	> File Name: main.cpp
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018��10��25�� ������ 09ʱ26��26�� 
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

void* decision_thread_func(void *Position)   //���̣߳����ߺ���
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
                //���ź�����1
	            sem_wait(&locat_sem);
	            pos = (float *)Position;
	            if(pos[0]==pos[1])//�ж�
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
                //���ź�����1
	            sem_wait(&locat_sem);
	            pos =  (float *)Position;
	            if(pos[0]==pos[1])//�ж�
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
                //���ź�����1
	            sem_wait(&locat_sem);
	            pos =  (float *)Position;
	            if(pos[0]==pos[1])//�ж�
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
	            //�˳��߳�
	            pthread_exit(NULL);
	            break;
	        }
	    }   
    }
   
}
void decision_thread_create(){
    pthread_t thread;
    //�����̣߳�����Position��Ϊ�̺߳����Ĳ��� 
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
        //���ź�����1
        sem_post(&locat_sem);
    }
}

int main(int argc, char *argv[])
{
    //��ʼ���ź���
    sem_all_init();
    /*�����߳�*/
    decision_thread_create();
    /*CAN�����߳�*/
    can_snd_thread_create();
    /*��λ�߳�*/
    loaction_thread_create(argc, argv);
    //can_recv_create();
    
    // kinectv2_get_picture_demo();    //kinect demo
    //�����ź���
    sem_all_destory();
    exit(EXIT_SUCCESS);
    return 0;
}

