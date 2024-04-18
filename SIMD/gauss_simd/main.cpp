#include <iostream>
#include <fstream>
#include <chrono>
#include <arm_neon.h>

using namespace std;

int N = 4;
float C[5100][5100];
float C_j[10],C_n1[10];
int pivotcol, y;
float pivot,k;
float32x4_t K,C_I,C_i,kC_I,C_J,C_N1,C_JN1;
ifstream In;
ofstream Out("data.csv");
double diff,diffrate;

float32x4_t K_ali,C_I_ali,C_i_ali,kC_I_ali,C_J_ali,C_N1_ali,C_JN1_ali;

void open(string i)
{
	In.open("data"+i+".txt");
	In>>N;
	for(int i=1;i<=N;i++)
	{
		for(int j=1;j<=N+1;j++)
		{
			In>>C[i][j];
		}
	}
	In.close();	
}

void show() 
{
    for (int i = 1; i <= N; i++) 
	{
        for (int j = 1; j <= N + 1; j++) 
		{
            cout << C[i][j] << " ";
        }
        cout << endl;
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
		//show();
		
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

void Forward_elimination_NEON() 
{
    for (int I = 1; I <= N; I++) 
	{
        // 主元选择及交换
        
        pivot = abs(C[I][I]);
        pivotcol = I;
        for (int i = I; i <= N; i++) 
		{
            if (pivotcol < abs(C[i][I])) 
			{
                pivotcol = i;
                pivot = abs(C[i][I]);
            }
        }
        pivot = C[pivotcol][I];
        for (int j = I; j <= N + 1; j++) 
		{
            swap(C[pivotcol][j], C[I][j]);
            C[I][j] = C[I][j] / pivot;
        }

        // 行加减运算（NEON版本）
        for(int i=I+1;i<=N;i++) 
		{
			K=vdupq_n_f32(C[i][I]);
			C[i][I]=0;
            for(int j=I+1;j<=N+5;j+=4)
			{
                C_I=vld1q_f32(&C[I][j]);
                C_i=vld1q_f32(&C[i][j]);
                kC_I=vmulq_f32(K,C_I);
                C_i=vsubq_f32(C_i,kC_I);
                vst1q_f32(&C[i][j],C_i);
            }         
        }
    }
}

void backband_solving_NEON() 
{
    for (int I=N;I>=1;I--) 
	{
        for (int i=I-4;i>=1;i=i-4) 
		{
			for(int j=0;j<=3;j++)
			{
				C_j[j]=C[i+j][I];
				C_n1[j]=C[i+j][N+1];
			}
			K=vdupq_n_f32(C[I][N+1]);//复制第I行解(系数K)
			C_N1=vld1q_f32(&C_n1[0]);//复制第N+1列的值
			C_I=vld1q_f32(&C_j[0]);//复制第I列的值
			kC_I=vmulq_f32(K,C_I);//将第N+1列与第I列相乘
            //float32x4_t C_I_N1_vec = vdupq_n_f32(C[I][N + 1]);
            //float32x4_t C_i_I_vec = vdupq_n_f32(C[i][I]);
            //float32x4_t C_i_N1_vec = vld1q_f32(&C[i][N + 1]);
            //float32x4_t result = vmulq_f32(C_I_N1_vec, C_i_I_vec);
            C_N1=vsubq_f32(C_N1,kC_I);//将第N+1列减去（第N+1列与第I列相乘）
            vst1q_f32(&C_n1[0],C_N1);
            
            for(int j=0;j<=3;j++)
			{
				C[i+j][I]=0;
				C[i+j][N+1]=C_n1[j];
			}
        }
        //边界处理
        y=1;
        while(C[y][I]!=0&&y<I)
        {
        	C[y][N+1]=C[y][N+1]-C[I][N+1]*C[y][I];
			C[y][I]=0;
			y++;
		}
    }
}

void Forward_elimination_ALI() 
{
    for (int I = 1; I <= N; I++) 
	{
        // 主元选择及交换
        
        pivot = abs(C[I][I]);
        pivotcol = I;
        for (int i = I; i <= N; i++) 
		{
            if (pivotcol < abs(C[i][I])) 
			{
                pivotcol = i;
                pivot = abs(C[i][I]);
            }
        }
        pivot = C[pivotcol][I];
        for (int j = I; j <= N + 1; j++) 
		{
            swap(C[pivotcol][j], C[I][j]);
            C[I][j] = C[I][j] / pivot;
        }

        // 行加减运算（ALI版本）
        //float32x4_t k = vdupq_n_f32(C[I][I]);
        for(int i=I+1;i<=N;i++) 
		{
			K_ali=vdupq_n_f32(C[i][I]);
			C[i][I]=0;
            for(int j=I+1;j<=N+5;j+=4)
			{
                C_I_ali=vld1q_f32(&C[I][j]);
                C_i_ali=vld1q_f32(&C[i][j]);
                kC_I_ali=vmulq_f32(K_ali,C_I_ali);
                C_i_ali=vsubq_f32(C_i_ali,kC_I_ali);
                vst1q_f32(&C[i][j],C_i_ali);
            }        
        }
    }
}

void backband_solving_ALI() 
{
    for (int I=N;I>=1;I--) 
	{
        for (int i=I-5;i>=1;i=i-4) 
		{
			for(int j=0;j<=3;j++)
			{
				C_j[j]=C[i+j][I];
				C_n1[j]=C[i+j][N+1];
			}
			K_ali=vdupq_n_f32(C[I][N+1]);//复制第I行解(系数K)
			C_N1_ali=vld1q_f32(&C_n1[0]);//复制第N+1列的值
			C_I_ali=vld1q_f32(&C_j[0]);//复制第I列的值
			kC_I_ali=vmulq_f32(K_ali,C_I_ali);//将第N+1列与第I列相乘
            C_N1_ali=vsubq_f32(C_N1_ali,kC_I_ali);//将第N+1列减去（第N+1列与第I列相乘）
            vst1q_f32(&C_n1[0],C_N1_ali);
            
            for(int j=0;j<=3;j++)
			{
				C[i+j][I]=0;
				C[i+j][N+1]=C_n1[j];
			}
        }
        y=1;
        while(C[y][I]!=0&&y<I)
        {
        	C[y][N+1]=C[y][N+1]-C[I][N+1]*C[y][I];
			C[y][I]=0;
			y++;
		}
    }
}

int main() 
{
	auto start=std::chrono::high_resolution_clock::now();
	auto end=std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> duration = end - start;
	/*
	C[1][1]=1854; C[1][2]=620;  C[1][3]=1154; C[1][4]=909;  C[1][5]=1205490;
	C[2][1]=3124; C[2][2]=1058; C[2][3]=1934; C[2][4]=1537; C[2][5]=2036626;
	C[3][1]=242;  C[3][2]=74;   C[3][3]=156;  C[3][4]=116;  C[3][5]=154584;
	C[4][1]=1784; C[4][2]=602;  C[4][3]=1106; C[4][4]=877;  C[4][5]=1162182;
	*/
	Out<<"n,SFE,SBS,PMFE,PMBS,PAFE,PABS"<<endl;
	for(int i=0;i<=4;i++)
	{
		string I=to_string(i);
		open(I);
		Out<<N<<",";
		cout<<"N:"<<N<<endl;
		start=std::chrono::high_resolution_clock::now();
		Forward_elimination();
		end=std::chrono::high_resolution_clock::now();
		duration = end - start;
		cout<<"SFE运行时间："<<fixed<<duration.count()<<"μs"<<endl;
		Out<<fixed<<duration.count()<<",";
		
		start=std::chrono::high_resolution_clock::now();
		backband_solving();
		end=std::chrono::high_resolution_clock::now();
		duration = end - start;
		cout<<"SBS运行时间："<<fixed<<duration.count()<<"μs"<<endl;
		Out<<fixed<<duration.count()<<",";
				
		open(I);	
		start=std::chrono::high_resolution_clock::now();
		Forward_elimination_NEON();
		end=std::chrono::high_resolution_clock::now();
		duration = end - start;	
		cout<<"PMFE运行时间："<<fixed<<duration.count()<<"μs"<<endl;
		Out<<fixed<<duration.count()<<",";
				
		start=std::chrono::high_resolution_clock::now();
		backband_solving_NEON();
		end=std::chrono::high_resolution_clock::now();
		duration = end - start;
		cout<<"PMBS运行时间："<<fixed<<duration.count()<<"μs"<<endl;
		Out<<fixed<<duration.count()<<",";
				
		open(I);	
		start=std::chrono::high_resolution_clock::now();
		Forward_elimination_ALI();
		end=std::chrono::high_resolution_clock::now();
		duration = end - start;	
		cout<<"PAFE运行时间："<<fixed<<duration.count()<<"μs"<<endl;
		Out<<fixed<<duration.count()<<",";
				
		start=std::chrono::high_resolution_clock::now();
		backband_solving_ALI();
		end=std::chrono::high_resolution_clock::now();
		duration = end - start;
		cout<<"PABS运行时间："<<fixed<<duration.count()<<"μs"<<endl;
		Out<<fixed<<duration.count()<<endl;
						
	}

	//show();
	/*In.open("ans1.txt");
	for(int i=1;i<=N;i++)
	{
		In>>pivot;
		diff=max(diff,(double)abs(pivot-C[i][N+1]));
		diffrate=max(diffrate,(double)abs(pivot-C[i][N+1])/pivot);
	}
	cout<<"diff:"<<diff<<" diffrate:"<<diffrate<<endl;*/
	return 0;
}