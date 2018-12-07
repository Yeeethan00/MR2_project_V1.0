#ifndef _CAN_H_
#define	_CAN_H_
#include <assert.h>
#include "canbussend.h"
//#include "canbusrecv.h"
#include "candump.h"
#include "../Utils/sem.h"
//#include "can_func.h"
#define DEBUG 0
#define RCV 1

//extern Cthread_sendworkqueue * sndqueue;   //全局发送队列结构体
extern Cthread_sendworkqueue *sndqueue;
extern char * interface ;
//char * interface = (char *)"can0";          //指定设备
//struct epoll_event *events;         //用来进行监听的结构体数组
typedef union{
    char ch[8];
    int in[2];
    float fl[2];
    double df;
}can_msg;
typedef struct CanRxMsg{
    int stdId;
    char data[8];
}CanRxMsg;

typedef struct CanTxMsg{
    int stdId;
    char data[8];
}CanTxMsg;

typedef struct canlist{
    void (*func)(CanRxMsg* pRxMsg);
    uint32_t ID;
}CanList;

extern can_msg Can_Send_msg;
void handle(char * mse_in);
int can_snd_thread_create();
int can_recv_create();

void can_double2char(double dou, char *ch);
void can_rcv_char(char* buf, CanRxMsg *Can_Rx_Msg);
void can_recv_callback(CanRxMsg *Can_Rx_Msg);
void can_add_func(void);
int can_send_msg(int ID, double df_snd);

int callback(CanRxMsg *pRxMsg);
#endif // _CAN_H_
