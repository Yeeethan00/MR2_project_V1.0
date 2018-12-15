/*
 * File: CPGNetWork.h
 *
 * MATLAB Coder version            : 3.1
 * C/C++ source code generated on  : 10-Dec-2018 17:27:26
 */

#ifndef CPGNETWORK_H
#define CPGNETWORK_H

/* Include Files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "CPGNetWork_types.h"


/*Define*/
#define PI 3.1415926535
#define ACC_ROLL_THRESHOLD 0.909   //侧向冲击加速度阈值
#define LENHGTH 0.64 //体长
#define WIDTH 0.48  //体宽
#define L 0.3 //腿长
/*typedef*/
typedef enum {
    phi_elbow=1,
    phi_knee=-1
}joint_type;

/*global */
extern double theta0;   //腿与垂直方向（立足面）夹角，原平衡位置
extern double beta;     //负载因子，表示支撑相所占比例
extern float phiRB, phiLB;      //RB LB相位, 与beta一同决定步态
extern float alpha;     //控制振荡器收敛到极限环得到速度
extern float a;         //决定omiga在omiga_sw和omiga_st之间的变化速度
extern double joint_angle[16];  //关节角输出
extern double osc_x[16];        //振荡器理论输出 （开环）
extern double osc_x_feedback[16];       //振荡器实际反馈  (CAN接收  闭环)

/* Function Declarations */
void CPGNetWork(double acc_roll, double slope_alpha, double v, double T, double tfinal, double phi);

#endif

/*
 * File trailer for CPGNetWork.h
 *
 * [EOF]
 */
