/*************************************************************************
	> File Name: location.h
	> Author: 
	> Mail: 
	> Created Time: 2018年11月13日 星期二 19时05分45秒
 ************************************************************************/

#ifndef _LOCATION_H
#define _LOCATION_H
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include "../Hokuyo_urg/Urg_func.h"
#include "../KinectV2/KinectV2_func.h"
#include "../utils/sem.h"
extern "C" {
#include "../Can/can.h"
#include "../utils/band_cpu.h"
}

void loaction_thread_create();
#endif
