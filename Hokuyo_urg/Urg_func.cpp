/*************************************************************************
	> File Name: Urg_func.cpp
	> Author: yi rongjie
	> Mail: yirj0809@gmail.com
	> Created Time: 2018年10月25日 星期四 10时13分31秒
 ************************************************************************/

#include<iostream>
#include "Urg_func.h"

using namespace std;
using namespace qrk;

static float Pose_high[2]={5360,43480};
static float Pose_mid[2]={400,43480};
static float Pose_low[2]={2390,43480};
float Position[2]={0,0};

void Urg_func::print_data(const vector<long>& data, long time_stamp)
{
#if 1
    // Shows only the front step
        int front_index = this->step2index(0);
        cout << data[front_index] << " [mm], ("
             << time_stamp << " [msec])" << endl;

#else
    // Prints the X-Y coordinates for all the measurement points
        long min_distance = this->min_distance();
        long max_distance = this->max_distance();
        size_t data_n = data.size();
        for (size_t i = 0; i < data_n; ++i) {
            long l = data[i];
            if ((l <= min_distance) || (l >= max_distance)) {
                continue;
            }

            double radian = this->index2rad(i);
            long x = static_cast<long>(l * cos(radian));
            long y = static_cast<long>(l * sin(radian));
            cout << "(" << x << ", " << y << ")" << endl;
        }
        cout << endl;
#endif    
}
int Urg_func::get_position(int argc, const char*const argv[])
{
    Connection_information information(argc, argv);
    
    if (!this->open(information.device_or_ip_name(), information.baudrate_or_port_number(), information.connection_type())) 
    {
        cout << "Urg_driver::open(): "
             << information.device_or_ip_name() << ": " << this->what() << endl;
        return 1;
    }

    // Gets measurement data
#if 1
    // Case where the measurement range (start/end steps) is defined
    this->set_scanning_parameter(this->deg2step(-135), this->deg2step(+135), 0);
#endif
    enum { Capture_times = 1 };
    this->start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
    for (int i = 0; i < 1;++i)//Capture_times; ++i) 
    {
        vector<long> data;
        long time_stamp = 0;

        if (!this->get_distance(data, &time_stamp)) 
        {
            cout << "Urg_driver::get_distance(): " << this->what() << endl;
            return 1;
        }
/*
	cout<<data.size()<<endl;
	for(int i = 0; i < data.size(); i++)
	{
		cout<<data[i]<<",";
	}
*/

/*
	MR2Scan2Point a;
	int g,b,c;
	double d,e,f;
	int a1[] = {600,800};//位置范围 
	int a2[] = {14,100};//点宽最大值 点深平均值
	long a3[] = {700,500};//距离范围 
	a.Init_Lastpos(a1,a2,a3);
	a.POS_3_COLUMN(array, 1080, g, b, c, d, e, f);
*/
//王云选 你来改一下
	MR2Scan2Point M;
	int a,b,c;
	double d,e,f;
	int a1[] = {8,9,9};//点宽
	long a2[] = {770,880,1000};//距离
	int a3[] = {300,400,500};//位置 
/*
for(int j = 0; j < data.size(); ++j)
{
	cout<<data[j]<<" ";
}*/
	
	M.Init_Lastpos(a1,a2,a3);
	if(!M.POS_3_COLUMN(&data[0], data.size(), 0, 1079)) cout<<"not found"<<endl;
	M.RetValue(a,b,c,d,e,f);
	
	if(a > b)
	{
		double tmp1 = d;
		double tmp2 = a;
		d = e;
		e = tmp1;
		a = b;
		b = tmp2;
	}
	if(a > c)
	{
		double tmp = d;
		d = f;
		f = tmp;
		double tmp2 = a;
		a = c;
		c = tmp2;
	}
	if(b > c)
	{
		double tmp = e;
		e = f;
		f = tmp;
		double tmp2 = b;
		b = c;
		c = tmp2;
	}
    //d,e,f按方位从小到大排序了	红方f最底下
	//计算坐标:红方
	float r_high=d;
   	float r_mid=e;
    float r_low=f;
    /*计算位置*/
    float Point_x_1=0.5*((r_high*r_high-r_mid*r_mid)/(Pose_mid[0]-Pose_high[0])+Pose_mid[0]+Pose_high[0]);
    float Point_y_1=Pose_high[1]-sqrt(r_high*r_high-(Point_x_1-Pose_high[0])*(Point_x_1-Pose_high[0]));

    float Point_x_2=0.5*((r_high*r_high-r_low*r_low)/(Pose_low[0]-Pose_high[0])+Pose_low[0]+Pose_high[0]);
    float Point_y_2=Pose_high[1]-sqrt(r_high*r_high-(Point_x_2-Pose_high[0])*(Point_x_2-Pose_high[0]));

    float Point_x_3=0.5*((r_low*r_low-r_mid*r_mid)/(Pose_mid[0]-Pose_low[0])+Pose_mid[0]+Pose_low[0]);
    float Point_y_3=Pose_low[1]-sqrt(r_low*r_low-(Point_x_3-Pose_low[0])*(Point_x_3-Pose_low[0]));
    
    Position[0]=(Point_x_1+Point_x_2+Point_x_3)/3;
    Position[1]=(Point_y_1+Point_y_2+Point_y_3)/3;
    cout<<"POS:["<<Position[0]<<","<<Position[1]<<"]"<<endl;
    }

#if defined(URG_MSC)
    getchar();
#endif
    return 0;
}
