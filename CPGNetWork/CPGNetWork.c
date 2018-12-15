/*
 * File: CPGNetWork.c
 *
 * MATLAB Coder version            : 3.1
 * C/C++ source code generated on  : 10-Dec-2018 17:27:26
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "CPGNetWork.h"
#include "CPGNetWork_emxutil.h"
#include "fprintf.h"
#include "ode45.h"
#include "CPGNetWork_rtwutil.h"
#include <stdio.h>




/*global */
/*步态参数*/
double theta0 = PI/6;  //腿与垂直方向（立足面）夹角，原平衡位置
double beta=0.5;       //负载因子，表示支撑相所占比例
float phiRB=0, phiLB=0.5;       //RB LB相位, 与beta一同决定步态
/*振荡器*/
float alpha=100;           //控制振荡器收敛到极限环得到速度
float a=50;                //决定omiga在omiga_sw和omiga_st之间的变化速度
/*输出 输入*/
double joint_angle[16]; //关节角输出
double osc_x[16]={0.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //振荡器理论输出 （开环）
double osc_x_feedback[16]={0}; //振荡器实际反馈  (CAN接收  闭环)

/* Function Definitions */

/*
* CPGNetWork
* Arguments    : double acc_roll       侧向冲击加速度  attitude.h
*                double slope_alpha    坡度角  attitude.h
*                double v              移动速度
*                double T              移动周期
*                double tfinal         采样时间
*                int phi               关节方式   肘式||膝式
* Return Type  : void
*/
void CPGNetWork(double acc_roll, double slope_alpha, double v, double T,
                double tfinal, double phi)
{
  double omiga_sw;      //摆动相频率 Hz
  double h;             //摆动腿足端高度  m
  emxArray_real_T *x;   //微分方程输出x
  emxArray_real_T *t;   //微分方程输出t （没用）
  double Ak;            //髋关节幅值
  double Ah;            //膝关节幅值
  double delte_theta;   //原平衡位置改变量
  double U[16];         //外部反馈矩阵
  double dv1[2];        //微分方程时域
  double k1;            //躯体姿态角/坡度角
  double k2;            //平衡位置改变量/躯体姿态角
  int i;        //循环计数
  int b_i;
  int i2;
  int j;

  /* 步态参数 */
  omiga_sw = 1.0 / (beta * T);        // 摆动相频率 Hz

  /* 摆动腿足端高度  m */
  if (acc_roll > ACC_ROLL_THRESHOLD) {
    h = acc_roll * sin(atan(WIDTH/LENHGTH)) / (2.0 * (omiga_sw * omiga_sw) * sqrt(1.0
                                                                                  - acc_roll * acc_roll / (4.0 * rt_powd_snf(omiga_sw, 4.0))));
  } else {
    h = 0.02;
  }

  emxInit_real_T(&x, 2);
  emxInit_real_T1(&t, 1);

  Ak = acos((L * cos(theta0) - h) / L) - theta0;      // 髋关节幅值
  Ah = asin(beta * v * T / (4*L / cos(theta0)));      // 膝关节幅值
  //振荡器幅值 Ah=sqrt(miu)

  /* 反馈项参数 */
  /* 前庭反射 */
  k2=2.26;           //平衡位置改变量/躯体姿态角________需要完善
  k1=0.24;           //躯体姿态角/坡度角_________________需要完善
  delte_theta = phi * k2 * k1 * slope_alpha;    // 原平衡位置改变量
  /* 外部反馈(feed) 对应x1 y1 x2 y2 x3 y3 x4 y4 */
  U[0] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[1] = delte_theta;
  U[2] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[3] = delte_theta;
  U[4] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[5] = delte_theta;
  U[6] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[7] = delte_theta;
  memset(&U[8], 0, sizeof(double) << 3);

  /********************** 求解 *******************************/
  /* 响应时间域（Time region of response） */
  dv1[0] = 0.0;
  dv1[1] = tfinal;
  //以osc_x_feedback||osc_x作为振荡器初值
  /* 求解Hopf振荡器微分方程的数值解⣨Numerical solution of differential equation */
  ode45(dv1, osc_x_feedback, alpha, beta, Ah * Ah, omiga_sw, a, phiRB, phiLB, U, t, x);//此处修改  osc_x_feedback(有反馈)||osc_x(无反馈)


  emxFree_real_T(&t);
  /* 振荡器输出 */  //对于开环，在此处赋值osc_x；  对于闭环， 在CAN BUS 回调赋值 osc_x_feedback
  for (i = 0; i < 16; i++) {
    osc_x[i] = x->data[(x->size[0] + x->size[0] * i) - 1];
    osc_x_feedback[i]=osc_x[i];
  }

  /* 修改振荡器输出y为膝关节，振荡器输出x为髋关节【x->data(0)~x->data(7)】 */
  for (i = 0; i < 4; i++) {
    b_i = 1 + (i << 1);
    i2 = x->size[0];
    for (j = 0; j < i2; j++) {
      if (x->data[j + x->size[0] * b_i] <= U[b_i]) {
        x->data[j + x->size[0] * b_i] = -phi * (Ak / Ah) * (x->data[j + x->size
                                                                        [0] * b_i] - U[b_i]) + U[b_i];
      } else {
        x->data[j + x->size[0] * b_i] = U[b_i];
      }
    }
  }

  /*  振荡器关节输出 */
  joint_angle[0] = x->data[x->size[0] - 1];
  joint_angle[1] = x->data[(x->size[0] + x->size[0]) - 1];
  joint_angle[2] = x->data[(x->size[0] + (x->size[0] << 1)) - 1];
  joint_angle[3] = x->data[(x->size[0] + x->size[0] * 3) - 1];
  joint_angle[4] = x->data[(x->size[0] + (x->size[0] << 2)) - 1];
  joint_angle[5] = x->data[(x->size[0] + x->size[0] * 5) - 1];
  joint_angle[6] = x->data[(x->size[0] + x->size[0] * 6) - 1];
  joint_angle[7] = x->data[(x->size[0] + x->size[0] * 7) - 1];
  joint_angle[8] = x->data[(x->size[0] + (x->size[0] << 3)) - 1];
  joint_angle[9] = x->data[(x->size[0] + x->size[0] * 9) - 1];
  joint_angle[10] = x->data[(x->size[0] + x->size[0] * 10) - 1];
  joint_angle[11] = x->data[(x->size[0] + x->size[0] * 11) - 1];
  joint_angle[12] = x->data[(x->size[0] + x->size[0] * 12) - 1];
  joint_angle[13] = x->data[(x->size[0] + x->size[0] * 13) - 1];
  joint_angle[14] = x->data[(x->size[0] + x->size[0] * 14) - 1];
  joint_angle[15] = x->data[(x->size[0] + x->size[0] * 15) - 1];
  /*无侧面冲击时测角度为零*/
  if (acc_roll <= ACC_ROLL_THRESHOLD){
    for(int i=8;i<16;i++){
      joint_angle[i]=0;
    }
  }

/*
  cfprintf(joint_angle[0], joint_angle[1], joint_angle[8]);//LF
  b_cfprintf(joint_angle[2], joint_angle[3], joint_angle[10]);//RF
  c_cfprintf(joint_angle[4], joint_angle[5], joint_angle[12]);//RB
  d_cfprintf(joint_angle[6], joint_angle[7], joint_angle[14]);//LB
*/
  emxFree_real_T(&x);
}

/*
 * File trailer for CPGNetWork.c
 *
 * [EOF]
 */
