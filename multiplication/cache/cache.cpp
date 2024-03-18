#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>

using namespace std;

void Ini();
void ordinary(int n);
void cache(int n);
ofstream outputFile("cache.csv"); // 创建 CSV 文件

const int N=10005;
int A[N],B[N][N],sum[N];

int main() 
{
	Ini();
	outputFile << "n,ordinary,cache" << endl;
	for(int i=100;i<=5000;i=i+100)
	{
		ordinary(i);
		cache(i);
	}
	cout<<endl;	
	outputFile.close();
	return 0;
}

void Ini()
{
	for(int i=1;i<=N;i++)
	{
		A[i]=i;
	}
	for(int i=1;i<=N;i++)
	{
		for(int j=1;j<=N;j++)
		{
			B[i][j]=i+j;
		}
	}	
	cout<<"数据加载成功"<<endl;
}



void ordinary(int n)
{
	auto start=std::chrono::high_resolution_clock::now();
	for(int I=1;I<=10;I++)
	{
		for(int i=1;i<=n;i++)
		{
			sum[i]=0;
		}
		for(int i=1;i<=n;i++)
		{
			for(int j=1;j<=n;j++)
			{
				sum[i]+=A[j]*B[j][i];
			}
		}
	}	
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	cout<<"当n="<<n<<"时,ordinary运行时间："<<fixed<<duration.count()/10<<"μs"<<endl;
	outputFile << n<< "," << fixed <<duration.count()/10;	
}

void cache(int n)
{
	auto start=std::chrono::high_resolution_clock::now();
	for(int I=1;I<=10;I++)
	{	
		for(int i=1;i<=n;i++)
		{
			sum[i]=0;
		}
		for(int i=1;i<=n;i++)
		{
			for(int j=1;j<=n;j++)
			{
				sum[j]+=A[i]*B[i][j];
			}
		}
	}
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	cout<<"当n="<<n<<"时,cache运行时间："<<fixed<<duration.count()/10<<"μs"<< endl;
	outputFile<< "," << fixed <<duration.count()/10<<endl;
}