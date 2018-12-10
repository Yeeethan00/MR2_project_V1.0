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
    //初始化信号量
    sem_all_init();
    /*GUI线程*/
    gui_thread_create();
    /*CAN发送线程*/
    can_snd_thread_create();
    /*定位线程*/
    loaction_thread_create();
    /*决策线程*/
    decision_thread_create();
    /*CAN接收*/
    can_recv_create();
    //while(1){;}//测试进程
    //main_CPGNetWork();//CPG测试
    //删除信号量
    sem_all_destory();
    return 0;
}

