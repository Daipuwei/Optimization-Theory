#include <iostream>
#include <string>
#include <cstring>
#include <set>
#include <algorithm>
#include <E:\研究生\运筹学\单纯型算法\Fraction.h> 
using namespace std;

const long long SIZE = 100;

/*
	这是实现线性规划标准型下的单纯型算法， 
*/

class Simplex{
	private:
		int m;												//约束条件个数 
		int n;						   		   			   //变量个数 
		Fraction A[SIZE][SIZE];				  			  //约束系数矩阵 
		Fraction b[SIZE];					 			 //常数项矩阵
		Fraction c[SIZE]; 								//目标函数系数
		Fraction cb[SIZE];							   //基向量对应的目标系数 
		Fraction theta[SIZE];			 			  //常数项与换基约束矩阵列向量的相除的结果
		int basic_vector_list[SIZE];				 //变量列表 
		int type;									//记录约束目标类型，-1代表最小化，1代表最大化 
	public:
		//初始化线性规划目标函数与约束条件 
		void Init(){
			//首先初始化相关系数矩阵
			for(int i = 0 ; i < SIZE ; i++){
				for(int j = 0 ; j < SIZE ; j++){
					this->A[i][j].set("0/1");
				}
				this->b[i].set("0/1");
				this->c[i].set("0/1");
			}
			int m,n,type,direction;
			cout<<"------------单纯型算法输入------------------------"<<endl; 
			cout<<"请输入约束条件个数m:"<<endl;
			cin>>m;
			this->m = m;
			cout<<"请输入变量个数n:"<<endl;
			cin>>n;
			this->n = n;
			cout<<"请输入约束条件系数与不等式方向(-1代表<=，0代表=，1代表>=)"<<endl;
			for(int i = 1 ; i <= n ; i++){
				cout<<"\t\tx"<<i; 
			}
			cout<<"\t\t"<<"不等式方向\t\t常数项"<<endl;
			int cnt = 0;	//松弛变量计数器 
			for(int i = 0 ; i < m ; i++){
				cout<<"输入不等式"<<i+1<<":";
				for(int j = 0 ; j < n ; j++){
					string str;
					cin>>str;
					this->A[i][j].set(str);
				}
				//输入不等式方向和常数项 
				string _b;		//常数项 
				cin>>direction>>_b;
				if(direction != 0){
					Fraction relaxation_variable;
					relaxation_variable.set("1/1");	//不等号为<=时，系数置为1 
					if(direction == 1){//不等号为>=时，系数置为-1 
						relaxation_variable.setOpposite();
					}
					this->A[i][n+cnt] = relaxation_variable;
					cnt++;	
				}
				this->b[i].set(_b);
			} 
			cout<<"请输入目标函数优化类型和目标函数系数矩阵(类型值为1代表最大,-1代表最小)"<<endl; 
			cout<<"请输入目标函数优化类型："<<endl; 
			cin>>type;
			this->type = type;
			cout<<"请输入目标函数系数："<<endl;
			for(int i = 1 ; i <= n ; i++){
				cout<<"\tx"<<i;
			}
			cout<<endl;
			for(int i = 0 ; i < this->n ; i++){
				this->c[i].set("0/1");
				this->cb[i].set("0/1");
			}
			for(int i = 0 ; i < n ; i++){
				string str;
				cin>>str;
				this->c[i].set(str);
				if(this->type == -1){// 当目标函数有求最小值时，要进行模型转化，转换为最大值 
					this->c[i].setOpposite();
				}
				this->basic_vector_list[i] = i; 
			}
			//更新当前的变量个数 
			this->n += cnt; 
			//若有不等号出现，需要加入松弛变量，对应目标约束系数置为0 
			for(int i = n ; i < this->n ; i++){
				this->c[i].set("0/1");
				this->cb[i].set("0/1");
				this->basic_vector_list[i] = i; 
			}
		}
		
		//线性规划标准型单纯型算法解法
		bool SimplexSolve(){
			bool ans; 
			//首先将约束系数矩阵分解成基矩阵和非基矩阵，之后找出对应的可行基与非可行基 
			//在这里基矩阵默认变换成单位阵，以方便单纯型算法的相关矩阵计算
			//首先初始化是否为单位向量的标志数组和元素1的下标数组 
			//flag[i]是记录第i列是否为单位向量,index[i]记录第i列若为单位向量时1的下标 
			int flag[this->n],index[this->n];
			//判断是否存在m个线性无关单位向量
			int cnt_unit_vector = this->exsitUnitMatrix(flag,index,this->n);
			cout<<cnt_unit_vector<<endl; 
			if(cnt_unit_vector != this->m){	//未找到m个线性无关单位向量 
				Transform(flag,index,this->n);
			} 
			//初始化典式数组 
			Fraction test_array[this->n];
			for(int i = 0 ; i < this->n ; i++){
				test_array[i].set("0/1");
			}
			this->Print();
			while(1){
				//计算检验数向量(典式)
				this->ComputeTestNumber(test_array,this->n);
				cout<<endl<<"检验数为："<<endl;
				for(int i = 0 ; i < this->n ; i++){
					cout<<test_array[i].result()<<" ";
				}
				cout<<endl;
				//判断典式是否全小于0
				//cout<<"是否全部小于等于0："<<this->isAllLessZero(test_array,this->n)<<endl;
				if(this->isAllLessZero(test_array,this->n)){
					ans = true;
					break;
				}else{
					//求得最大检验数的对应的变量的下标(列下标)，将对应的变量作为换入基变量 
					int max_col = this->Find_Max_TestNumber(test_array,this->n);
					cout<<"需要换入的列:"<<max_col+1<<endl;
					//获得对应的约束矩阵列向量
					Fraction p_index[this->m];
					for(int i = 0 ; i < m ;i++){
						p_index[i] = this->A[i][max_col]; 
					}
					//判断对应约束矩阵列向量是否全小于0
					if(this->isAllLessZero(p_index,this->m)){//全小于0,没有最优解 
						ans = false;
						break;
					}else{//不全小于0，选择最小的变量作为换出变量
						//首先进行将约束矩阵列向量和常数项进行相除得到对应结果
						this->Divide_Array(p_index,this->m);
						//基矩阵由于已经被高斯消元法转化为相乘单位矩阵，因此简化了矩阵运算
						//寻找theta向量大于0的分量中最小的分量 (行下标)
						int r = this->Find_Min_x(this->theta,this->m);
						int index = this->Find_BasicVector_Index(this->basic_vector_list[r]);
						cout<<"需要换出的列:"<<this->basic_vector_list[r]+1<<endl;
						//开始将A[r][max_index]定位主元，进行消元，A[r][max_index]转化为1，
						//该列其余的全部转化为0，常数项和矩阵其他元素做出相应变化。
						this->transform(r,max_col);
						//更改基向量列表
						//int index = this->Find_BasicVector_Index(r);
						this->basic_vector_list[index] = max_col;
						//更新基向量对应的目标系数数组
						this->cb[r] = this->c[max_col]; 
					} 
				}
				this->Print();
			}
			return ans;
		} 
		
		//判断约束矩阵中第col列是否为单位向量 
		int isUnitVector(int col){
			int cnt0 = 0; 			//记录元素为0的个数 
			int index = -1;			//记录元素为1的下标 
			Fraction one;
			one.set("1/1");
			for(int i = 0; i < this->m ; i++){
				//发现元素1 
				if(this->A[i][col].Compare2Fraction(one) == 0){
					index = i;
				}
				//发现元素0 
				if(this->A[i][col].Compare2Zero() == 0){
					cnt0++; 
				}
			}
			//该列存在m-1的元素0，剩下一个元素1，返回元素1的下标 
			if(cnt0 == this->m-1 && index != -1){
				return index;
			}else{//否则返回-1 
				return -1;
			}
		}
		
		//判断是否存在m个线性无关单位矩阵 
		int exsitUnitMatrix(int* flag,int* index,int n){
			int cnt_unit_vector = 0;		//存储单位向量个数
			int cnt = 0;
			for(int i = n-1 ; i >= 0 ; i--){
				//判断第i列是否单位向量 
				int _index = this->isUnitVector(i);
				if(_index == -1){	//不是单位向量 
					flag[i] = 0;
					index[i] = -1; 
				}else{	//是单位向量，存储元素为1的下标与是单位向量的标志 
					flag[i] = 1;
					index[i] = _index;
					//依次记录基向量 
					this->basic_vector_list[cnt++] = i; 
					cnt_unit_vector++;
				}
			}
			sort(basic_vector_list,basic_vector_list+this->m);
			/*for(int i = 0 ; i < this->m ; i++){
				int index = this->basic_vector_list[i];
				this->cb[index] = this->c[index];
			}*/
			return cnt_unit_vector;
		} 
		
		void Transform(int* flag,int* index,int n){
			set<int> s;
			//将已经是单位向量的列向量的1的下标加入集合 
			for(int i = 0 ; i < n ; i++){
				if(flag[i] == 1){
					s.insert(index[i]); 
				}
			}
			int cnt = 0;
			//开始遍历每列，调整出m个性无关单位向量 
			for(int i = this->n-1; i >= 0 && s.size() != this->m; i--){
				if(flag[i] == 1){	//第i列是单位向量 
					this->basic_vector_list[cnt] = i;		//把该向量加入候选基向量列表中
					this->cb[cnt] = this->c[i];				//把相应的目标函数系数加入基向量对应的目标系数数组当中
					cnt++; 
					continue;
				}else{				//第i列不是单位向量
					for(int j = 0 ; j < this->m ; j++){
						if(s.find(j) != s.end()){  //集合中含有下标j，即已经存在元素1在第j个位置的单位向量 
							continue;
						}else{//不存在，那么A[j][i]为主元,把该列调整为单位向量 
							this->transform(j,i);
							s.insert(j);
							this->basic_vector_list[cnt] = i;		//把该向量加入候选基向量列表中
							this->cb[cnt] = this->c[i];				//把相应的目标函数系数加入基向量对应的目标系数数组当中
							cnt++; 
							break;
						}
					}
				}
			} 
		} 
		
		//计算检验数（典式） 
		void ComputeTestNumber(Fraction* test_array,int n){
			for(int i = 0 ; i < this->n ; i++){
				test_array[i] = this->c[i];
				for(int j = 0 ; j < this->m ; j++){
					Fraction tmp;
					tmp = tmp.MUL(this->cb[j],this->A[j][i]);
					test_array[i] = test_array[i].SUB(test_array[i],tmp);
				}
			} 
		}
		
		//这是判断向量是是否全为0的函数 
		int isAllLessZero(Fraction* array,int size){
			int flag = 1;
			for(int i = 0 ; i < size ; i++){
				if(array[i].Compare2Zero() == 1){//检验数大于0 
					flag = 0;
					break;
				}
			}
			return flag;
		}		
		
		//寻找检验数中的最大值
		int Find_Max_TestNumber(Fraction* test_array,int size){
			int index = 0;
			Fraction max;
			max.set("0/1"); 
			for(int i = 0 ; i < size ; i++){
				if(test_array[i].Compare2Zero() <= 0){//不大于0的检验数跳过 
					continue;
				}else{//检验数大于0时
					int flag = test_array[i].Compare2Fraction(max);
					if(flag == 1){//当前检验数比max大，更新max与下标 
						max = test_array[i];
						index = i;
					}
				}
			}
			return index;		//返回下标 
		}
		
		//寻找出最小比值对应的下标 
		int Find_Min_x(Fraction* theta,int size){
			int index = 0;
			Fraction min;
			min.set("9999999/1");			//定义为最正大数 
			for(int i = 0 ; i < size ; i++){
				if(theta[i].Compare2Zero() <= 0){//不大于0的检验数跳过 
					continue;
				}else{//检验数大于0时
					int flag = theta[i].Compare2Fraction(min);
					if(flag == -1){//当前检验数比min小，更新min与下标 
						min = theta[i];
						index = i;
					}	
				}	
			}
			return index;		//返回下标 
		} 
		
		//在每列中发寻找1的下标 
		int FindOne(int col){
			int cnt0 = 0; 
			int index = -1;
			Fraction one;
			one.set("1/1");
			for(int i = 0; i < this->m ; i++){
				//发现元素1 
				if(this->A[i][col].Compare2Fraction(one) == 0){
					index = i;
				}
				//发现元素0 
				if(this->A[i][col].Compare2Zero() == 0){
					cnt0++; 
				}
			}
			//该列存在m-1的元素0，剩下一个元素1，返回元素1的下标 
			if(cnt0 == this->m-1 && index != -1){
				return index;
			}else{//否则返回-1 
				return -1;
			}
		}
		
		//寻找基变量列表中k所在的下标 
		int Find_BasicVector_Index(int k){
			for(int i = 0 ; i < this->m ; i++){
				if(this->basic_vector_list[i] == k){
					return i;
				}
			} 
		}	
				
		// 计算约束矩阵列向量与常数项相除的结果 
		void Divide_Array(Fraction* pk,int size){
			for(int i = 0 ; i < size ; i++){
				//由于Fraction中对于除数为0的结果定义为“-32768/1”
				this->theta[i] = this->theta[i].DIV(this->b[i],pk[i]);
			}
		}
		
		//将A[row][col]定位主元后，将其转化为1，该列其余为转化为0，常数项和其余约束矩阵做相应处理 
		void transform(int row,int col){
			//首先处理row这一行的数据 
			//A[row][col]作为系数 
			Fraction rate = this->A[row][col];
			//处理row这行的常数项b和约束矩阵A 
			this->b[row] = this->b[row].DIV(this->b[row],rate); 
			for(int i = 0 ; i < this->n; i++){
				this->A[row][i] = this->A[row][i].DIV(this->A[row][i],rate);
			} 
			//处理剩余的元素
			for(int i = 0 ; i < this->m; i++){
				if(i == row){//遍历到主元那行跳过 
					continue;
				}
				Fraction rate = this->A[i][col];
				int flag = rate.Compare2Zero();
				if(flag == 0){	//A[i][col]为0，跳过这行 
					continue;
				}else{	//A[i][col]不为0，rate=A[i][col]
					Fraction tmp; 
					//对第i行进行行变换 
					for(int j = 0 ; j < this->n ; j++){
						tmp = tmp.MUL(this->A[row][j],rate);
						this->A[i][j] = this->A[i][j].SUB(this->A[i][j],tmp);
					}
					//对常数项进行变换 
					tmp = tmp.MUL(this->b[row],rate);
					this->b[i] = this->b[i].SUB(this->b[i],tmp);
				}
			} 
		}
		
		//在A[row][col]所在列搜索非负元素 
		int Search_Col(int row,int col){
			int index = -1;		//index用来存储寻找的元素的行坐标，-1代表没有找到 
			//从最后一行开始搜索，将当前行放到最后面，防止之后多次交换操作 
			for(int i = this->m-1 ; i > row ; i--){
				if(this->A[i][col].Compare2Zero() != 0){   //找到不为0的元素 
					index = i;
					break;
				}
			}
			return index;
		} 
		
		//在A[row][col]所在行搜索非负元素 
		int Search_Row(int row,int col){
			int index = -1;		//index用来存储寻找的元素的列坐标，-1代表没有找到 
			//从最后一列开始搜索，将当前列放到最后面，防止之后多次交换操作 
			for(int i = this->n-1 ; i > col ; i--){
				if(this->A[row][i].Compare2Zero() != 0){   //找到不为0的元素 
					index = i;
					break;
				}
			}
			return index;
		} 
		
		//按行交换 
		void Swap_Row(int row1,int row2){
			//将下标为row1和row2进行交换 
			for(int i = 0 ; i < this->n ; i++){
				Fraction tmp = this->A[row1][i];
				this->A[row1][i] = this->A[row2][i];
				this->A[row2][i] = tmp;
			}
			//对常数项数组对应元素进行调整
			Fraction tmp = this->b[row1];
			this->b[row1] = this->b[row2];
			this->b[row2] = tmp; 
		}
		
		//两列交换 
		void Swap_Col(int col1,int col2){
			//交换约束系数矩阵第col1列和第col2列 
			for(int i = 0 ; i < this->m ; i++){
				Fraction tmp = this->A[i][col1];
				this->A[i][col1] = this->A[i][col2];
				this->A[i][col2] = tmp;
			}
			//将目标目标系数进行互换 
			Fraction tmp = this->c[col1];
			this->c[col1] = this->c[col2];
			this->c[col2] = tmp;
			//变量列表交换
			int tmp1 = this->basic_vector_list[col1];
			this->basic_vector_list[col1] = this->basic_vector_list[col2];
			this->basic_vector_list[col2] = tmp1; 
		}
		
		//计算最优解 
		Fraction getopt(){
			//初始化最优解向量为0 
			Fraction x[this->n];
			for(int i = 0 ; i < this->n ; i++){
				Fraction tmp;
				tmp.set("0/1");
				x[i] = tmp;
			}
			cout<<"基向量为:"<<endl; 
			for(int i = 0 ; i < this->m ; i++){
				cout<<"x"<<this->basic_vector_list[i]+1<<" "; 
			}
			cout<<endl;
			//获取最优解向量 
			for(int i = 0 ; i < this->m ; i++){
				//获得基变量下标 
				int j = this->basic_vector_list[i];
				//基变量赋值 
				x[j] = this->b[i]; 
			}
			cout<<"最优解向量为:"<<endl; 
			for(int i = 0 ; i < this->n ; i ++){
				cout<<x[i].result()<<" ";
			}
			cout<<endl;
			Fraction opt;//最优解 
			opt.set("0/1");	
			//计算最优值						
			for(int i = 0 ; i < this->n ; i++){
				Fraction tmp;
				tmp= tmp.MUL(this->c[i],x[i]);
				opt = opt.ADD(opt,tmp);
			}
			//若目标是最小化，由于但数单纯型算法是解标准型，
			//因此需要在对结果取相反数 
			if(this->type == -1){
				opt.setOpposite();
			} 
			cout<<"最优解为："<<opt.result()<<endl;
			return  opt; 
		}
		
		//打印函数 
		void Print(){
			cout<<"约束矩阵为："<<endl;
			for(int i = 0 ; i < this->m ; i++){
				for(int j = 0 ; j < this->n ; j++){
					cout<<this->A[i][j].result()<<" ";
				}
				cout<<endl;
			}
			cout<<"常数项为："<<endl;
			for(int i = 0 ; i < this->n ; i++){
				cout<<this->b[i].result()<<" ";
			} 
			cout<<endl;
			cout<<"目标系数（c）为："<<endl;
			for(int i = 0 ; i < this->n ; i++){
				cout<<this->c[i].result()<<" ";
			} 
			cout<<endl;
			cout<<"目标系数（cb）为："<<endl;
			for(int i = 0 ; i < this->m ; i++){
				cout<<this->cb[i].result()<<" ";
			}
			cout<<endl<<"基向量组:"<<endl;
			for(int i = 0 ; i < this->m ; i++){
				cout<<"x"<<this->basic_vector_list[i]+1<<" ";
			}
			cout<<endl;
		} 
}; 

int main()
{
	Simplex simplex; 
	while(1){
		simplex.Init();
		bool flag = simplex.SimplexSolve();
		if(flag){
			Fraction ans = simplex.getopt(); 	
		}else{
			cout<<"无最优解"<<endl; 
		}	
	}
	
	return 0;
 } 
