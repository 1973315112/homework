#include <iostream>
#include <fstream>
#include <chrono>
#include <omp.h>  

using namespace std;


//unsigned short** A;
//unsigned short** B;
bool* C;
bool* D;
int num,row,col1,col2,comrow,nowcol,nowrow,Time,checknum;//,nownum
ifstream In;
ofstream Out("state.csv");
ifstream In2;
ofstream Out2;

const int offset=7;
const int Maxn=127;
__uint128_t** A;
__uint128_t** B;
//const __uint128_t One=((__uint128_t)1);
__uint128_t nownum;
__uint128_t Bit[128];

void read()
{
	Bit[Maxn]=1;
	for(int i=Maxn-1;i>=0;i--)
	{
		Bit[i]=Bit[i+1]<<1;
	}
	
	In>>row>>col1>>col2;
	comrow=(row>>offset)+1;
	A=new __uint128_t*[row];
	C=new bool[row];
	for(int i=0;i<row;i++)
	{
		A[i]=new __uint128_t[comrow];
		C[i]=0;
		for(int j=0;j<comrow;j++)
		{
			A[i][j]=0;
		}
	}
	for(int i=0;i<col1;i++)
	{
		In>>num>>nowrow;
		A[nowrow][(nowrow>>offset)]=A[nowrow][(nowrow>>offset)]|Bit[nowrow&Maxn];
		C[nowrow]=1;
		for(int j=2;j<=num;j++)
		{
			In>>nowcol;
			A[nowrow][(nowcol>>offset)]=A[nowrow][(nowcol>>offset)]|Bit[nowcol&Maxn];
		}
	}
	B=new __uint128_t*[col2];
	D=new bool[col2];
	for(int i=0;i<col2;i++)
	{
		B[i]=new __uint128_t[comrow];
		D[i]=1;
		for(int j=0;j<comrow;j++)
		{
			B[i][j]=0;
		}		
		In>>num;
		for(int j=0;j<=num-1;j++)
		{
			In>>nowcol;
			B[i][(nowcol>>offset)]=B[i][(nowcol>>offset)]|Bit[nowcol&Maxn];
		}		
	}

}

void dfs(__uint128_t x)
{
	if(x<=0) return;
	dfs(x/2);
	cout<<(int)(x%2);
}

void Forward_elimination()//前向消元
{
	auto start=std::chrono::high_resolution_clock::now();
	nowrow=0;
	for(int I=0;nowrow<comrow;nowrow++)
	{
		for(int j=0;j<=Maxn;j++,I++)
		{
			nownum=Bit[j];
			if(C[I]==0)
			{
				for(int i=0;i<col2;i++)
				{
					if(D[i]==0) continue;
					if((B[i][nowrow]&nownum)==0) continue;
					C[I]=1;
					D[i]=0;
					A[I]=B[i];
					break;
				}			
			}
			if(C[I]==0) continue;
			#pragma omp parallel for
			for(int i=0;i<col2;i++)
			{
				if(D[i]==0) continue;
				if((B[i][nowrow]&nownum)==0) continue;
				for(int j=nowrow;j<comrow;j++)
				{
					B[i][j]=B[i][j]^A[I][j];
				}
			}		
		}
	}
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	cout<<"运行时间："<<fixed<<duration.count()<<"μs"<<endl;	
	Out<<fixed<<duration.count()<<",";
}

void Print()
{
	for(int i=0;i<col2;i++)
	{
		if(D[i]==1) 
		{
			Out2<<endl;
			continue;
		}
		for(int j=0;j<row;j++)
		{
			if( (B[i][(j>>offset)]&Bit[j&Maxn]) !=0) Out2<<j<<" ";
		}
		Out2<<endl;
	}
}

bool check()
{
	while(In>>nowcol)
	{
		if(!(In2>>checknum)) return 0;
		if(nowcol!=checknum) return 0;
	}
	if(In2>>checknum) return 0;
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

int main() 
{
	for(int i=1;i<=11;i++)
	{
//------------------------------读入-------------------------
		In.open("./tests/test"+to_string(i)+".txt");
		read();
		In.close();
//------------------------------前向消元---------------------
		Forward_elimination();
//------------------------------输出-------------------------
		Out2.open("myans.txt");
		Print();
		Out2.close();
//------------------------------验证-------------------------
		In.open("./tests/ans"+to_string(i)+".txt");
		In2.open("myans.txt");
		if(check()==0) cerr<<"test"<<i<<"答案错误"<<endl;
		else cerr<<"test"<<i<<"答案正确"<<endl;
		In2.close();
		In.close();
	}
	Out.close();
	return 0;
}