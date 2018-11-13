/*************************************************************************
	> File Name: Urg_func.h
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018年10月25日 星期四 10时13分10秒
 ************************************************************************/
#ifndef URF_URG_FUNC_H
#define URF_URG_FUNC_H
#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
#include "MR2_scan2point.h"
#include <iostream>

using namespace std;

extern float Position[2];

namespace qrk
{
    class Urg_func : public Urg_driver
    {
    private:
        
    public:    
        void print_data(const vector<long>& data, long time_stamp);
        int get_position(int argc, const char*const argv[]);//更新坐标
    };
}

#endif