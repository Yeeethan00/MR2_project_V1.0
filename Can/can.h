#ifndef _CAN_H_
#define	_CAN_H_
#include "canbussend.h"
//#include "canbusrecv.h"
#include "candump.h"
#define DEBUG 0
#define RCV 1

//extern Cthread_sendworkqueue * sndqueue;   //全局发送队列结构体
extern Cthread_sendworkqueue *sndqueue;
extern char * interface ;
//char * interface = (char *)"can0";          //指定设备
//struct epoll_event *events;         //用来进行监听的结构体数组

void handle(char * mse_in);
int can_snd_thread_create();
int can_recv_create();

#endif // _CAN_H_
