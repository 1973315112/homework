#include <iostream>
#include <fstream>
#include <chrono>
#include <math.h>
#include <mpi.h>  
#include <stddef.h> // For size_t
#include <omp.h>   

using namespace std;

unsigned short** A;
unsigned short** B;
bool* C;
bool* D;
int num,row,col1,col2,comrow,nowcol,nowrow,nownum,Time;
ifstream In;
ofstream Out;

const int MPI_US=sizeof(unsigned short);
unsigned short* SSA;
unsigned short* SA;
unsigned short** SB;
unsigned short* SSB;
unsigned short* BB;
bool SC;
bool* SD;
int world_rank,world_size,local_size,Num,from,to,my_from_rank,world_from_rank,which_row;
auto timestart=std::chrono::high_resolution_clock::now();
auto timeend=std::chrono::high_resolution_clock::now();
MPI_Status status;

void subprocess_read()
{
	//声明全长的unsigned short数组，再用unsigned short*数值切割为二维
	SSB=new unsigned short[Num*comrow];
	SSA=new unsigned short[comrow];
	SB=new unsigned short*[Num];
	SD=new bool[Num];
	for(int i=0;i<Num;i++)
	{
		SB[i]=&SSB[i*comrow];
		SD[i]=1;
	}	
}


void read()
{
	SB=new unsigned short*[Num];
	A=new unsigned short*[row];
	C=new bool[row];
	for(int i=0;i<row;i++)
	{
		A[i]=new unsigned short[comrow];
		C[i]=0;
		for(int j=0;j<comrow;j++)
		{
			A[i][j]=0;
		}
	}
	for(int i=0;i<col1;i++)
	{
		In>>num>>nowrow;
		A[nowrow][(nowrow>>4)]=A[nowrow][(nowrow>>4)]|(1<<(15-(nowrow&15)));
		C[nowrow]=1;
		for(int j=2;j<=num;j++)
		{
			In>>nowcol;
			A[nowrow][(nowcol>>4)]=A[nowrow][(nowcol>>4)]|(1<<(15-(nowcol&15)));
		}
   		//cerr<<A[i][0]<<endl;
	}
	//声明全长的unsigned short数组，再用unsigned short*数值切割为二维
	BB=new unsigned short[col2*comrow];
	B=new unsigned short*[col2];
	D=new bool[col2];
	for(int i=0;i<col2;i++)
	{
		B[i]=&BB[i*comrow];
		D[i]=1;
		for(int j=0;j<comrow;j++)
		{
			B[i][j]=0;
		}		
		In>>num;
		for(int j=0;j<=num-1;j++)
		{
			In>>nowcol;
			B[i][(nowcol>>4)]=B[i][(nowcol>>4)]|(1ULL<<(15-(nowcol&15)));
		}	
	}
	
	for(int i=0;i<Num;i++)
	{
		SB[i]=B[i];
	}
	SD=D;
}

void mpi_Forward_elimination()//前向消元
{
	//auto start=std::chrono::high_resolution_clock::now();
//---------------------------------------------------------------------------------
	for(int I=0;I<row;I++)
	{		
		nowrow=(I>>4);
		nownum=(1<<(15-(I&15)));
		my_from_rank=1e9;
		world_from_rank=1e9;
		which_row=-1;
		if(world_rank==0) SA=A[I];
		else SA=SSA;
		
		//首先广播消元子是否存在
		if(world_rank==0) SC=C[I];
		MPI_Bcast(&SC,1,MPI_BYTE,0,MPI_COMM_WORLD);
		
		if(SC==1) world_from_rank=0; //如果存在,world_from_rank设为0
		else//如果不存在,各进程寻找消元子
		{
			for(int i=0;i<Num;i++)
			{
				if(SD[i]==0) continue;
				if((SB[i][nowrow]&nownum)==0) continue;
    			my_from_rank=world_rank;
				which_row=i;
				break;
			}
			MPI_Allreduce(&my_from_rank,&world_from_rank,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD); //找到线程编号最小的消元子
		}
		
		if(world_from_rank==1e9) continue;
		if(world_rank==0) C[I]=1;
		
		if(world_rank==world_from_rank&&which_row!=-1) //准备广播消元子
		{
			SD[which_row]=0;
			SA=SB[which_row];
			if(world_rank==0) A[I]=SA;		
		}
		
		MPI_Bcast(&SA[0],comrow*MPI_US,MPI_BYTE,world_from_rank,MPI_COMM_WORLD);//广播消元子
		
		for(int i=0;i<Num;i++)
		{
			if(SD[i]==0) continue;
			if((SB[i][nowrow]&nownum)==0) continue;
   			for(int j=nowrow;j<comrow;j++) 
			{
        		SB[i][j]=SA[j]^SB[i][j];
    		}
		}
	}
}

bool check()
{
	for(int i=0;i<col2;i++)
	{
		In>>num;
		if(num==0) continue;
		In>>nowrow;
		if(C[nowrow]==0) return 0;
		for(int j=0;j<nowrow;j++)
		{
			if((A[nowrow][(j>>4)]&(1<<(15-(j&15))))!=0) return 0;
		}
		if( (A[nowrow][(nowrow>>4)]&(1<<(15-(nowrow&15))))==0) return 0;
		for(int j=nowrow+1;j<row;j++)
		{
			if( (A[nowrow][(j>>4)]&(1<<(15-(j&15)))) !=0)
			{
				In>>nowcol;
				if(j!=nowcol) return 0;
			}
		}
	}	
	return 1;
}

void del()
{
	In.close();
	Out.close();
	for(int i=0;i<row;i++)
	{
		delete[] A[i];
	}
	for(int i=0;i<col2;i++)
	{
		delete[] B[i];
	}
	delete[] A;	
	delete[] B;		
	delete[] C;	
	delete[] D;	
}

int main(int argc,char** argv) 
{
omp_set_num_threads(4); // 设置线程数为4
	// 初始化MPI环境，获取总的进程数，获取当前进程的标识符
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);	
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);      
//---------------------------------------------------------------------------------	
	if(world_rank==0)
	{
    	Out.open("mpi.csv");
		Out<<"test,i\n";
	}
	
	for(int i=1;i<=9;i++)
	{
		cerr<<"进程"<<world_rank<<":文件"<<i<<endl;
		if(world_rank==0) 
		{
			cout<<i<<endl;
			Out<<"test"<<i<<",";
			In.open("/home/s2213917/MPI/tests/test"+to_string(i)+".txt");
			In>>row>>col1>>col2;
		}
		
		MPI_Bcast(&row,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&col1,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&col2,1,MPI_INT,0,MPI_COMM_WORLD);
		comrow=(row>>4)+1;
		local_size=col2/world_size;
        if(world_rank<col2%local_size) Num=local_size+1;
        else Num=local_size;
        
    	if(world_rank==0) 
    	{
    		read();
    		timestart=std::chrono::high_resolution_clock::now();
    		//分发数组到所有进程
    		to=local_size;
    		if(col2%local_size>0) to++;
        	for(int rank=1;rank<world_size;rank++) 
			{  
            	from=to;
            	to=to+local_size;
				if(rank<col2%local_size) to++;
            	MPI_Send(&B[from][0],(to-from)*comrow*MPI_US,MPI_BYTE,rank,0,MPI_COMM_WORLD);
        	}
    	}
		else 
		{
			subprocess_read();
			MPI_Recv(&SB[0][0],Num*comrow*MPI_US,MPI_BYTE,0,0,MPI_COMM_WORLD,&status);//接受数组
		}
//---------------------------------------------------------------------------------
		mpi_Forward_elimination();
//---------------------------------------------------------------------------------		
		if(world_rank==0)
		{
			timeend=std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::micro> duration = timeend - timestart;
			cout<<"运行时间："<<fixed<<duration.count()<<"μs"<<endl;	
			Out<<fixed<<duration.count()<<endl;

			if(i!=10&&check()==0) cerr<<i<<"error!"<<endl;
			else cerr<<i<<"success!"<<endl;
			In.close();
		}	
	}
//---------------------------------------------------------------------------------	  
    MPI_Finalize();//清理MPI环境
  	return 0;
}