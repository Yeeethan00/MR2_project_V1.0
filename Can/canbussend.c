
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
#include <stdlib.h>
#include <sched.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "lib.h"
#include "can.h"
#include "canbussend.h"
/*
extern Cthread_sendworkqueue *sndqueue;
extern char * interface ;
*/
int canbus_send(char *interface,char *snd_msg)
{
    #ifdef DEBUG
    printf("***msg is :%s\n",snd_msg);
    printf("***interface is :%s\n",interface);
    #endif // DEBUG


	int s;                       /* can raw socket */
	int required_mtu;           //�������ݵ����������
	int mtu;
	int enable_canfd = 1;       //�����ر�flagλ
	struct sockaddr_can addr;   //can���ߵĵ�ַ ͬsocket�������� socketaddr�ṹ�� ��������can�������Ϣ
	struct canfd_frame frame;   //Ҫ���͵�buffer
	struct ifreq ifr;           //�ӿ�����ṹ��

	/* check command line options  //��������в����ǲ���3��
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <device> <can_frame>.\n", argv[0]);
		return 1;
	}
    */

	/* parse CAN frame */
	required_mtu = parse_canframe(snd_msg, &frame);//����Ҫ���͵�can�����Ƿ���ϱ�׼
	if (!required_mtu){
		fprintf(stderr, "\nWrong CAN-frame format! Try:\n\n");
		fprintf(stderr, "    <can_id>#{R|data}          for CAN 2.0 frames\n");
		fprintf(stderr, "    <can_id>##<flags>{data}    for CAN FD frames\n\n");
		fprintf(stderr, "<can_id> can have 3 (SFF) or 8 (EFF) hex chars\n");
		fprintf(stderr, "{data} has 0..8 (0..64 CAN FD) ASCII hex-values (optionally");
		fprintf(stderr, " separated by '.')\n");
		fprintf(stderr, "<flags> a single ASCII Hex value (0 .. F) which defines");
		fprintf(stderr, " canfd_frame.flags\n\n");
		fprintf(stderr, "e.g. 5A1#11.2233.44556677.88 / 123#DEADBEEF / 5AA# / ");
		fprintf(stderr, "123##1 / 213##311\n     1F334455#1122334455667788 / 123#R ");
		fprintf(stderr, "for remote transmission request.\n\n");
		return 1;
	}

    /* ����socket�׽���
    PF_CAN Ϊ��λ ͬ�������е�AF_INET ��ipv4Э��
    SOCK_RAWʹ�õ�Э������ SOCK_RAW��ʾԭʼ�׽��� ����ͷ���Լ�����
    CAN_RAWΪʹ�õľ���Э�� Ϊcan����Э��
    */
	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		return 1;
	}

	/*�ӿ�����ṹ�����׽���ioctl�����нӿ�
    ioctl��һ�������Ķ��忪ʼifr_name��
    ���ಿ�ֿ������ض��ڽӿڵġ�*/
	strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);   //���������ṹ�帳������
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';              //�ַ�������λ
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name); //ָ������ӿ������ַ�����Ϊ���������ýӿڴ��ڣ��򷵻���Ӧ�����������򷵻�0 ���Ǽ���ýӿ��Ƿ���ڣ�
	if (!ifr.ifr_ifindex) {
		perror("if_nametoindex");
		return 1;
	}

	addr.can_family = AF_CAN;                       //Э������
	addr.can_ifindex = ifr.ifr_ifindex;             //can��������ľ������� ���� ip��ַ

	if (required_mtu > CAN_MTU) {                   //Maximum Transmission Unit����䵥Ԫ������

		/* check if the frame fits into the CAN netdevice */
		if (ioctl(s, SIOCGIFMTU, &ifr) < 0) {       //��ȡcan�������䵥Ԫ
			perror("SIOCGIFMTU");
			return 1;
		}
		mtu = ifr.ifr_mtu;

		if (mtu != CANFD_MTU) {                     //��canbus����䵥Ԫ������ �ͱ���
			printf("CAN interface is not CAN FD capable - sorry.\n");
			return 1;
		}

		/* interface is ok - try to switch the socket into CAN FD mode */
		if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,       //�����������͡�����״̬�׽ӿڵ�����ѡ��ֵ
			       &enable_canfd, sizeof(enable_canfd))){
			printf("error when enabling CAN FD support\n");
			return 1;
		}

		/* ensure discrete CAN FD length values 0..8, 12, 16, 20, 24, 32, 64 */
		frame.len = can_dlc2len(can_len2dlc(frame.len));
	}

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0); //���ù���

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) { //���׽��� ���� �׽��ֺ�canbus������а�
		perror("bind");
		return 1;
	}

	/* send frame */
	if (write(s, &frame, required_mtu) != required_mtu) {       //�������ݣ�
		perror("write");
		return 1;
	}

	//close(s);                                                   //�ر��׽���

	return 0;
}

/*canbus���ͳ�ʼ��
����ֵ���������нṹ��ָ��
*/
Cthread_sendworkqueue * sendworkqueue_init()
{

    Cthread_sendworkqueue *sendqueue = (Cthread_sendworkqueue *) malloc (sizeof (Cthread_sendworkqueue)); //Ϊ�������з����ڴ�
    pthread_mutex_init (&(sendqueue->sendqueue_lock), NULL);             //��ʼ��������
    pthread_cond_init (&(sendqueue->sendqueue_ready), NULL);             //��ʼ����������
    sendqueue->sendqueue_head = NULL;                                    //��ʼ����������ͷ
    sendqueue->cur_sendtask_size = 0;
    sendqueue->sendshutdown=0;
    sendqueue->sndthreadid =(pthread_t *) malloc (sizeof (pthread_t));   //�����߳�id

    return sendqueue;
}

/*�������ݷ�������
������Ҫ���͵�����
*/
int snd_add_task (void *arg,Cthread_sendworkqueue * sndqueue)
{
    Cthread_sendtask *new_task = (Cthread_sendtask *) malloc (sizeof (Cthread_sendtask));//����һ��������
    //new_task->canbussend = canbussend;      //ִ�е�����
    new_task->arg = arg;                    //����
    new_task->next = NULL;

    pthread_mutex_lock (&(sndqueue->sendqueue_lock));
    /*��������뵽�ȴ�������*/
    Cthread_sendtask *member = sndqueue->sendqueue_head;
    if (member != NULL)
    {
        while (member->next != NULL)        //whileѭ����������ĩβ
            member = member->next;
        member->next = new_task;
    }
    else
    {
        sndqueue->sendqueue_head = new_task;
    }

    sndqueue->cur_sendtask_size++;
    pthread_mutex_unlock (&(sndqueue->sendqueue_lock)); //������������++

    pthread_cond_signal (&(sndqueue->sendqueue_ready)); //֪ͨ�߳�ok

    return 0;
}

void * snd_thread(void *arg)
{
    band_cpu(3);
    #ifdef DEBUG
    printf ("***SEND PTHREAD:starting thread 0x%x\n", (unsigned int)pthread_self ());
    #endif // DEBUG
    //pthread_mutex_lock (&(sndqueue->sendqueue_lock));
    while (1)
    {
        pthread_mutex_lock (&(sndqueue->sendqueue_lock));

        while (sndqueue->cur_sendtask_size == 0 && !sndqueue->sendshutdown) //���û������͵ȴ�
        {
            #ifdef DEBUG
            printf ("***SEND PTHREAD 0x%x is waiting\n", (unsigned int)pthread_self ());
            #endif // DEBUG


            pthread_cond_wait (&(sndqueue->sendqueue_ready), &(sndqueue->sendqueue_lock)); //�ȴ���
        }

        /*�̳߳�Ҫ������*/
        if (sndqueue->sendshutdown)
        {
            pthread_mutex_unlock (&(sndqueue->sendqueue_lock));
            printf ("***SEND PTHREAD 0x%x  will exit\n", (unsigned int)pthread_self ());
            pthread_exit (NULL);
        }
        #ifdef DEBUG
        printf ("***SEND PTHREAD 0x%x  is starting to work\n", (unsigned int)pthread_self ());
        #endif // DEBUG

        //��ʼ��������
        sndqueue->cur_sendtask_size--;
        Cthread_sendtask *task = sndqueue->sendqueue_head;
        sndqueue->sendqueue_head = task->next;

        pthread_mutex_unlock (&(sndqueue->sendqueue_lock));

        //���ûص�������ִ������
        canbus_send(interface,task->arg);

        free (task);
        task = NULL;
    }
    /*��һ��Ӧ���ǲ��ɴ��*/
    pthread_exit (NULL);

}

//���������߳�
int snd_thread_create(Cthread_sendworkqueue * sndqueue)
{
   pthread_create(sndqueue->sndthreadid, NULL, snd_thread, NULL);
   return 0;
}













