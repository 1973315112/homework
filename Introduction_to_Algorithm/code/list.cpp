#include <iostream>
#include <fstream>
#include <chrono>
#include <list>

using namespace std;

list<int>* A;
list<int>* B;
bool* C;
bool* D;
int num,row,col1,col2,comrow,nowcol,nowrow,nownum,Time;
ifstream In;
ofstream Out("list.csv", std::ios::app); // 创建或打开 CSV 文件以追加模式 

void read()
{
	In>>row>>col1>>col2;
	A=new list<int>[row];
	C=new bool[row];
	for(int i=0;i<row;i++)
	{
		C[i]=0;
	}
	for(int i=0;i<col1;i++)
	{
		In>>num>>nowrow;
		A[nowrow].push_back(nowrow);
		C[nowrow]=1;
		for(int j=2;j<=num;j++)
		{
			In>>nowcol;
			A[nowrow].push_back(nowcol);
		}
	}
	B=new list<int>[col2];
	D=new bool[col2];
	for(int i=0;i<col2;i++)
	{
		D[i]=1;	
		In>>num;
		for(int j=0;j<=num-1;j++)
		{
			In>>nowcol;
			B[i].push_back(nowcol);
		}		
	}
}

void showA()
{
	cout<<"here id show A"<<endl;
	for(int i=0;i<row;i++)
	{
		if(C[i]==0) continue;
		auto it = A[i].begin();
		while(it!=A[i].end())
		{
			cout<<*it<<" ";
			it++;
		}
		cout<<endl;
	}
}

void showB()
{
	cout<<"here id show B"<<endl;
	for(int i=0;i<col2;i++)
	{
		if(C[i]==0) continue;
		auto it = B[i].begin();
		while(it!=B[i].end())
		{
			cout<<*it<<" ";
			it++;
		}
		cout<<endl;
	}
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
				if(D[i]==0) continue;
				if(B[i].empty()==1||B[i].front()!=I) continue;
				C[I]=1;
				D[i]=0;
        		swap(A[I],B[i]);
				break;
			}			
		}
		if(C[I]==0) continue;
		for(int i=0;i<col2;i++)
		{
			if(D[i]==0) continue;
			if(B[i].empty()==1||B[i].front()!=I) continue;
			auto a=A[I].begin();
			auto b=B[i].begin();
			while(a!=A[I].end()&&b!=B[i].end())
			{
				if(*a==*b)
				{
					b=B[i].erase(b);
					a++;
				} 
				else if(*a<*b)
				{
					b=B[i].insert(b,*a);
					b++;
					a++;
				}
				else if(*a>*b) b++;
			}
			while(a!=A[I].end())
			{
				if(*a==*b)
				{
					b=B[i].erase(b);
					a++;
				} 
				else if(*a!=*b)
				{
					b=B[i].insert(b,*a);
					b++;
					a++;
				}
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
		auto a=A[nowrow].begin();
		if(*a!=nowrow) return 0;
		a++;		
		for(int j=2;j<=num;j++,a++)
		{
			In>>nowcol;
			if(*a!=nowcol)	return 0;
		}
	}	
	return 1;
}

void del()
{
	In.close();
	Out.close();
	delete[] A;	
	delete[] B;		
	delete[] C;	
	delete[] D;	
}

int main(int argc, char* argv[]) 
{
	cout<<argv[1]<<" "<<argv[2]<<endl;
	In.open("./tests/test"+to_string(atoi(argv[1]))+".txt");
	Time=atoi(argv[2]);
	read();
	Forward_elimination();
	if(atoi(argv[1])!=10&&check()==0) cerr<<argv[1]<<"error!\n";
	if(Time==5) Out<<endl;
  del(); 
	//else cout<<"success!";
	return 0;
}