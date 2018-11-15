/*************************************************************************
	> File Name: main.cpp
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018年10月25日 星期四 09时26分26秒 
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
    /*决策线程*/
    decision_thread_create();
    /*CAN发送线程*/
    can_snd_thread_create();
    /*定位线程*/
    loaction_thread_create();
    /*CAN接收*/
    can_recv_create();
    gui_main();
    // kinectv2_get_picture_demo();    //kinect demo
    //清理信号量
    sem_all_destory();
    exit(EXIT_SUCCESS);
    return 0;
}

