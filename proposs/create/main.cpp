#include <iostream>
#include <fstream> 

using namespace std;

int m[5000][5000];
int n,t,x,y;
ofstream Out;
string a;

int main()
{
	cin>>a>>n;
	Out.open("ans"+a+".txt");
	for(int i=0;i<n;i++)
	{
		m[i][i]=1.0;
		m[i][n]=rand()%1000;
		Out<<m[i][n]<<" ";
	}
	Out.close();
	/*for(int i=0;i<n;i++)
	{
		for(int j=0;j<=n;j++)
		{
			cout<<m[i][j]<<" ";
		}
		cout<<endl;
	}*/
	t=10*n;
	while(t--)
	{
		x=rand()%n;
		y=rand()%n;
		for(int i=0;i<=n;i++)
		{
			m[x][i]+=m[y][i];
		}
	}	
	t=10*n;
	while(t--)
	{
		x=rand()%n;
		y=rand()%n;
		for(int i=0;i<=n;i++)
		{
			swap(m[x][i],m[y][i]);
		}
	}
	Out.open("data"+a+".txt");
	Out<<n<<endl;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<=n;j++)
		{
			//cout<<m[i][j]<<" ";
			Out<<m[i][j]<<" ";
		}
		//cout<<endl;
		Out<<endl;
	}
	Out.close();
	return 0;
}