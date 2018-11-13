#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "can.h"

Cthread_sendworkqueue * sndqueue;   //ȫ�ַ��Ͷ��нṹ��
char * interface = (char *)"can0";          //ָ���豸

//char **recv_set;
//recv_set[0] = "candump";

void handle(char * mse_in)
{
    snd_add_task(mse_in,sndqueue);  //�յ���Ϣ ���з���
}

void menu()
{
    char msg_in[50] = "0";
    int first_flag = 1;
    while(1)
    {
        if(1==first_flag)
        {
            printf("int put the canbus meg!\n");
            first_flag = 0;
        }
        printf(">>>");
        scanf("%s",msg_in);
        #ifdef DEBUG
        //printf("in put mes is:%s\n",msg_in);
        #endif // DEBUG
        handle(msg_in);
    }
}

void canrecv_thread()
{
    //canrecv(recvset);
}

int can_snd_thread_create()
{
    sndqueue = (Cthread_sendworkqueue *)sendworkqueue_init();   //ʵ����һ���������У�
    snd_thread_create(sndqueue);                                //���������߳�
    return 0;
} 
int can_recv_create()
{
    char* recvset[2];
    recvset[0] = "candump";
    recvset[1] = "can0";
    canrecv(recvset);
    return 0;
}
/*
int main(int argc,char **argv)
{
    char* recvset[2];
    recvset[0] = "candump";
    recvset[1] = "can0";
    if(argc >= 2)
    {
        interface = argv[1];
        recvset[1]  = argv[1];
        printf("setting interface is %s\n",argv[1]);
    }
    sndqueue = (Cthread_sendworkqueue *)sendworkqueue_init();   //ʵ����һ���������У�
    snd_thread_create(sndqueue);                                //���������߳�

//    pthread_t canrecv_thread;                                   //���������߳�
//    pthread_create(&canrecv_thread,NULL,(void*(*)(void*))canrecv_thread,NULL);

    pthread_t menu_thread;                                                  //�˵��̵߳Ĵ���
    pthread_create(&menu_thread,NULL,(void*(*)(void*))menu,NULL);
    #ifdef RCV
    canrecv(recvset);
    #endif // debug


//    pthread_join(canrecv_thread,NULL);
    pthread_join(menu_thread,NULL);

    return 0;
}

*/









