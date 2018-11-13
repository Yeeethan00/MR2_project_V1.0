/*************************************************************************
	> File Name: main.cpp
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018��10��25�� ������ 09ʱ26��26�� 
************************************************************************/
#include "Users/decision.h"
#include "Users/location.h"
extern "C" {
#include "Can/can.h"
}
using namespace std;
using namespace qrk;

int main(int argc, char *argv[])
{
    //��ʼ���ź���
    sem_all_init();
    /*�����߳�*/
    decision_thread_create();
    /*CAN�����߳�*/
    can_snd_thread_create();
    /*��λ�߳�*/
    loaction_thread_create();
    /*CANjie����*/
    can_recv_create();
    
    // kinectv2_get_picture_demo();    //kinect demo
    //�����ź���
    sem_all_destory();
    exit(EXIT_SUCCESS);
    return 0;
}

