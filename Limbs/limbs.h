/*************************************************************************
	> File Name: limbs.h
	> Author: 
	> Mail: 
	> Created Time: 2018年10月31日 星期三 10时48分06秒
 ************************************************************************/

#ifndef _LIMBS_Hb
#include<iostream>
#include<stdio.h>
#include<math.h>
#include "../Utils/sem.h"
extern "C" {
#include "../CPGNetWork/CPG_gait.h"
#include "../Can/can.h"
}

#define PI 3.1415926535
#define LOOP_NUM 20
typedef enum Leg_type{
    LF=0,
    RF,
    RB,
    LB
}Leg_type;
typedef struct Leg_states{
    Leg_type leg_type;

    double angle_hip_side;
    double angle_hip;
    double angle_knee;

    double track_x;
    double track_y;
    double track_z;

    double track_z1;
}Leg_states;

class Limbs
{
    private:
        Leg_states leg_LF, leg_LB, leg_RF, leg_RB;
        

        void track2states(Leg_states *leg);
        void leg_control(Leg_states *leg, double tim ,double stepSize, double height, double angle_alpha, int leg_dir);
    public:
        Limbs();
        void Limbs_track_move(double stepSize, double height, double angle_turn);
        void Limbs_CPG_move(double v);
        
};

#endif
