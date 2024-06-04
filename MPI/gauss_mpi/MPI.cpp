#include <iostream>
#include <fstream>
#include <chrono>
#include <mpi.h>  
#include <stddef.h> 
#include <math.h>

using namespace std;

int N=4;
double** C;
int pivotcol,y;
double pivot,k;
double diff=0,diffrate=0;
ifstream In;
ofstream Out;


const double e=0.0001; 
int world_rank;
int world_size;
int local_size;
int Num;
int to;
int from;
int my_from_rank;
int world_from_rank;
double my_pivot;
double world_pivot;
double** SC;//不动
double* SCC;//不动
double* CC;//不动
double* A;
double* AA;//不动
auto timestart=std::chrono::high_resolution_clock::now();
auto timeend=std::chrono::high_resolution_clock::now();
MPI_Status status;

void show()
{
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N+1;j++)
		{
			cerr<<C[i][j]<<" ";
		}
		cerr<<endl;
	}	
	cerr<<endl<<endl;
}

void subprocess_open()
{
	AA=new double[N+1];
	A=AA;
	SCC=new double[Num*(N+1)];
	SC=new double*[Num];
	for(int i=0;i<Num;i++)
	{
		SC[i]=&SCC[i*(N+1)];
	}
}

void open()
{
	CC=new double[N*(N+1)];
	C=new double*[N];
	for(int i=0;i<N;i++)
	{
		C[i]=&CC[i*(N+1)];
		for(int j=0;j<N+1;j++)
		{
			In>>C[i][j];
		}
	}
	In.close();	
}

void MPI_Forward_elimination()//前向消元
{
	for(int I=0;I<N;I++)
	{
//---------------------------------------------------------------------------------	
		//初始化工作
		if(world_rank==0) A=C[I];
		else A=AA;
		my_from_rank=1e9;
		world_from_rank=1e9;		
//---------------------------------------------------------------------------------			
		//主元选择及交换
		my_pivot=0;
		pivotcol=-1;
		for(int i=0;i<Num;i++)
		{
			if(my_pivot<abs(SC[i][I]))
			{
				pivotcol=i;
				my_pivot=abs(SC[i][I]);
			}
		}
		MPI_Allreduce(&my_pivot,&world_pivot,1,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);
		if(world_pivot==0) continue;
		if(abs(my_pivot-world_pivot)<e) my_from_rank=world_rank;
		MPI_Allreduce(&my_from_rank,&world_from_rank,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);	
		if(world_rank==world_from_rank) 
		{
			Num--;
			swap(SC[pivotcol],SC[Num]);
			if(world_rank==0) C[I]=SC[Num];
			A=SC[Num];
			world_pivot=SC[Num][I];
			for(int i=I;i<N+1;i++)
			{
				A[i]=A[i]/world_pivot;
			}
		}
		MPI_Bcast(&A[0],N+1,MPI_DOUBLE,world_from_rank,MPI_COMM_WORLD);
//---------------------------------------------------------------------------------
		//行加减运算
		for(int i=0;i<Num;i++)
		{
			if(SC[i][I]==0) continue;
			k=SC[i][I];
			SC[i][I]=0;
			for(int j=I+1;j<N+1;j++)
			{
				SC[i][j]=SC[i][j]-k*A[j];
			}
		}			
	}	
}

void backband_solving()//回带求解
{
	for(int I=N-1;I>=0;I--)
	{
		//cerr<<"线程0:回带求解至："<<I<<endl;
		for(int i=I-1;i>=0;i--)
		{
			C[i][N]=C[i][N]-C[I][N]*C[i][I];
			C[i][I]=0;
		}
	}
}

int main(int argc,char** argv) 
{
	// 初始化MPI环境，获取总的进程数，获取当前进程的标识符
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);	
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);      

	if(world_rank==0)
	{
    	Out.open("mpi.csv");
    	cerr<<"开始MPI版本高斯消元,进程数为:"<<world_size<<endl;
		Out<<"n,time"<<endl;		
	}
//---------------------------------------------------------------------------------	
	for(int I=0;I<=5;I++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		cerr<<"进程"<<world_rank<<":文件"<<I<<endl;
		if(world_rank==0)
		{
			cout<<I<<endl;
			Out<<"test"<<I<<",";
			In.open("/home/s2213917/MPI2/tests/data"+to_string(I)+".txt");
			In>>N;			
		}
		MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);
		local_size=N/world_size;
        if(world_rank<N%local_size) Num=local_size+1;
        else Num=local_size;
        subprocess_open();
        
        if(world_rank==0) 
    	{
    		open();
    		//分发数组到所有进程
    		to=local_size;
    		if(N%local_size>0) to++;
        	for(int rank=1;rank<world_size;rank++) 
			{
            	from=to;
            	to=to+local_size;
				if(rank<N%local_size) to++;
            	MPI_Send(&C[from][0],(to-from)*(N+1),MPI_DOUBLE,rank,0,MPI_COMM_WORLD);
        	}
        	for(int i=0;i<Num;i++)
        	{
        		for(int j=0;j<N+1;j++)
        		{
        			SC[i][j]=C[i][j];
				}
			}
    	}
		else MPI_Recv(&SC[0][0],Num*(N+1),MPI_DOUBLE,0,0,MPI_COMM_WORLD,&status);//接受数组
//---------------------------------------------------------------------------------
		MPI_Barrier(MPI_COMM_WORLD);
		if(world_rank==0) timestart=std::chrono::high_resolution_clock::now();
//---------------------------------------------------------------------------------	  
		MPI_Forward_elimination();
//---------------------------------------------------------------------------------	 
		if(world_rank==0) 
		{
			backband_solving();
			timeend=std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::micro> duration = timeend - timestart;
			cout<<"运行时间："<<fixed<<duration.count()<<"μs"<<endl;
			Out<<fixed<<duration.count()<<endl;
//---------------------------------------------------------------------------------
		    diff=0;
		    diffrate=0;			  
			In.open("/home/s2213917/MPI2/tests/ans"+to_string(I)+".txt");
			for(int i=0;i<N;i++)
			{
				In>>pivot;
				diff=max(diff,(double)abs(pivot-C[i][N]));
				diffrate=max(diffrate,(double)abs(pivot-C[i][N])/pivot);
			}
			cerr<<"diff:"<<diff<<" diffrate:"<<diffrate<<endl;
			In.close();
		}
	}
//---------------------------------------------------------------------------------	
    MPI_Finalize();//清理MPI环境	
	return 0;
}