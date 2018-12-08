#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "can.h"

Cthread_sendworkqueue * sndqueue;   //ȫ�ַ��Ͷ��нṹ��
char * interface = (char *)"can0";          //ָ���豸

static int canlistnum = 0;
CanList canList[50];//最多能加50个can链接，可以改
can_msg Can_Send_msg;
// char **recv_set;
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
    can_add_func();
    char* recvset[2];
    recvset[0] = "candump";
    recvset[1] = "can0";
    canrecv(recvset);
    return 0;
}

void can_double2char(double dou, char *ch)
{
    can_msg un;
    un.df=dou;
    for(int i=1;i<3;i++)
    {
        for(int j=1;j<9;j=j+2)
        {
            int temp=((un.in[2-i]>>(64-4*j))&0x0000000f);
            if(temp<10)	ch[27-8*i-j]=temp+48;
            else	ch[27-8*i-j]=temp+87;
            int temp_1=((un.in[2-i]>>(64-4*(j+1)))&0x0000000f);
            if(temp_1<10)	ch[28-8*i-j]=temp_1+48;
            else	ch[28-8*i-j]=temp_1+87;
        }
    }
}
/* 返回ch字符在sign数组中的序号 */
int getIndexOfSigns(char ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if(ch >= 'A' && ch <='F')
    {
        return ch - 'A' + 10;
    }
    if(ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    return -1;
}
/* 十六进制数转换为十进制数 */
long hexToDec(char *source,int len)
{
    long sum = 0;
    long t = 1;
    int i;
    for(i=len-1; i>=0; i--)
    {
        sum += t * getIndexOfSigns(*(source + i));
        t *= 16;
    }

    return sum;
}
void can_rcv_char(char* buf, CanRxMsg *Can_Rx_Msg)
{
    //char buf[]={'0','0','1',' ',' ',' ','[','8',']',' ',' ','0','0',' ','0','0',' ','0','0',' ','0','0',' ','0','0',' ','0','0',' ','2','8',' ','4','0'};

    char stdid[3]={buf[0],buf[1],buf[2]};
    int len_id=(int)(sizeof(stdid)/sizeof(char));
    Can_Rx_Msg->stdId=hexToDec(stdid, len_id);
    //printf("IDshi%d\n",Can_Rx_Msg->stdId);
    char hex[8][2];
    for(int cnt=0; cnt<8; cnt++)
    {
        hex[cnt][0]=buf[11+3*cnt];hex[cnt][1]=buf[12+3*cnt];
        int len=(int)(sizeof(hex[cnt])/sizeof(char));
        //printf("16进制数：\t%c%c\n", hex[cnt][0],hex[cnt][1]);
        //printf("%d 10进制数：\t%ld\n",cnt, hexToDec(hex[cnt], len));
        Can_Rx_Msg->data[cnt]=hexToDec(hex[cnt], len);
    }
}

int can_add_callback(uint32_t ID, void (*func)(CanRxMsg* pRxMsg))
{
  assert(ID <= 2048);
  assert(canlistnum < 50);
  canList[canlistnum].func = func;
  canList[canlistnum].ID = ID;
  canlistnum++;
  return 0;
}
//消息ID+处理函数
void can_add_func(void)
{
    can_add_callback(0X04,callback);
}
int CAN_LIST_MATCH(int ID, CanRxMsg* pRxMsg) {
    for (int i = 0; i < canlistnum; i++) {
        if (ID == canList[i].ID) {
            (*canList[i].func)(pRxMsg);//执行相应的函数
            return 1;
        }
    }
}
void can_recv_callback(CanRxMsg *pRxMsg)
{
    CAN_LIST_MATCH(pRxMsg->stdId, pRxMsg);
}
int can_send_msg(int ID, double df_snd, char* str_snd)
{

    str_snd[2]=ID&0x0000000f;
    str_snd[1]=(ID&0x000000f0)>>4;
    str_snd[0]=(ID&0x00000f00)>>8;
    if(str_snd[0]<10)str_snd[0]+=48;
    else str_snd[0]+=87;
    if(str_snd[1]<10)str_snd[1]+=48;
    else str_snd[1]+=87;
    if(str_snd[2]<10)str_snd[2]+=48;
    else str_snd[1]+=87;
    can_double2char(df_snd, str_snd);
    for(int i=0; i<20; i++)
        printf("%c", str_snd[i]);
    handle(str_snd);
}

int callback(CanRxMsg *pRxMsg)
{
    can_msg Can_msg;
    int i;
    for(i=0;i<8;i++)
    {
        Can_msg.ch[i]=pRxMsg->data[i];
    }
    GUI_show_data_1=Can_msg.df;

    return 1;
}