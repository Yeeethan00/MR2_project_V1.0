/*************************************************************************
	> File Name: decision.h
	> Author: 
	> Mail: 
	> Created Time: 2018年11月13日 星期二 19时05分59秒
 ************************************************************************/

#ifndef _DECISION_H
#define _DECISION_H

#define CPG

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include "../Hokuyo_urg/Urg_func.h"
#include "../Limbs/limbs.h"
#include "../Utils/sem.h"
extern "C" {
#include "../Can/can.h"
#include "../Utils/band_cpu.h"
}

void decision_thread_create();
#endif
