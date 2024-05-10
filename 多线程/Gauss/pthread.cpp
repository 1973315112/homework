#include <iostream>
#include <fstream>
#include <chrono>
#include <pthread.h>

using namespace std;

struct Parameter
{
	int Id;//第几个线程
	int I;//工作到第几列
};

const int MAX_THREADS=4; // 设置最大线程数
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;// 互斥锁
pthread_t handles[MAX_THREADS+1];// 创建对应的 Handle 
Parameter param[MAX_THREADS+1];// 创建对应的线程参数

int N=4;
double C[5100][5100];
int pivotcol,y;
double pivot,k;
double diff=0,diffrate=0;
ifstream In;
ofstream Out("pthread.csv");

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

void* onethread_Forward_elimination(void *arg)
{
	//行加减运算
	Parameter para=*(Parameter*)arg;
	int I=para.I;
	int begin=para.I+para.Id;
	for(int i=begin;i<=N;i=i+MAX_THREADS)
	{
		pthread_mutex_lock(&mutex);
		k=C[i][I];
		C[i][I]=0;
		for(int j=I+1;j<=N+1;j++)
		{
			C[i][j]=C[i][j]-k*C[I][j];
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
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

		//分配任务,创建线程
		for(int i=1;i<=MAX_THREADS;i++)
		{
			param[i].I=I;
			param[i].Id=i;
			//cout<<pthread_create(&handles[i],NULL,onethread_Forward_elimination,&param[i])<<endl;	
			pthread_create(&handles[i],NULL,onethread_Forward_elimination,&param[i]);
		}
		//挂起主线程
		for(int i=1;i<=MAX_THREADS;i++)
		{
			//cout<<pthread_join(handles[i],NULL)<<endl;
			pthread_join(handles[i],NULL);
		}
	}	
}

void* onethread_backband_solving(void *arg)
{
	//行加减运算
	Parameter para=*(Parameter*)arg;
	int I=para.I;
	int begin=para.I-para.Id;
	
	for(int i=begin;i>=1;i=i-MAX_THREADS)
	{
		pthread_mutex_lock(&mutex);
		C[i][N+1]=C[i][N+1]-C[I][N+1]*C[i][I];
		C[i][I]=0;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

void backband_solving()//回带求解
{
	for(int I=N;I>=1;I--)
	{
		//分配任务,创建线程
		for(int i=1;i<=MAX_THREADS;i++)
		{
			param[i].I=I;
			param[i].Id=i;
			//cout<<pthread_create(&handles[i],NULL,onethread_Forward_elimination,&param[i])<<endl;	
			pthread_create(&handles[i],NULL,onethread_backband_solving,&param[i]);
		}
		//挂起主线程
		for(int i=1;i<=MAX_THREADS;i++)
		{
			//cout<<pthread_join(handles[i],NULL)<<endl;
			pthread_join(handles[i],NULL);
		}		
	}
}

int main() 
{
	cout<<"pthread"<<endl;
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