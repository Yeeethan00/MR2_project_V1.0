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

Limbs::Limbs()
{
    this->leg_LF.leg_type=LF;
    this->leg_LB.leg_type=LB;
    this->leg_RF.leg_type=RF;
    this->leg_RB.leg_type=RB;
}
/*TRACK MOVE*/
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
    if(leg->leg_type==LF)
    {
        printf("x: %lf, y %lf , z:%lf\n", leg->track_x,leg->track_y,leg->track_z);
        printf("leg_type:%d  angle:%lf  %lf  %lf \n",leg->leg_type,leg->angle_hip_side,leg->angle_hip,leg->angle_knee);

    }
    if(leg->leg_type==RF)
    {
	    GUI_show_data=leg->angle_hip;
        char str_snd[20] ={'0','0','0','#'};char str_snd1[20] ={'0','0','0','#'};char str_snd2[20] ={'0','0','0','#'};
        can_send_msg(0x003, leg->angle_hip_side, str_snd);
        can_send_msg(0x004, leg->angle_hip, str_snd1);
        can_send_msg(0x005, leg->angle_knee, str_snd2);
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
    usleep(50000);
}
void Limbs::Limbs_track_move(double stepSize, double height, double angle_turn)
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


/*CPG MOVE*/
void Limbs::Limbs_CPG_move(double v){
    //步态参数
    double T = 0.2;                        //运动周期 m
    double tfinal = 0.001;                         //响应时间域（Time region of response）
    for(int i = 1; i<T/tfinal; i++) {

        CPGNetWork_gait(v, T, tfinal);
        /*legs*/
        //LF
        this->leg_LF.angle_hip = joint_angle[0];
        this->leg_LF.angle_knee = joint_angle[1];
        this->leg_LF.angle_hip_side = joint_angle[8];
        //RF
        this->leg_RF.angle_hip = joint_angle[2];
        this->leg_RF.angle_knee = joint_angle[3];
        this->leg_RF.angle_hip_side = joint_angle[10];
        //RB
        this->leg_RB.angle_hip = joint_angle[4];
        this->leg_RB.angle_knee = joint_angle[5];
        this->leg_RB.angle_hip_side = joint_angle[12];
        //LB
        this->leg_LB.angle_hip = joint_angle[6];
        this->leg_LB.angle_knee = joint_angle[7];
        this->leg_LB.angle_hip_side = joint_angle[14];

        //show

        GUI_show_data = this->leg_LF.angle_hip;
        GUI_show_data_1 = this->leg_LF.angle_knee;
    }
}

