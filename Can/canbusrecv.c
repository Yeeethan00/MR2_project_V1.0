
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
#include <sys/epoll.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "can.h"


int epfd; //epoll���Ƶ�������

struct epoll_event can_epoll_event; //�������ü����¼��Ľṹ��


/*epoll�����س�ʼ��
����ֵ��epoll�� ������
��������Ҫ�������ļ�������
*/
int epoll_init(int fd)
{
    int epfd = epoll_create(0);//����������
    can_epoll_event.events = EPOLLIN|EPOLLET; //�����¼�Ϊ �� �Ƿ�ɶ� ������ʽ����Ե����
    can_epoll_event.data.fd = fd;//�������ļ�������
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&can_epoll_event);
    return epfd;
}

int canrecv_init(char *interface)
{
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);//���� SocketCAN �׽���
    strcpy(ifr.ifr_name, interface );
    ioctl(s, SIOCGIFINDEX, &ifr);//ָ�� can0 �豸
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr)); //���׽����� can0 ��
}

