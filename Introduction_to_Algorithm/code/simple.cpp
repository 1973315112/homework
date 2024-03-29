#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

bool** A;
bool** B;
bool* C;
bool* D;
int num,row,col1,col2,comrow,nowcol,nowrow,nownum,Time;
ifstream In;
ofstream Out("simple.csv", std::ios::app); // 创建或打开 CSV 文件以追加模式 

void read()
{
	In>>row>>col1>>col2;
	A=new bool*[row];
	C=new bool[row];
	for(int i=0;i<row;i++)
	{
		A[i]=new bool[row];
		C[i]=0;
		for(int j=0;j<row;j++)
		{
			A[i][j]=0;
		}
	}
	for(int i=0;i<col1;i++)
	{
		In>>num>>nowrow;
		A[nowrow][nowrow]=1;
		C[nowrow]=1;
		for(int j=2;j<=num;j++)
		{
			In>>nowcol;
			A[nowrow][nowcol]=1;
		}
	}
	B=new bool*[col2];
	D=new bool[col2];
	for(int i=0;i<col2;i++)
	{
		B[i]=new bool[row];
		D[i]=1;
		for(int j=0;j<row;j++)
		{
			B[i][j]=0;
		}		
		In>>num;
		for(int j=0;j<=num-1;j++)
		{
			In>>nowcol;
			B[i][nowcol]=1;
		}		
	}	
}

void showA()
{
	cout<<"here is show A"<<endl;
	for(int i=0;i<row;i++)
	{
		if(C[i]==0) continue;
		cout<<i<<":";
		for(int j=0;j<row;j++)
		{
			if(A[i][j]==1) cout<<j<<" ";
		}
		cout<<endl;
	}	
	cout<<endl<<endl;
}

void Forward_elimination()//前向消元
{
	auto start=std::chrono::high_resolution_clock::now();
	for(int I=0;I<row;I++)
	{
		if(C[I]==0)
		{
			for(int i=0;i<col2;i++)
			{
				if(D[i]==0||B[i][I]==0) continue;
				C[I]=1;
				D[i]=0;
				for(int j=I;j<row;j++)
				{
					A[I][j]=B[i][j];
				}				
				break;
			}			
		}
		if(C[I]==0) continue;
		for(int i=0;i<col2;i++)
		{
			if(D[i]==0||B[i][I]==0) continue;
			for(int j=I;j<row;j++)
			{
				B[i][j]=B[i][j]^A[I][j];
			}
		}
	}
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	cout<<"运行时间："<<fixed<<duration.count()<<"μs"<<endl;	
	Out<<fixed<<duration.count()<<",";
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
			if(A[nowrow][j]==1) return 0;
		}
		if(A[nowrow][nowrow]==0) return 0;	
			
		for(int j=nowrow+1;j<row;j++)
		{
			if(A[nowrow][j]==1)
			{
				In>>nowcol;
				if(j!=nowcol) return 0;
			}
		}
	}	
	return 1;
}


int main(int argc, char* argv[]) 
{
	cout<<argv[1]<<" "<<argv[2];
	In.open("./tests/test"+to_string(atoi(argv[1]))+".txt");
	Time=atoi(argv[2]);
	read();
	Forward_elimination();
	if(atoi(argv[1])!=10&&check()==0) cerr<<argv[1]<<"error!";
	if(Time==5) Out<<endl;
	//else cout<<"success!";
	return 0;
}