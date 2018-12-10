/*
 * File: main.c
 *
 * MATLAB Coder version            : 3.1
 * C/C++ source code generated on  : 10-Dec-2018 17:27:26
 */

/*************************************************************************/
/* This automatically generated example C main file shows how to call    */
/* entry-point functions that MATLAB Coder generated. You must customize */
/* this file for your application. Do not modify this file directly.     */
/* Instead, make a copy of this file, modify it, and integrate it into   */
/* your development environment.                                         */
/*                                                                       */
/* This file initializes entry-point function arguments to a default     */
/* size and value before calling the entry-point functions. It does      */
/* not store or use any values returned from the entry-point functions.  */
/* If necessary, it does pre-allocate memory for returned values.        */
/* You can use this file as a starting point for a main function that    */
/* you can deploy in your application.                                   */
/*                                                                       */
/* After you copy the file, and before you deploy it, you must make the  */
/* following changes:                                                    */
/* * For variable-size function arguments, change the example sizes to   */
/* the sizes that your application requires.                             */
/* * Change the example values of function arguments to the values that  */
/* your application requires.                                            */
/* * If the entry-point functions return values, store these values or   */
/* otherwise use them as required by your application.                   */
/*                                                                       */
/*************************************************************************/
/* Include Files */
#include "rt_nonfinite.h"
#include "CPGNetWork.h"
#include "CPG_gait.h"
#include "CPGNetWork_terminate.h"
#include "CPGNetWork_initialize.h"
#include <stdio.h>

/*
 * Arguments    : void
 * Return Type  : void
 */
void main_CPGNetWork(void)
{
  double acc_roll=0;
  double slope_alpha=0;
  //步态参数
  double T = 0.4;                        //运动周期 m
  double v = 1;                          //运动速度  m/s
  double tfinal = 0.001;                         //响应时间域（Time region of response）

  for(int i = 1; i<100*T/tfinal; i++){
    CPGNetWork(acc_roll,  slope_alpha,  v,  T, osc_x,  tfinal,  joint_type_phi,  theta0,  beta,
     phiRB,  phiLB,  alpha,  a,  k1, k2, joint_angle);

  }
}

/*
 * File trailer for main.c
 *
 * [EOF]
 */
