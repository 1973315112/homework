#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

int N=4;
double C[5100][5100];
int pivotcol,y;
double pivot,k;
double diff=0,diffrate=0;
ifstream In;
ofstream Out("openmp.csv");

void show()
{
	for(int i=1;i<=N;i++)
	{
		for(int j=1;j<=N+1;j++)
		{
			cout<<C[i][j]<<" ";
		}
		cout<<endl;
	}	
	cout<<endl<<endl;
}

void open(string i)
{
	In.open("data"+i+".txt");
	In>>N;
	for(int i=1;i<=N;i++)
	{
		for(int j=1;j<=N+1;j++)
		{
			In>>C[i][j];
		}
	}
	In.close();	
}

void Forward_elimination()//前向消元
{
	for(int I=1;I<=N;I++)
	{
		//主元选择及交换
		pivot=abs(C[I][I]);
		pivotcol=I;
		for(int i=I;i<=N;i++)
		{
			if(pivotcol<abs(C[i][I]))
			{
				pivotcol=i;
				pivot=abs(C[i][I]);
			}
		}
		pivot=C[pivotcol][I];
		for(int j=I;j<=N+1;j++)
		{
			swap(C[pivotcol][j],C[I][j]);
			C[I][j]=C[I][j]/pivot;
		}
		//行加减运算
		#pragma omp parallel for
		for(int i=I+1;i<=N;i++)
		{
			k=C[i][I];
			C[i][I]=0;
			for(int j=I+1;j<=N+1;j++)
			{
				C[i][j]=C[i][j]-k*C[I][j];
			}
		}
	}	
}

void backband_solving()//回带求解
{
	#pragma omp parallel for
	for(int I=N;I>=1;I--)
	{
		for(int i=I-1;i>=1;i--)
		{
			C[i][N+1]=C[i][N+1]-C[I][N+1]*C[i][I];
			C[i][I]=0;
		}
	}
}

int main() 
{
	cout<<"openmp"<<endl;
	Out<<"n,time"<<endl;
	for(int I=0;I<=5;I++)
	{
	    cout<<"test"<<(I+1)<<endl;
	    diff=0;
	    diffrate=0;
		string I_s=to_string(I);
		open(I_s);
		Out<<N<<",";
		
		auto start=std::chrono::high_resolution_clock::now();
		Forward_elimination();
		backband_solving();
		auto end=std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> duration = end - start;
		cout<<"运行时间："<<fixed<<duration.count()<<"μs"<<endl;
		Out<<fixed<<duration.count()<<endl;
		
		In.open("ans"+I_s+".txt");
		for(int i=1;i<=N;i++)
		{
			In>>pivot;
			diff=max(diff,(double)abs(pivot-C[i][N+1]));
			diffrate=max(diffrate,(double)abs(pivot-C[i][N+1])/pivot);
		}
		cout<<"diff:"<<diff<<" diffrate:"<<diffrate<<endl;		
		In.close();
	}
	
	return 0;
}