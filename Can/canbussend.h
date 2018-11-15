#ifndef _CANBUSSEND_H_
#define	_CANBUSSEND_H_

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "../Utils/band_cpu.h"

int canbussend(void *arg);



/*����*/
typedef struct sendtask
{

    int (*canbussend)(void *arg);     //��������ִ�е�����
    void * arg;                        //�������в���
    struct sendtask *next;          //��һ������
} Cthread_sendtask;

/*��������*/
typedef struct sendworkqueue
{
    pthread_mutex_t sendqueue_lock; //������
    pthread_cond_t  sendqueue_ready;//
    pthread_t *sndthreadid;         //�����߳�id

    /*����ṹ���̳߳������еȴ�����*/
    Cthread_sendtask *sendqueue_head;

    /*�Ƿ������̳߳�*/
    int sendshutdown;
    // pthread_t *threadid;

    /*��ǰ�ȴ���������*/
    int cur_sendtask_size;

} Cthread_sendworkqueue;

int snd_add_task (void *arg,Cthread_sendworkqueue * sndqueue);
int snd_thread_create(Cthread_sendworkqueue * sndqueue);
Cthread_sendworkqueue * sendworkqueue_init();


#endif // _CANBUSSEND_H_


