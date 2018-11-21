/*************************************************************************
	> File Name: limbs.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年10月31日 星期三 10时48分12秒
 ************************************************************************/

#include "limbs.h"
#define MR2_L 0.6
#define MR2_W 0.3

#define L1 0.25
#define L2 0.25
#define HIP_HEIGHT 0.35
using namespace std;

typedef union{
    int in[2];
    double dou;
}int2_to_double;

void can_double2char(double dou, char *ch)
 {
 	int2_to_double un;		
 	un.dou=dou;
	printf("%x\r\n",un.in[0]);printf("%x\r\n",un.in[1]);
 	for(int i=1;i<3;i++)
 	{
 		for(int j=1;j<9;j++)
 		{
 			int temp=((un.in[i-1]>>(64-4*j))&0x0000000f);
 			if(temp<10)	ch[8*(i-1)+(j-1)+4]=temp+48;
			else	ch[8*(i-1)+(j-1)+4]=temp+87;
			cout<<8*(i-1)+(j-1)+4<<":"<<ch[8*(i-1)+(j-1)+4]<<' ';
		}
	}
	
 }

Limbs::Limbs()
{
    this->leg_LF.leg_type=LF;
    this->leg_LB.leg_type=LB;
    this->leg_RF.leg_type=RF;
    this->leg_RB.leg_type=RB;
}

void Limbs::track2states(Leg_states *leg)
{
    leg->track_z1=sqrt(leg->track_y*leg->track_y+leg->track_z*leg->track_z);

    leg->angle_knee=acos((L1*L1+L2*L2-leg->track_x*leg->track_x-leg->track_z1*leg->track_z1)/(2*L1*L2));
    leg->angle_hip=PI/2-asin(L2*sin(leg->angle_knee)/sqrt(leg->track_x*leg->track_x+leg->track_z1*leg->track_z1))+atan(leg->track_x/leg->track_z1);
    leg->angle_hip_side=atan(leg->track_y/leg->track_z);
}

void Limbs::leg_control(Leg_states *leg, double tim ,double stepSize, double height, double angle_alpha, int leg_dir)
{
    double period=LOOP_NUM/LOOP_NUM;
    double time=tim/LOOP_NUM+1/LOOP_NUM;
    //double R=stepSize/(2*sin(angle_alpha*0.5)*sin(0.5*(PI-angle_alpha))+1e-121);
    double R=stepSize/(cos(0.5*(PI-angle_alpha))+1e-121);
    double Lx=0, Ly=0;
    if((leg->leg_type==RF)||(leg->leg_type==RB))
    {
        Lx=2*sin(0.5*angle_alpha)*sqrt(pow((R*leg_dir-0.5*MR2_L),2)+0.25*MR2_W*MR2_W)*sin(0.5*PI-0.5*angle_alpha-leg_dir*atan(MR2_W/(leg_dir*2*R-MR2_L)));
        Ly=2*sin(0.5*angle_alpha)*sqrt(pow((R*leg_dir-0.5*MR2_L),2)+0.25*MR2_W*MR2_W)*cos(0.5*PI-0.5*angle_alpha-leg_dir*atan(MR2_W/(leg_dir*2*R-MR2_L)))*leg_dir;
    }   
    else
    {
        Lx=2*sin(0.5*angle_alpha)*sqrt(pow((R*leg_dir+0.5*MR2_L),2)+0.25*MR2_W*MR2_W)*sin(0.5*PI-0.5*angle_alpha-leg_dir*atan(MR2_W/(leg_dir*2*R+MR2_L)));
        Ly=2*sin(0.5*angle_alpha)*sqrt(pow((R*leg_dir+0.5*MR2_L),2)+0.25*MR2_W*MR2_W)*cos(0.5*PI-0.5*angle_alpha-leg_dir*atan(MR2_W/(leg_dir*2*R+MR2_L)))*leg_dir;
    }
    //printf("%lf   %lf \n",Lx,Ly);
    if((leg->leg_type==LF)||(leg->leg_type==RB)) 
    {
        leg->track_x=Lx*(tim<LOOP_NUM/2?(2*time/period-(0.5/PI)*sin(4*PI*time/period)):(2*(period-time)/period-(0.5/PI)*sin(4*PI*(period-time)/period)));
        leg->track_y=Ly*leg->track_x/(Lx+1e-123);
        leg->track_z=(tim<LOOP_NUM/2?(height*(0.5-0.5*cos(4*PI*time/period))):0)-HIP_HEIGHT;
    }
    else
    {
        leg->track_x=Lx*(tim<LOOP_NUM/2?(2*(0.5*period-time)/period-(0.5/PI)*sin(4*PI*(0.5*period-time)/period)):(2*(time-0.5*period)/period-(0.5/PI)*sin(4*PI*(time-0.5*period)/period)));
        leg->track_y=Ly*leg->track_x/(Lx+1e-123);
        leg->track_z=(tim<LOOP_NUM/2?0:height*(0.5-0.5*cos(4*PI*(time-0.5*period)/period)))-HIP_HEIGHT;
    }
    track2states(leg);

    /*执行*/
    //char *msg = (char *)"123#1122334455667788";

    if(leg->leg_type==LF)
    {
        printf("x: %lf, y %lf , z:%lf\n", leg->track_x,leg->track_y,leg->track_z);
        printf("leg_type:%d  angle:%lf  %lf  %lf \n",leg->leg_type,leg->angle_hip_side,leg->angle_hip,leg->angle_knee);

    }
    if(leg->leg_type==RF)
    {
	GUI_show_data=(double)leg->angle_hip;
  	GUI_show_data_1=(double)leg->angle_knee;
	char msg_hip_side[20] ={'0','0','3','#'};
        can_double2char((double)leg->angle_hip_side, msg_hip_side);
        handle(msg_hip_side);
        char msg_hip[20] ={'0','0','2','#'};
        can_double2char((double)leg->angle_hip, msg_hip);
        handle(msg_hip);
        char msg_knee[20] ={'0','0','1','#'};
        can_double2char((double)leg->angle_knee, msg_knee);
        handle(msg_knee);
        printf("x: %lf, y %lf , z:%lf\n", leg->track_x,leg->track_y,leg->track_z);
        printf("leg_type:%d  angle:%lf  %lf  %lf \n",leg->leg_type,leg->angle_hip_side,leg->angle_hip,leg->angle_knee);

    }
    if(leg->leg_type==LB)
    {
        printf("x: %lf, y %lf , z:%lf\n", leg->track_x,leg->track_y,leg->track_z);
        printf("leg_type:%d  angle:%lf  %lf  %lf \n",leg->leg_type,leg->angle_hip_side,leg->angle_hip,leg->angle_knee);

    }
    if(leg->leg_type==LB)
    {
        printf("x: %lf, y %lf , z:%lf\n", leg->track_x,leg->track_y,leg->track_z);
        printf("leg_type:%d  angle:%lf  %lf  %lf \n",leg->leg_type,leg->angle_hip_side,leg->angle_hip,leg->angle_knee);
    }
    usleep(1000);
}

void Limbs::Limbs_move(double stepSize, double height, double angle_turn)
{
    int leg_dir;
    double angle_t;
    if(angle_turn>=0)//左转
    {
        angle_t=angle_turn+0.0000001;
        leg_dir=1;
    }
    else//右转
    {
        angle_t=-angle_turn-0.0000001;
        leg_dir=-1;
    }
    for(double i=0; i<LOOP_NUM; i++)
    {
        leg_control(&this->leg_LF, i, stepSize, height,angle_t,leg_dir);
        leg_control(&this->leg_LB, i, stepSize, height,angle_t,leg_dir);
        leg_control(&this->leg_RF, i, stepSize, height,angle_t,leg_dir);
        leg_control(&this->leg_RB, i, stepSize, height,angle_t,leg_dir);
    }
}

