#include <iostream>

using namespace std;

 
char In1[100]="00010010000101010011010100100100",In2[100]="11000000100010010101111010000001";
int operator1[100],operator2[100];
long long op1,op2,ANS;
int addnum[50][100];
int sum[30][100];

void Ini(int* A,char* B);
void negation(int *A);
long long char32_to_decimal(char* A);
void creataddnum();
void change(int* A,int n);
void left(int* A,int* B,int n);
void mul_neg1(int *A,int n);
void merge(int* A,int* B,int* C,int* Cout,int* S);
void add64(int* A,int* B,int* S);
void source(int* A);
void show(int* A,int n);
long long int64_to_decimal(int* A);

int main() 
{
	op1=char32_to_decimal(In1);
	op2=char32_to_decimal(In2);
	cout<<"本轮计算的被乘数为:"<<In1<<" 对应的有符号整数为："<<op1<<endl;
	cout<<"本轮计算的乘数为  :"<<In2<<" 对应的有符号整数为："<<op2<<endl;	
	cout<<"现在将输入转化为补码形式，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	Ini(operator1,In1);
	Ini(operator2,In2);
	cout<<"现在生成16个加数，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	creataddnum();
	cout<<"现在进行第一层华莱士树计算，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	merge(addnum[1],addnum[2],addnum[3],sum[1],sum[2]);
	merge(addnum[4],addnum[5],addnum[6],sum[3],sum[4]);
	merge(addnum[7],addnum[8],addnum[9],sum[5],sum[6]);
	merge(addnum[10],addnum[11],addnum[12],sum[7],sum[8]);
	merge(addnum[13],addnum[14],addnum[15],sum[9],sum[10]);//sum[1-10],addnum[1]
	cout<<"现在进行第二层华莱士树计算，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	merge(addnum[16],sum[1],sum[2],sum[11],sum[12]);
	merge(sum[3],sum[4],sum[5],sum[13],sum[14]);
	merge(sum[6],sum[7],sum[8],sum[15],sum[16]);//sum[9-16]
	cout<<"现在进行第三层华莱士树计算，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	merge(sum[9],sum[10],sum[11],sum[17],sum[18]);
	merge(sum[12],sum[13],sum[14],sum[19],sum[20]);//sum[15-20]
	cout<<"现在进行第四层华莱士树计算，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	merge(sum[15],sum[16],sum[17],sum[21],sum[22]);
	merge(sum[18],sum[19],sum[20],sum[23],sum[24]);//sum[21-24]
	cout<<"现在进行第五层华莱士树计算，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	merge(sum[21],sum[22],sum[23],sum[25],sum[26]);//sum[24-26]	
	cout<<"现在进行第六层华莱士树计算，所有操作数不相互依赖，消耗1个时钟周期:"<<endl;
	merge(sum[24],sum[25],sum[26],sum[27],sum[28]);//sum[27-28]
	cout<<"现在进行最后的合并计算，采用普通全加器，消耗32个时钟周期:"<<endl;
	add64(sum[27],sum[28],sum[29]);
	source(sum[29]);
	ANS=int64_to_decimal(sum[29]);
	cout<<"答案为:";
	show(sum[29],63);
	cout<<" 对应的有符号整数为："<<ANS<<endl;
	cout<<"现在进行正确性验证:";
	if(op1*op2==ANS) cout<<"结果正确";
	else cout<<"结果错误";
		
	return 0;
}

void Ini(int* A,char* B) 
{
	for(int i=0;i<=31;i++)
	{
		A[i]=B[31-i]-'0';
	}	
	if(B[0]=='0') return;
	negation(A);
}

void negation(int *A)
{
	A[0]=!A[0]+1;
	for(int i=1;i<=31;i++)
	{
		A[i]=!A[i]+A[i-1]/2;
		A[i-1]=A[i-1]%2;
	}
	A[31]=1;
}

long long char32_to_decimal(char* A)
{
	long long ans=0;
	for(int i=1;i<=31;i++)
	{
		ans=ans*2+A[i]-'0';
	}
	if(A[0]=='1') ans=-ans;
	return ans;
}

void creataddnum()
{
	for(int i=0;i<=31;i++)
	{
		addnum[1][i]=operator1[i];
	}
	for(int i=32;i<=63;i++)
	{
		addnum[1][i]=operator1[31];
	}
	for(int i=2;i<=16;i++)
	{
		left(addnum[i],addnum[i-1],2);
	}
	
	int num=operator2[1]*4+operator2[0]*2;
	change(addnum[1],num);
	for(int i=2;i<=16;i++)
	{
		num=operator2[2*i-1]*4+operator2[2*i-2]*2+operator2[2*i-3];
		change(addnum[i],num);
	}
}

void change(int* A,int n)
{
	if(n==0||n==7) n=0;
	else if(n==1||n==2) n=1; 
	else if(n==3) n=2; 
	else if(n==4) n=-2;
	else if(n==5||n==6) n=-1; 
	if(n==0)
	{
		for(int i=0;i<=63;i++)
		{
			A[i]=0;
		}
	}
	if(n==2||n==-2) left(A,A,1);
	if(n==-1||n==-2) mul_neg1(A,64);
}

void left(int* A,int* B,int n)
{
	for(int i=63;i>=n;i--)
	{
		A[i]=B[i-n];
	}
	for(int i=n-1;i>=0;i--)
	{
		A[i]=0;
	}	
}

void mul_neg1(int *A,int n)
{
	if(A[n-1]==0)
	{
		A[0]=!A[0]+1;
		for(int i=1;i<=n-1;i++)
		{
			A[i]=!A[i]+A[i-1]/2;
			A[i-1]=A[i-1]%2;
		}
		A[n-1]=1;
	}
	else
	{
		//cout<<"here3"<<endl;
		A[0]=A[0]-1;
		for(int i=0;i<=n-2;i++)
		{
			if(A[i]==-1)
			{
				A[i]=1;
				A[i+1]--;
			}
			A[i]=!A[i];
		}
		A[n-1]=0;
	}
}

void add1(int a,int b,int Cin,int& Cout,int& s)
{
	//cout<<"begin add1"<<(a+b+cin)%2;
	Cout=(a+b+Cin)/2;
	//cout<<"COUT";
	s=(a+b+Cin)%2;
}

void merge(int* A,int* B,int* C,int* Cout,int* S)
{
	Cout[0]=0;
	for(int i=0;i<=63;i++)
	{
		add1(A[i],B[i],C[i],Cout[i+1],S[i]);
	}
}

void add64(int* A,int* B,int* S)
{
	int Cout=0;//,in=0;
	for(int i=0;i<=63;i++)
	{
		//cout<<"i:"<<i;
		//cout<<A[i]<<" "<<B[i]<<" "<<Cout<<" ";
		add1(A[i],B[i],Cout,Cout,S[i]);
		//cout<<Cout<<" "<<S[i]<<endl;
		//in=Cout;
	}
	//cout<<endl;
}

void source(int* A)
{
	if(A[63]==0) return;
	A[0]--;
	for(int i=0;i<=62;i++)
	{
		if(A[i]==-1)
		{
			A[i]=1;
			A[i+1]--;
		}
		A[i]=!A[i];
	}
	A[63]=1;
}

void show(int* A,int n)
{
	for(int i=n-1;i>=0;i--)
	{
		cout<<A[i];
	}
}

long long int64_to_decimal(int* A)
{
	long long ans=0;
	for(int i=62;i>=0;i--)
	{
		ans=ans*2+A[i];
	}
	if(A[63]==1) ans=-ans;
	return ans;
}