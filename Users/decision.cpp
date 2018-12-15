/*************************************************************************
	> File Name: decision.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年11月13日 星期二 19时06分04秒
 ************************************************************************/

#include "decision.h"
using namespace std;

int status=0;

void* decision_thread_func(void *Position)   //子线程：决策函数
{
    band_cpu(0);
    Limbs limbs_demo;
#ifdef TRACK
    double stepSize=0.1;
    double height=0.1;	
    double angle_turn=0.1;
#endif
#ifdef NO_GAIT
    /*停止移动*/
    char str_snd[20] ={'0','0','0','#'};
    double mov_flag=0;
    can_send_msg(0x1C0, mov_flag, str_snd);
#endif
    float *pos;
	while(1){
	    switch(status){
	    case 0:{
#ifdef TRACK
				/*TRACK MOVE*/
	        	stepSize=0.1;
                height=0.1;
	        	angle_turn=0.758;
                limbs_demo.Limbs_track_move(stepSize, height, angle_turn);
#endif
#ifdef CPG
				/*CPG MOVE*/
				double v = 1;
				limbs_demo.Limbs_CPG_move(v);
				cout<<"move"<<endl;
#endif
#ifdef NO_GAIT
				/*NO GAIT*/
				/*停止移动*/
                char str_snd[20] ={'0','0','0','#'};
                double mov_flag=0;
                can_send_msg(0x1C4, mov_flag, str_snd);
#endif
                //把信号量减1
	            //sem_wait(&locat_sem);
	            pos = (float *)Position;
	            if(pos[0]==pos[1]+15151)//判断
	            {
	                status=1;
	            }
	            printf("status%d\r\n",status);
	            break;
	        }
	    case 1:{
#ifdef TRACK
	        	/*TRACK MOVE*/
	        	stepSize=0.1;
                height=0.1;	
	        	angle_turn=0.758;
                limbs_demo.Limbs_track_move(stepSize, height, angle_turn);
#endif
#ifdef CPG
                /*CPG MOVE*/
				double v = 1;
				limbs_demo.Limbs_CPG_move(v);
#endif
#ifdef NO_GAIT
                /*NO GAIT*/
				/*开始移动*/
                char str_snd1[20] ={'0','0','0','#'};
                double mov_flag=1;
                can_send_msg(0x1C4, mov_flag, str_snd1);
#endif

				//把信号量减1
	            sem_wait(&locat_sem);
	            pos =  (float *)Position;
	            if(pos[0]==pos[1])//判断
	            {
	                status=2;
	            }
	            printf("status%d\r\n",status);
	            break;
	        }
	    case 2:{
#ifdef TRACK
				/*TRACK MOVE*/
	        	stepSize=0.1;
                height=0.1;
	        	angle_turn=0.758;
                limbs_demo.Limbs_track_move(stepSize, height, angle_turn);
#endif
#ifdef CPG
				/*CPG MOVE*/
				double v = 1;
				limbs_demo.Limbs_CPG_move(v);
#endif
#ifdef NO_GAIT
               /*NO GAIT*/
				/*改变移动方向*/
                char str_snd2[20] ={'0','0','0','#'};
                double mov_dir=1;
                can_send_msg(0x1C5, mov_dir, str_snd2);
#endif
                //把信号量减1
	            sem_wait(&locat_sem);
	            pos =  (float *)Position;
	            if(pos[0]==pos[1])//判断
	            {
	                status=3;
	            }
	            printf("status%d\r\n",status);
	            break;
	        }
	    case 3:{
	            finish_flag=1;
	            printf("status%d\r\n",status);
	            //退出线程
	            pthread_exit(NULL);
	            break;
	        }
	    }   
    }
   
}
void decision_thread_create(){
    pthread_t thread;
    //创建线程，并把Position作为线程函数的参数 
    if(pthread_create(&thread, NULL, decision_thread_func, Position) != 0) { 
        perror("pthread_create failed\n"); 
        exit(EXIT_FAILURE); 
    }
}
