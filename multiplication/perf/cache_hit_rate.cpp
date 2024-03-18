#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>

using namespace std;

void Ini();
void cache(int n);

const int N=10005;
int A[N],B[N][N],sum[N];

int main(int argc, char* argv[]) 
{
	    if (argc != 2) {  
        cerr << "error" << endl;
		cerr << "error" << endl; 
		cerr << "error" << endl;   
        return 1; 
    }  
    
	Ini();
	int n = stoi(argv[1]); 
	cache(n);
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


void cache(int n)
{
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
}

