#include <iostream>
#include <string>
#include <cstring>
#include <set>
#include <algorithm>
#include <E:\研究生\运筹学\两阶段法\Fraction.h>
using namespace std;

const long long SIZE = 100;

class Two_Stages_Simplex{
	private: 
		Fraction stage_one_c[SIZE]; 						 //阶段一目标函数系数 
		Fraction stage_two_c[SIZE]; 						//阶段二目标函数系数
		Fraction c[SIZE]; 						 			//阶段一目标函数系数 
		int m;												//约束条件个数 
		int n;						   		   			   //变量个数 
		Fraction A[SIZE][SIZE];				  			  //约束系数矩阵 
		Fraction b[SIZE];					 			 //常数项矩阵
		Fraction cb[SIZE];							   //基向量对应的目标系数 
		Fraction theta[SIZE];			 			  //常数项与换基约束矩阵列向量的相除的结果
		int basic_vector_list[SIZE];				 //变量列表 
		int type;									//记录约束目标类型，-1代表最小化，1代表最大化
		int isuse[SIZE];							//人工变量是否使用标志，用来限制第一和第二阶段的人工变量
		set<int> artificai_variable_set;			//人工变量集合 
	public:
		//初始化线性规划目标函数与约束条件 
		void Init(){
			//首先初始化相关系数矩阵
			for(int i = 0 ; i < SIZE ; i++){
				for(int j = 0 ; j < SIZE ; j++){
					this->A[i][j].set("0/1");
				}
				this->b[i].set("0/1");
				this->stage_one_c[i].set("0/1");
				this->stage_two_c[i].set("0/1");
				this->isuse[i] = 1; 
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
			int cnt = 0;	//松弛变量和人工变量计数器 
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
				this->b[i].set(_b);		//常数项初始化
				Fraction relaxation_variable;
				relaxation_variable.set("1/1");	//不等号为<=时，系数置为1
				if(direction == 0){//不等号为=时 
					Fraction artificial_variable,artificial_c;
					artificial_variable.set("1/1");
					artificial_c.set("-1/1");
					this->A[i][this->n+cnt] = artificial_variable;//引入人工变量 
					this->stage_one_c[this->n+cnt] = artificial_c;//人工变量的第一阶段的目标系数置为-1
					this->artificai_variable_set.insert(this->n+cnt);		//人工变量的下标加入集合(下边是从0开始) 
					cnt++;
				}else if(direction == 1){//不等号为>=时
					if(this->b[i].Compare2Zero() == -1){//常数项为负数时 
						this->b[i].setOpposite();		//常数项取反
						for(int j = 0 ; j < this->n; j++){//该行系数取反 
							this->A[i][j].setOpposite();
						}
						this->A[i][this->n+cnt] = relaxation_variable;	//引入松弛变量 
						cnt++;
					}else{	//常数项不小于0时 
						relaxation_variable.setOpposite();	//松弛变量系数置为-1
						this->A[i][this->n+cnt] = relaxation_variable;	//引入松弛变量 
						cnt++;	
						//this->loose_variable_cnt++;
						Fraction artificial_variable,artificial_c;
						artificial_variable.set("1/1");		//引入人工变量 
						artificial_c.set("-1/1");			//人工变量对应的目标系数 
						this->A[i][this->n+cnt] = artificial_variable;//引入人工变量 
						this->stage_one_c[this->n+cnt] = artificial_c;//人工变量的第一阶段的目标系数置为-1
						this->artificai_variable_set.insert(this->n+cnt);//人工变量的下标加入集合(下边是从0开始) 
						cnt++;
					}
				}else{//不等号为<=时 
					this->A[i][this->n+cnt] = relaxation_variable;	//引入松弛变量 
					cnt++;
				}
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
				this->stage_one_c[i].set("0/1");
				this->stage_two_c[i].set("0/1");
				this->cb[i].set("0/1");
			}
			for(int i = 0 ; i < n ; i++){
				string str;
				cin>>str;
				this->stage_two_c[i].set(str);
				if(this->type == -1){// 当目标函数有求最小值时，要进行模型转化，转换为最大值 
					this->stage_two_c[i].setOpposite();
				}
				this->basic_vector_list[i] = i; 
			}
			//更新当前的变量个数 
			this->n += cnt; 
			//若有不等号出现，需要加入松弛变量，对应目标约束系数置为0 
			for(int i = n ; i < this->n ; i++){
				this->cb[i].set("0/1");
				this->basic_vector_list[i] = i; 
			}
		}
		
		bool Two_Stage_Simplex_Solve(){
			//第一阶段的结果 
			cout<<"两阶段算法的第一阶段："<<endl;
			bool ans = this->first_stage_solve();
			if(ans){//为真则继续进行第二阶段 
				cout<<"两阶段算法的第二阶段："<<endl;
				ans = this->second_stage_solve(); 
			}
			return ans; 
		}
		
		//两阶段算法的第一阶段 
		bool first_stage_solve(){
			//flag[i]是记录第i列是否为单位向量,index[i]记录第i列若为单位向量时1的下标 	
			int flag[this->n],index[this->n];
			//首先对目标系数进行赋值 
			for(int i = 0 ; i < this->n ; i++){
				this->c[i] = this->stage_one_c[i];
			} 
			//寻找m个单位列向量
			this->FindUnitVector(flag,index,n);
			return this->SimplexSolve();		//执行第一阶段单纯行算法 
		} 
		
		//两阶段算法的第二阶段
		bool second_stage_solve(){ 
			//将人工变量设置成不可用 
			Fraction one;
			one.set("-1/1");
			for(int i = 0 ; i < this->n ; i++){
				if(this->stage_one_c[i].Compare2Fraction(one) == 0){//是人工变量 
					this->isuse[i] = 0;
				}
			}
			//更新cb 
			for(int i = 0 ; i < this->m ; i++){
				this->cb[i] = this->stage_two_c[this->basic_vector_list[i]];
			}
			//更新c
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){//第一阶段的人工变量在第二阶段舍弃 
					continue;
				}
				this->c[i] = this->stage_two_c[i]; 
			}
			return this->SimplexSolve();		//进行第二阶段的单纯型算法 
		} 
		
		//寻找单位列向量并对基向量列表、cb进行赋值 
		void FindUnitVector(int* flag,int* index,int n){
			//寻找单位列向量 
			int cnt = 0;		//记录单位列向量个数 
			for(int i = this->n-1 ; i >= 0 ; i--){
				//判断第i列是否单位向量 
				int _index = this->isUnitVector(i);
				if(_index == -1){//不是单位列向量 
					flag[i] = 0;
					index[i] = -1; 
				}else{
					flag[i] = 1;
					index[i] = _index;
					//依次记录基向量 
					this->basic_vector_list[cnt++] = i; 
				} 
			}
			//对基向量进行排序 
			sort(basic_vector_list,basic_vector_list+this->m);
			//cnt与m相等 
			for(int i = 0 ; i < cnt ; i++){//对cb进行赋值 
				this->cb[i] = this->c[this->basic_vector_list[i]];
			}	
		}
		
		//线性规划标准型单纯型算法解法 
		bool SimplexSolve(){
			bool ans;
			//和单纯行算法不同，两阶段算法第一阶段中，由于松弛(辅助)变量和人工变量的存在
			//人工变量和松弛变量对应的约束系数一定是单位列向量。因此没有必要像单纯形算法
			//需要对约束矩阵进行初等变换，从而变换出单位列向量。即只需要找出初始可行基
			//及其目标系数即可。之后执行单纯形算法 
			//在这里基矩阵默认变换成单位阵，以方便单纯型算法的相关矩阵计算
			//初始化典式数组 
			Fraction test_array[this->n];
			for(int i = 0 ; i < this->n ; i++){
				test_array[i].set("0/1");
			}
			while(1){
				this->Print();
				//计算检验数向量(典式)
				this->ComputeTestNumber(test_array,this->n);
				cout<<endl<<"检验数为："<<endl;
				for(int i = 0 ; i < this->n ; i++){
					cout<<test_array[i].result()<<" ";
				}
				cout<<endl;
				//判断典式是否全小于0
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
						//基矩阵已经转化为相乘单位矩阵，因此简化了矩阵运算
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
		
		//计算检验数（典式） 
		void ComputeTestNumber(Fraction* test_array,int n){
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					test_array[i].set("0/1"); 
				} 
				test_array[i] = this->c[i];
				for(int j = 0 ; j < this->m ; j++){
					Fraction tmp;
					tmp = tmp.MUL(this->cb[j],this->A[j][i]);
					test_array[i] = test_array[i].SUB(test_array[i],tmp);
				}
			} 
		}
		
		//判断向量是是否全为0
		int isAllLessZero(Fraction* array,int size){
			int flag = 1;
			for(int i = 0 ; i < size ; i++){
				//isuse[i]是第i个变量是否能被使用，用来标记第二阶段中人工变量不能使用
				if(this->isuse[i] == 0){//人工变量不做处理  
					continue;
				}
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
				//isuse[i]是第i个变量是否能被使用，用来标记第二阶段中人工变量不能使用
				if(this->isuse[i] == 0){//人工变量不做处理 
					continue;
				}
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
		
		//寻找基变量列表中k所在的下标 
		int Find_BasicVector_Index(int k){
			for(int i = 0 ; i < this->m ; i++){
				if(this->basic_vector_list[i] == k){
					return i;
				}
			} 
		}	
				
		//算约束矩阵列向量与常数项相除的结果 
		void Divide_Array(Fraction* pk,int size){
			for(int i = 0 ; i < size ; i++){
				//由于Fraction中对于除数为0的结果定义为"9999999/1"
				this->theta[i] = this->theta[i].DIV(this->b[i],pk[i]);
			}
		}
		
		//A[row][col]定位主元后，约束矩阵进行初等变换。将A[row][col]定位主元后,
		//将其转化为1，该列其余为转化为0，常数项和其余约束矩阵做相应处理 
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
						if(this->isuse[j] == 0){
							continue;
						}
						tmp = tmp.MUL(this->A[row][j],rate);
						this->A[i][j] = this->A[i][j].SUB(this->A[i][j],tmp);
					}
					//对常数项进行变换 
					tmp = tmp.MUL(this->b[row],rate);
					this->b[i] = this->b[i].SUB(this->b[i],tmp);
				}
			} 
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
				cout<<"\t"<<"x"<<this->basic_vector_list[i]+1; 
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
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				}
				cout<<"\t"<<"x"<<i+1; 
			}
			cout<<endl;
			for(int i = 0 ; i < this->n ; i ++){
				if(this->isuse[i] == 0){
					continue;
				}
				cout<<"\t"<<x[i].result();
			}
			cout<<endl;
			Fraction opt;//最优解 
			opt.set("0/1");	
			//计算最优值						
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				} 
				Fraction tmp;
				tmp= tmp.MUL(this->c[i],x[i]);
				opt = opt.ADD(opt,tmp);
			}
			//若目标是最小化，由于但数单纯型算法是解标准型，
			//因此需要在对结果取相反数 
			if(this->type == -1){
				opt.setOpposite();
			}
			cout<<"目标系数为："<<endl; 
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				}
				cout<<"\t"<<"c"<<i+1; 
			}
			cout<<endl;
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				}
				if(type == -1){
					this->c[i].setOpposite();	
				}
				cout<<"\t"<<this->c[i].result(); 
			} 
			cout<<endl;
			cout<<"最优解为："<<opt.result()<<endl;
			return  opt; 
		}
		
		//打印函数 
		void Print(){
			cout<<"约束矩阵为："<<endl;
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				} 
				cout<<"\t"<<"x"<<i+1; 
			}
			cout<<endl;
			for(int i = 0 ; i < this->m ; i++){ 
				for(int j = 0 ; j < this->n ; j++){
					if(this->isuse[j] == 0){
						continue;
					}
					cout<<"\t"<<this->A[i][j].result();
				}
				cout<<endl;
			}
			cout<<"常数项为："<<endl;
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				}
				cout<<"\t"<<this->b[i].result();
			} 
			cout<<endl;
			cout<<"目标系数（c）为："<<endl;
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				}
				cout<<"\tx"<<i+1; 
			}
			cout<<endl;
			for(int i = 0 ; i < this->n ; i++){
				if(this->isuse[i] == 0){
					continue;
				}
				cout<<"\t"<<this->c[i].result();
			} 
			cout<<endl;
			cout<<"目标系数（cb）为："<<endl;
			for(int i = 0 ; i < this->m ; i++){
				cout<<"\tx"<<this->basic_vector_list[i]+1;
			} 
			cout<<endl;
			for(int i = 0 ; i < this->m ; i++){
				cout<<"\t"<<this->cb[i].result();
			}
			cout<<endl<<"基向量组:"<<endl;
			for(int i = 0 ; i < this->m ; i++){
				cout<<"\tx"<<this->basic_vector_list[i]+1;
			}
			cout<<endl;
		} 
};

int main()
{
	Two_Stages_Simplex two_stages_simplex; 
	while(1){
		two_stages_simplex.Init();
		bool flag = two_stages_simplex.Two_Stage_Simplex_Solve();
		if(flag){
			Fraction ans = two_stages_simplex.getopt(); 	
		}else{
			cout<<"无最优解"<<endl; 
		}	
	}
	
	return 0;
 } 
