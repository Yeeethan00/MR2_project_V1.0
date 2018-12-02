#include "MR2_scan2point.h"

double MR2Scan2Point::Max_Of_Three(double a,double b,double c)
{
	double tmp = a > b ? a : b;
	return tmp > c ? tmp : c;
}

double MR2Scan2Point::Min_Of_Three(double a,double b,double c)
{
	double tmp = a > b ? b : a;
	return tmp > c ? c : tmp;
}

MR2Scan2Point::MR2Scan2Point()
{
	disRange_min = 0;
	disRange_max = 65533;
	searchRange_min = 0;
	searchRange_max = 1081;
	for(int i = 0; i < DEPTH_MAX; i++)
	{
		memory[i].p = 0;
		for(int j = 0; j < 3; j++)
		{
			memory[i].pointLen[j] = 0;
			memory[i].distance[j] = 0;
			memory[i].angle[j] = 0;
		}
	}
}

void MR2Scan2Point::filter(int len, double rate)//����ɢ��,len���˹��� 
{
	int lastPtr = searchRange_min;
	int curPtr = searchRange_min;
	int bgn =  searchRange_min;
	int state = 0;
	while(curPtr <= searchRange_max)
	{	
		switch(state)
		{
			case 0:
			{
				if(array[curPtr] - array[lastPtr] >= rate)
				{
					bgn = curPtr;
					state = 1;
				}
				else if(array[curPtr] - array[lastPtr] <= -rate)
				{
					bgn = curPtr;
					state = 2;
				}
				lastPtr = curPtr;
				curPtr++;
				break;
			}
			case 1:
			{
				if(curPtr - bgn > len)
				{
					state = 0;
				}
				else if(array[curPtr] - array[lastPtr] <= -rate)
				{
					double mid = (array[bgn - 1] + array[curPtr]) / 2;
					for(int i = bgn; i < curPtr; i++)
					{
						array[i] = mid;
					}
					state = 0;
				}
				else 
				{
					lastPtr = curPtr;
					curPtr++;
				}
				break;	
			}
			case 2:
			{
				if(curPtr - bgn > len)
				{
					state = 0;
				}
				else if(array[curPtr] - array[lastPtr] >= rate)
				{
					double mid = (array[bgn - 1] + array[curPtr]) / 2;
					for(int i = bgn; i < curPtr; i++)
					{
						array[i] = mid;
					}
					state = 0;
				}
				else 
				{
					lastPtr = curPtr;
					curPtr++;
				}
				break;	
			}
		}

	}
}

void MR2Scan2Point::Save_to_memory(int bgn, int end, int cnt, int posNum)
{
	memory[cnt].pointLen[posNum] = abs(end - bgn) + 1;
	memory[cnt].angle[posNum] = (bgn + end) / 2;
	int e = bgn > end ? bgn : end;
	int b = bgn < end ? bgn : end;
	double add = 0;
	for(int i = b; i <= e; i++)
	{
		add += array[i];
	}
	memory[cnt].distance[posNum] = add / memory[cnt].pointLen[posNum];
}

void MR2Scan2Point::computePro(Sign &sign, int posNum)
{
	double innerProduct = 0;
	double mode1 = 0, mode2 = 0;
	int alpha = 130, beta = 1, gamma = 1;//����������Ȩֵ 
	for(int i = 0; i < posNum; i++)
	{
		innerProduct += alpha * alpha * lastSign.pointLen[i] * sign.pointLen[i];
		innerProduct += beta * beta * lastSign.distance[i] * sign.distance[i];	
		innerProduct += gamma * gamma * lastSign.angle[i] * sign.angle[i];	
		mode1 += alpha * alpha * lastSign.pointLen[i] * lastSign.pointLen[i];
		mode1 += beta * beta * lastSign.distance[i] * lastSign.distance[i];
		mode1 += gamma * gamma * lastSign.angle[i] * lastSign.angle[i];
		mode2 += alpha * alpha * sign.pointLen[i] * sign.pointLen[i];
		mode2 += beta * beta * sign.distance[i] * sign.distance[i];
		mode2 += gamma * gamma * sign.angle[i] * sign.angle[i];
	}
	if(posNum < 3) 
	{
		sign.p = 0;
	}
	else 
	{
		sign.p = innerProduct / (sqrt(mode1) * sqrt(mode2));
	}
}

int MR2Scan2Point::Renew_Lastpos()
{
	double maxDis = Max_Of_Three(lastSign.distance[0], lastSign.distance[1], lastSign.distance[2]);
	double minDis = Min_Of_Three(lastSign.distance[0], lastSign.distance[1], lastSign.distance[2]);
	int beginPos = (lastSign.angle[0] + lastSign.angle[1] + lastSign.angle[2]) / 3;
	int cnt = 0;
	while((maxDis < disRange_max || minDis > disRange_min) && cnt < DEPTH_MAX - 1)
	{
		int i = beginPos, j = beginPos, lastI = i, lastJ = j, firstPosIn = 0, posNum = 0, bgni = 0, bgnj = 0;	
		if(array[i] >= minDis && array[i] <= maxDis)
		{
			firstPosIn = 1;
		}
		while((i > searchRange_min || j < searchRange_max) && posNum < 3)
		{
			if(1 == firstPosIn)
			{
				while(!((array[lastI] >= minDis && array[lastI] <= maxDis) && (array[i] < minDis || array[i] > maxDis)))
				{
					lastI = i--;
					if(i < searchRange_min)
					{
						i = searchRange_min;
						break;
					}
				}
				while(!((array[lastJ] >= minDis && array[lastJ] <= maxDis) && (array[j] < minDis || array[j] > maxDis)))
				{
					lastJ = j--;
					if(j > searchRange_max)
					{
						j = searchRange_max;
						break;
					}
				}
				firstPosIn = 0;
				Save_to_memory(lastI,lastJ,cnt,posNum++);
			}
			else
			{
				if((array[i] >= minDis && array[i] <= maxDis) && (array[lastI] < minDis || array[lastI] > maxDis))
				{
					bgni = i;
				}
				if((array[lastI] >= minDis && array[lastI] <= maxDis) && (array[i] < minDis || array[i] > maxDis))
				{
					Save_to_memory(bgni,lastI,cnt,posNum++);
				}
				
				if((array[j] >= minDis && array[j] <= maxDis) && (array[lastJ] < minDis || array[lastJ] > maxDis))
				{
					bgnj = j;
				}
				if((array[lastJ] >= minDis && array[lastJ] <= maxDis) && (array[j] < minDis || array[j] > maxDis))
				{
					Save_to_memory(bgnj,lastJ,cnt,posNum++);
				}
				lastI = i--;
				lastJ = j++;
				if(i < searchRange_min)
				{
					i = searchRange_min;
				}
				if(j > searchRange_max)
				{
					j = searchRange_max;
				}
			}		
		}
		computePro(memory[cnt], posNum);
		cnt++;
		maxDis++;
		minDis--; 
		if(maxDis > disRange_max)
		{
			maxDis = disRange_max;
		}
		if(minDis < disRange_min)
		{
			minDis = disRange_min;
		}
	}
	/*//���ȫ�����ҽ�� 
	for(int i = 0; i < DEPTH_MAX; i++)
	{
		
		cout<<"p = "<<memory[i].p;
		for(int j = 0; j < 3; j++)
		{
			cout<<"LEN= "<<memory[i].pointLen[j]<<" ";
			cout<<"DIS= "<<memory[i].distance[j]<<" ";
			cout<<"ANG= "<<memory[i].angle[j]<<" ";
		}
		cout<<endl;
	}*/
	//Ѱ�ҿ��������ĵ㲢�����
	int found = 0;
	double max = -1;
	for(int i = 0; i < cnt; i++)
	{
		if(memory[i].p > max)
		{
			max = memory[i].p;
			found = i;
		}
	}
	if(max <= 0) //���û���ҵ� 
	{
		return -1;
	}
	
	lastSign = memory[found];//��ֵ 
	//������ҽ��
	/*
	cout<<"p = "<<lastSign.p<<endl;
	for(int j = 0; j < 3; j++)
	{
		cout<<"LEN= "<<lastSign.pointLen[j]<<" ";
		cout<<"DIS= "<<lastSign.distance[j]<<" ";
		cout<<"ANG= "<<lastSign.angle[j]<<" ";
	}
	 */
	return 1;
} 
void MR2Scan2Point::Init_Lastpos(int posLen[3], long distance[3], int angle[3])
{
	for(int i = 0; i < 3; i++)
	{
		lastSign.pointLen[i] = posLen[i] > 0 ? posLen[i] : 0;
		lastSign.distance[i] = (double)(distance[i] > 0 ? distance[i] : 0);
		lastSign.angle[i] = angle[i] > 0 ? angle[i] : 0;
	}
}
int MR2Scan2Point::POS_3_COLUMN(long *data, int num, int bgn, int end)
{
	if(num > ARRAY_LEN - 1)
	{
		searchposNum = ARRAY_LEN - 1;
	}
	else if(num <= 0)
	{
		searchposNum = 0;
	}
	else
	{
		searchposNum = num;		
	}
	for(int i = 0; i < searchposNum; i++)
	{
		if(array[i] < 0)
		{
			array[i] = 0;
		}
		array[i] = (double)data[i]; 
	}
	if(bgn < 0)
	{
		bgn = 0;
	}
	if(end - bgn >= searchposNum)
	{
		end = bgn + searchposNum - 1;
	}
	searchRange_min = bgn;
	searchRange_max = end;
	filter(2,500);
	if(Renew_Lastpos() == -1)
	{
		return -1;
	}
	return 1;
}
void MR2Scan2Point::RetValue(int &frPoint, int &scPoint, int &trPoint, double &frDis, double &scDis, double &trDis)
{
	frPoint = lastSign.angle[0];
	scPoint = lastSign.angle[1];
	trPoint = lastSign.angle[2];
	frDis = lastSign.distance[0];
	scDis = lastSign.distance[1];
	trDis = lastSign.distance[2];
}
