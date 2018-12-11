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

/*typedef*/
typedef enum {
    phi_elbow=1,
    phi_knee=-1
}joint_type;

/*global */
extern double theta0;
extern double beta;
extern int phiRB, phiLB;
extern int alpha;
extern int a;
extern double k2;
extern double k1;
extern double joint_angle[16];
extern double osc_x[16];

/* Function Declarations */
void CPGNetWork(double acc_roll, double slope_alpha, double v, double T, double tfinal, double phi);

#endif

/*
 * File trailer for CPGNetWork.h
 *
 * [EOF]
 */
