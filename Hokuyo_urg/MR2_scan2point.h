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
	float angle[3];
	double p;
}Sign;

class MR2Scan2Point
{
	public:
		MR2Scan2Point();//���캯��,���ڳ�ʼ�� 
		/**
		@brief  ɨ����ִ�к�����ʹ��ǰ��Ҫ����Init_Lastpos��ʼ������ 
		@param  dataΪ�������飬numΪ���鳤�ȣ�bgn��endΪ������Χ 
		*/
		int POS_3_COLUMN(long *data, int num, int bgn, int end);
		 /**
		@brief  ��ʼ����һ�β������ڶ���մ�����ʱ����ã��Ժ��ٴ�ɨ�蹹�ɵ�������Ҫ���� 
		@param  posLenΪ������ĳ�ʼ���distanceΪ������ľ��룬angleΪ������ĽǶ� 
		*/
		void Init_Lastpos(int posLen[3], long distance[3], int angle[3]);
		/*
		*fr��sc,trPointΪ�����������λ�� ������������˳�򣩣�DisΪ������������ 
		*/
		void RetValue(int &frPoint, int &scPoint, int &trPoint, double &frDis, double &scDis, double &trDis);
		/*
		*@brief ����ɢ��
		@param lenΪɢ��ĵ�����˹����� rateΪ�ж�ɢ�������������Ĳ�ֵ 
		*/
		void filter(int len, double rate); 
	private:
		double array[ARRAY_LEN];
		int searchposNum;//����ĵ������ 
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
