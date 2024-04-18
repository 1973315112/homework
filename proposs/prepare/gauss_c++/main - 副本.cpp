#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

int N=4;
float C[1100][1100];
int pivotcol,y;
float pivot,k;
ifstream In;

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
		for(int i=I+1;i<=N;i++)
		{
			k=C[i][I];
			C[i][I]=0;
			for(int j=I+1;j<=N+1;j++)
			{
				C[i][j]=C[i][j]-k*C[I][j];
			}
		}
		show();
	}	
}

void backband_solving()//回带求解
{
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
	/*
	C[1][1]=1854; C[1][2]=620;  C[1][3]=1154; C[1][4]=909;  C[1][5]=1205490;
	C[2][1]=3124; C[2][2]=1058; C[2][3]=1934; C[2][4]=1537; C[2][5]=2036626;
	C[3][1]=242;  C[3][2]=74;   C[3][3]=156;  C[3][4]=116;  C[3][5]=154584;
	C[4][1]=1784; C[4][2]=602;  C[4][3]=1106; C[4][4]=877;  C[4][5]=1162182;
	*/
	In.open("data1");
	In>>N;
	for(int i=1;i<=N;i++)
	{
		for(int j=1;j<=N+1;j++)
		{
			In>>C[i][j];
		}
	}
	In.close();
	
	
	//show();
	auto start=std::chrono::high_resolution_clock::now();
	Forward_elimination();
	//show();
	backband_solving();
	//show();	
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	cout<<"运行时间："<<fixed<<duration.count()<<"μs"<<endl;
	
	
	In.open("ans1");
	for(int i=1;i<=N;i++)
	{
		In>>pivot;
		if(abs(pivot-C[i][N+1])<=1)
		{
			cerr<<"error";
			return 0;
		}
	}
	cout<<"Success";	
	return 0;
}