#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>

using namespace std;

void read();
void ordinary(int m,int n);

int M=25;
int N;
long long* A;
long long* C;
long long sum,sum1,sum2;

int main()
{
	read();
	ordinary(20,(1<<20));
	delete[] A;
	delete[] C;
}

void read()
{
	ifstream In("data.txt");
	In>>N;
	A=new long long[N+1];
	C=new long long[N+1];
	for(int i=1;i<=N;i++)
	{
		In>>A[i];
	}
	cout<<"数据加载成功"<<endl;
}

void ordinary(int m,int n)
{
	auto start=std::chrono::high_resolution_clock::now();	
	for(int I=1;I<=100;I++)
	{
	   	sum=0;
	  	sum1=0;
	  	sum2=0;
		for(int i=1;i<=n;i=i+2)
		{
			sum+=A[i];
			sum+=A[i+1];
		}	
	}
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	cout<<"当m="<<m<<"时,ordinary运行时间："<<fixed<<duration.count()/100<<"μs"<<endl;	
}



