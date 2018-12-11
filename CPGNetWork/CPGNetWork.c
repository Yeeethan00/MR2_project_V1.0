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

/*Define*/
#define PI 3.1415926535
#define ACC_ROLL_THRESHOLD 0.909
#define LENHGTH 0.64 //�峤
#define WIDTH 0.48  //���
#define L 0.3 //�ȳ�

/*global */
/* ��̬���� */
/* �������ӣ���ʾ֧������ռ���� */
joint_type joint_type_phi=phi_elbow;
double theta0 = PI/6;  //���봹ֱ���������棩�нǣ�ԭƽ��λ��
double beta=0.5;       //�������ӣ���ʾ֧������ռ����
int phiRB=0, phiLB=0.5;  //RB LB��λ, ��betaһͬ������̬
int alpha=100;           //�����������������޻��õ��ٶ�
int a=50;                //����omiga��omiga_sw��omiga_st֮��ı仯�ٶ�
double k2=2.26;           //ƽ��λ�øı���/������̬��
double k1=0.24;         //������̬��/�¶Ƚ�

double joint_angle[16]; //�ؽڽ�
double osc_x[16]={0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //������ʼֵ��Initial value��

/* Function Definitions */

/*
 * % ����
 * ��̬����
 * Arguments    : double acc_roll
 *                double slope_alpha
 *                double v
 *                double T
 *                double osc_x[16]
 *                double tfinal
 *                double phi
 *                double theta0
 *                double beta
 *                int phiRB
 *                int phiLB
 *                int alpha
 *                int a
 *                double k1
 *                double k2
 *                double joint_angle[16]
 * Return Type  : void
 */
void CPGNetWork(double acc_roll, double slope_alpha, double v, double T,
                double tfinal, double phi)
{
  double omiga_sw;
  double h;
  emxArray_real_T *x;
  emxArray_real_T *t;
  double Ak;
  double Ah;
  double delte_theta;
  double U[16];
  double dv1[2];
  int i;
  int b_i;
  int i2;
  int j;

  /* ��̬���� */
  /* �ڶ���Ƶ�� */
  omiga_sw = 1.0 / (beta * T);

  /* �ڶ�����˸߶�  m */
  if (acc_roll > ACC_ROLL_THRESHOLD) {
    h = acc_roll * sin(atan(WIDTH/LENHGTH)) / (2.0 * (omiga_sw * omiga_sw) * sqrt(1.0
      - acc_roll * acc_roll / (4.0 * rt_powd_snf(omiga_sw, 4.0))));
  } else {
    h = 0.02;
  }

  emxInit_real_T(&x, 2);
  emxInit_real_T1(&t, 1);
  /* �Źؽڷ�ֵ */
  Ak = acos((L * cos(theta0) - h) / L) - theta0;
  /* ϥ�ؽڷ�ֵ */
  Ah = asin(beta * v * T / (4*L / cos(theta0)));


  /* �������� */
  /* ������ֵ A=sqrt(miu) */
  /* ��������� */
  /* ǰͥ���� */
  /* ԭƽ��λ�øı��� */
  delte_theta = phi * k2 * k1 * slope_alpha;


  /* �ⲿ������(feed) ��Ӧx1 y1 x2 y2 x3 y3 x4 y4 */
  U[0] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[1] = delte_theta;
  U[2] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[3] = delte_theta;
  U[4] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[5] = delte_theta;
  U[6] = -(delte_theta / 2.0 - k1 * slope_alpha);
  U[7] = delte_theta;
  memset(&U[8], 0, sizeof(double) << 3);

  /* % ��ֵ */
  /*  ��Ӧʱ����Time region of response�� */
  /* % ��� */
  /*  ���Hopf����΢�ַ��̵���ֵ�⣨Numerical solution of differential equation�� */
  dv1[0] = 0.0;
  dv1[1] = tfinal;
  ode45(dv1, osc_x, alpha, beta, Ah * Ah, omiga_sw, a, phiRB, phiLB, U, t, x);

  /* ������� */
  emxFree_real_T(&t);
  for (i = 0; i < 16; i++) {
    osc_x[i] = x->data[(x->size[0] + x->size[0] * i) - 1];
  }

  /* �޸��������yΪϥ�ؽڣ��������xΪ�Źؽ� */
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

  /*  �����ؽ���� */
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
