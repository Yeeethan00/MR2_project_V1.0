/*************************************************************************
	> File Name: main.cpp
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018��10��25�� ������ 09ʱ26��26�� 
************************************************************************/
#include "Users/decision.h"
#include "Users/location.h"
#include "GUI/gui_main.h"
extern "C" {
#include "Can/can.h"
}
using namespace std;
using namespace qrk;

int main(int argc, char *argv[])
{
    printf("start\r\n");
    //��ʼ���ź���
    sem_all_init();
    /*�����߳�*/
    decision_thread_create();
    /*CAN�����߳�*/
    can_snd_thread_create();
    /*��λ�߳�*/
    loaction_thread_create();
    /*CAN����*/
    can_recv_create();
    gui_main();
    // kinectv2_get_picture_demo();    //kinect demo
    //�����ź���
    sem_all_destory();
    exit(EXIT_SUCCESS);
    return 0;
}

