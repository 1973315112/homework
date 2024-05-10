#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

unsigned short** A;
unsigned short** B;
bool* C;
bool* D;
int num,row,col1,col2,comrow,nowcol,nowrow,nownum,Time;
ifstream In;
ofstream Out("normal.csv");


void read()
{
	In>>row>>col1>>col2;
	comrow=(row>>4)+1;
	A=new unsigned short*[row];
	C=new bool[row];
	for(int i=0;i<row;i++)
	{
		A[i]=new unsigned short[comrow+20];
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
	}
	B=new unsigned short*[col2];
	D=new bool[col2];
	for(int i=0;i<col2;i++)
	{
		B[i]=new unsigned short[comrow+20];
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
	
}

void Forward_elimination()//前向消元
{
	auto start=std::chrono::high_resolution_clock::now();
	for(int I=0;I<row;I++)
	{
		nowrow=(I>>4);
		nownum=(1<<(15-(I&15)));
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
		for(int i=0;i<col2;i++)
		{
			if(D[i]==0) continue;
			if((B[i][nowrow]&nownum)==0) continue;
   			for (int j=nowrow;j<=comrow;j++) 
			{
        		B[i][j]=A[I][j]^B[i][j];
    		}
		}
	}	
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	cout<<"运行时间："<<fixed<<duration.count()<<"μs"<<endl;	
	Out<<fixed<<duration.count()<<endl;
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

int main() 
{
	Out<<"test,i\n";
	for(int i=1;i<=9;i++)
	{
		cout<<i<<endl;
		Out<<"test"<<i<<",";
		In.open("./tests/test"+to_string(i)+".txt");
		read();
		Forward_elimination();
		if(i!=10&&check()==0) cerr<<i<<"error!";
		In.close();		
	}
	return 0;
}
