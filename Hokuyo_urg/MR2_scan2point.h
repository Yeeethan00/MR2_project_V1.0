#ifndef __MR2_SCAN2POINT_H__
#define __MR2_SCAN2POINT_H__
 
#define MAX_POINT_WIDTH 20
#define RNG_LEN_RATIO  20
#define LEN_MIN 0
#define LEN_MAX 3000
#define ARRAY_LEN 1090
#define DEPTH_MAX 1000

#include <cmath>
#include<iostream>
using namespace std;
typedef struct
{
	int pointLen[3];
	double distance[3];
	int angle[3];
	double p;
}Sign;

class MR2Scan2Point
{
	public:
		MR2Scan2Point();//构造函数,用于初始化 
		/**
		@brief  扫描点的执行函数，使用前需要调用Init_Lastpos初始化函数 
		@param  data为输入数组，num为数组长度，bgn和end为搜索范围 
		*/
		int POS_3_COLUMN(long *data, int num, int bgn, int end);
		 /**
		@brief  初始化第一次采样，在对象刚创建的时候调用，以后再次扫描构成迭代不需要调用 
		@param  posLen为三个点的初始点宽，distance为三个点的距离，angle为三个点的角度 
		*/
		void Init_Lastpos(int posLen[3], long distance[3], int angle[3]);
		/*
		*fr，sc,trPoint为输出的三个点位置 （按输入数组顺序），Dis为输出三个点距离 
		*/
		void RetValue(int &frPoint, int &scPoint, int &trPoint, double &frDis, double &scDis, double &trDis);
		/*
		*@brief 过滤散点
		@param len为散点的点宽（不宜过长） rate为判断散点相对于正常点的差值 
		*/
		void filter(int len, double rate); 
	private:
		double array[ARRAY_LEN];
		int searchposNum;//输入的点的数量 
		double disRange_min;
		double disRange_max;
		int searchRange_min;
		int searchRange_max;
		Sign memory[DEPTH_MAX];
		Sign lastSign;
		
		void computePro(Sign &sign, int posNum);
		void Save_to_memory(int bgn, int end, int cnt, int posNum);
		double Max_Of_Three(double a,double b,double c);
		double Min_Of_Three(double a,double b,double c);
		int Renew_Lastpos();
};

#endif
