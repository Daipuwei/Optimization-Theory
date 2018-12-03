#include <iostream>
#include <string>
using namespace std;

class Fraction{
	private:
		string fenmu;			//分母 
		string fenzi;			//分子 
	public:
		//分数类的构造函数 
		void set(string str){
			if(str.find('/') == string::npos){
				this->fenmu = "1";
				this->fenzi = str;	
			}else{
				int index = str.find('/');
				string fenzi = str.substr(0,index);
				string fenmu = str.substr(index+1,str.length()-index-1);
				this->fenmu = fenmu;
				this->fenzi = fenzi;
			}
		}
		
		//分数类的设置函数 
		void set(string fenmu,string fenzi){ 
			if(fenmu[0] == '-' && fenzi[0] == '-'){			// 分子分母同为负 
				fenmu = fenmu.erase(0,1);
				fenzi = fenzi.erase(0,1);
			}else if(fenmu[0] == '-' && fenzi[0] != '-'){	//分母为负 分子为正
				fenmu = fenmu.erase(0,1);
				fenzi = "-"+fenzi;
			}
			if(fenzi[0] == '-'){			//分子为0时，前面有负号 
				string tmp = fenzi.substr(1,fenzi.length()-1);
				if(tmp.compare("0") == 0){
					fenzi = tmp;
				}
			}
			this->setFenmu(fenmu);
			this->setFenzi(fenzi);
			//this->fenmu = fenmu;
			//this->fenzi = fenzi;
			//cout<<this->fenmu<<" "<<this->fenzi<<endl; 
		}
		
		void setFenzi(string fenzi){
			this->fenzi = fenzi;
		}
		
		void setFenmu(string fenmu){
			this->fenmu = fenmu; 
		} 
		
		string getFenzi(){
			return this->fenzi;
		}
		
		string getFenmu(){
			return this->fenmu; 
		}
		
		//分数取反 
		void setOpposite(){
			int flag = this->Compare2Zero();
			//cout<<this->result()<<endl;
			//cout<<flag<<endl;
			//为0不需要置反，保持原样即可
			//下面主要考虑负0的情况 
			if(flag != 0){
				string fenzi = this->getFenzi();
				if(flag == -1){//为负数时 
					fenzi = fenzi.erase(0,1);		//去掉前面的符号 
				}else if(flag == 1){//为正数时 
					fenzi = "-"+fenzi;
				}
				this->setFenzi(fenzi); 
			}
		} 
		
		string result(){
			/*if(this->fenzi.compare("0") == 0 && this->fenmu.compare("0") != 0){
				return "0";
			}*/
			string res = this->fenzi;
			if(this->fenmu.compare("0") != 0){
				if(this->fenmu.compare("1") != 0){
					res = res+"/"+this->fenmu; 
				}	
			}
			return res; 
		}
		
		//这是比较两个正数之间大小的函数 
		int compare(string str1,string str2){	
			//相等返回0，大于返回1，小于返回-1
			if(str1.size()>str2.size()){
				return 1; //长度长的整数大于长度小的整数
			}else if(str1.size()<str2.size()){
				return -1;	
			}else{
				return str1.compare(str2); //若长度相等，则头到尾按位比较
			}
		}
		
		//计算两个数最大公约数 
		string gcd(string str1,string str2){
			int sign = 1;
			if(str1[0] ==  '-'){			// str1为负数，去除前导负号 
				sign *= -1;					//标志位取相反数 
				str1 = str1.erase(0,1);		 
			}
			if(str2[0] == '-'){				// str2为负数，去除前导负号 
				sign *= -1;					//标志位取相反数 
				str2 = str2.erase(0,1);
			}
			if(str1.compare("1") == 0 || str2.compare("1") == 0){		//str1或str2为1时 
				if(sign == -1){				//标志位位-1，str2加负号 
					return "-1";		 
				}
				return "1";
			}else{				//str1和str2都不为1时，利用辗转相除法求最大公约数 
				string gcd;
				while(str2.compare("0") != 0){
					gcd = this->MOD_INT(str1,str2);
					str1 = str2;
					str2 = gcd;
				}
				gcd = str1;
				if(sign == -1){ 
					gcd = "-"+gcd;
				} 
				return gcd;
			}
		} 
	
		//计算两个数最小公倍数
		string gcm(string str1,string str2){
			if(str1.compare("1") == 0 || str2.compare("1") == 0){
				return "1";
			}
			if(str1.compare("-1") == 0 || str2.compare("-1") == 0){
				return "-1";
			}
			string tmp1 = str1;
			string tmp2 = str2;
			string gcd = this->gcd(tmp1,tmp2);			//计算最大公约数 
			string gcm = this->MUL_INT(str1,str2);
			gcm = this->DIV_INT(gcm,gcd);				//计算最小公倍数 
			return gcm; 
		} 
		
		//分数加法
		Fraction ADD(Fraction num1,Fraction num2){
			string fenmu,fenzi,gcd;
			if(num1.fenmu.compare("0") != 0 && num1.fenzi.compare("0") == 0){
				return num2;
			}
			if(num2.fenmu.compare("0") != 0 && num2.fenzi.compare("0") == 0){
				return num1;
			}
			if(num1.fenmu.compare(num2.fenmu) == 0){//两个分母相等 
				fenmu = num1.fenmu;									//保留分母 
				fenzi = this->ADD_INT(num1.fenzi,num2.fenzi);		//计算分子 
				gcd = this->gcd(fenmu,fenzi);						//计算最大公约数
				/*cout<<fenzi<<endl;
				cout<<gcd<<endl;*/ 
				fenmu = this->DIV_INT(fenmu,gcd);					//约分后的分母
				fenzi = this->DIV_INT(fenzi,gcd);					//约分后的分子
			}else{//分母不相等 
				string fenzi1 = this->MUL_INT(num1.fenzi,num2.fenmu);			//计算通分后的分子1 
				string fenzi2 = this->MUL_INT(num2.fenzi,num1.fenmu);			//计算通分后的分子2 
				fenzi = this->ADD_INT(fenzi1,fenzi2);							//计算分子 
				fenmu = this->MUL_INT(num1.fenmu,num2.fenmu);					//计算分母 
				gcd = this->gcd(fenmu,fenzi);									//计算分子和分母的最大公约数 
				if(gcd.compare("1") != 0){
					fenmu = this->DIV_INT(fenmu,gcd);								//计算通分后的分母
					fenzi = this->DIV_INT(fenzi,gcd);								//计算通分后的分子 	
				}
			}
			Fraction result;
			/*cout<<"fenzi:"<<fenzi<<endl;
			cout<<"fenmu:"<<fenmu<<endl; 
			cout<<"gcd:"<<gcd<<endl;*/
			result.set(fenmu,fenzi); 
			return result;
		}
		
		//分数减法
		Fraction SUB(Fraction num1,Fraction num2){
			// num1为0时，返回num2的相反数 
			if(num1.fenmu.compare("0") != 0 && num1.fenzi.compare("0") == 0){
				string fenzi = num2.getFenzi();
				if(fenzi[0] == '-'){
					fenzi = fenzi.erase(0,1);
				}else{
					fenzi = '-'+fenzi; 
				}
				string fenmu = num2.fenmu;
				Fraction result;
				result.set(fenmu,fenzi); 
				return result;
			}
			//num2为0时，返回num1 
			if(num2.fenmu.compare("0") != 0 && num2.fenzi.compare("0") == 0){
				return num1;
			}
			string fenmu,fenzi,gcd; 
			if(num1.fenmu.compare(num2.fenmu) == 0){//两个分母相等 
				fenmu = num1.getFenmu();									//保留分母 
				fenzi = this->SUB_INT(num1.getFenzi(),num2.getFenzi());		//计算分子 
				//cout<<fenmu<<" "<<fenzi<<endl;
				gcd = this->gcd(fenmu,fenzi);						//计算最大公约数 
				if(gcd.compare("1") != 0){
					fenmu = this->DIV_INT(fenmu,gcd);					//约分后的分母
					fenzi = this->DIV_INT(fenzi,gcd);					//约分后的分子	
					/*cout<<"gcd="<<gcd<<endl;
					cout<<fenmu<<" "<<fenzi<<endl;*/
				}
			}else{//分母不相等 
				string fenzi1 = this->MUL_INT(num1.fenzi,num2.fenmu);			//计算通分后的分子1 
				string fenzi2 = this->MUL_INT(num2.fenzi,num1.fenmu);			//计算通分后的分子2 
				fenzi = this->SUB_INT(fenzi1,fenzi2);							//计算分子 
				fenmu = this->MUL_INT(num1.fenmu,num2.fenmu);					//计算分母 
				gcd = this->gcd(fenmu,fenzi);									//计算分子和分母的最大公约数 
				if(gcd.compare("1") != 0){
					fenmu = this->DIV_INT(fenmu,gcd);								//计算通分后的分母
					fenzi = this->DIV_INT(fenzi,gcd);								//计算通分后的分子 	
				}
			}
			/*cout<<num1.result()<<"-"<<num2.result()<<"="<<endl;
			cout<<fenmu<<endl;
			cout<<fenzi<<endl;
			cout<<gcd<<endl;*/ 
			Fraction result;
			result.set(fenmu,fenzi);
			return result;
		}
		
		//分数乘法，保证分母没有0出现情况 
		Fraction MUL(Fraction num1,Fraction num2){
			string fenmu,fenzi;
			if(num1.fenzi.compare("0") == 0|| num2.fenzi.compare("0") == 0){		//分子出现0时，直接返回0 
				Fraction result;
				result.set("0/1");
				return result;
			}
			//计算分母 
			if(num1.fenmu.compare("1") == 0 && num2.fenmu.compare("1") == 0){			//两个分母全为1 
				fenmu = "1"; 
			}else if(num1.fenmu.compare("1") == 0 && num2.fenmu.compare("1") != 0){		//num1分母为1 
				fenmu = num2.fenmu;
			}else if(num1.fenmu.compare("1") != 0 && num2.fenmu.compare("1") == 0){		//num2分母为1 
				fenmu = num1.fenmu;
			}else{			//两个分母都不为1 
				fenmu = this->MUL_INT(num1.fenmu,num2.fenmu);
			}
			//计算分子
			//cout<<num1.fenzi<<" "<<num2.fenzi<<endl;
			if(num1.fenzi.compare("1") == 0 && num2.fenzi.compare("1") == 0){			//两个分子全为1 
				fenzi = "1"; 
			}else if(num1.fenzi.compare("1") == 0 && num2.fenzi.compare("1") != 0){		//num1分子为1 
				fenzi = num2.fenzi; 
			}else if(num1.fenzi.compare("1") != 0 && num2.fenzi.compare("1") == 0){		//num2分子为1 
				fenzi = num1.fenzi; 
			}else{			//两个分子都不为1 
				fenzi = this->MUL_INT(num1.fenzi,num2.fenzi);
			}
			// 分子分母都不为1时需要进行约分 
			if(fenmu.compare("1") != 0 && fenzi.compare("1") != 0){
				string gcd = this->gcd(fenmu,fenzi);
				if(gcd.compare("1") != 0){
					fenmu = this->DIV_INT(fenmu,gcd);								//计算通分后的分母
					fenzi = this->DIV_INT(fenzi,gcd);								//计算通分后的分子 	
				}
			}
			Fraction result;
			result.set(fenmu,fenzi); 
			return result; 
		} 
		
		//分数除法，分母不出现0 
		Fraction DIV(Fraction num1,Fraction num2){
			// num1为0时 
			if(num1.fenmu.compare("0") != 0 && num1.fenzi.compare("0") == 0){
				Fraction result;
				result.set("0/1");
				return result;
			}
			//num2为1时
			if(num2.fenmu.compare("1") == 0 && num2.fenzi.compare("1") == 0){
				return num1;
			}
			//num2为0时，我们将结果初始化为"9999999/1" 
			if(num2.fenmu.compare("1") == 0 && num2.fenzi.compare("0") == 0){
				Fraction result;
				result.set("9999999/1");
				return result;
			}
			string fenmu = num2.fenzi;
			string fenzi = num2.fenmu;
			//除法转化为乘法 
			Fraction tmp;
			tmp.set(fenmu,fenzi);
			return tmp.MUL(num1,tmp);
		} 
		
		//整数加法 
		string ADD_INT(string str1,string str2){
			//str1为0，返回str2 
			if(str1.compare("0") == 0){
				return str2;
			}
			//str2为0，返回str1
			if(str2.compare("0") == 0){
				return str1;
			}
			//高精度加法
		    int sign=1; //sign 为符号位
		    string str;
		    if(str1[0]=='-'){	//str1为负数 
				if (str2[0]=='-'){	//str2为负数 
					sign=-1;	//-1代表两个负数相加 
		            // 去掉两个符号后相加 
					str=ADD_INT(str1.erase(0,1),str2.erase(0,1));
		        }else{//str2为正数
					//去掉str1的符号，转化为减法 
		            str=SUB_INT(str2,str1.erase(0,1));
		        }
		    }else{	// str1为正数 
		        if(str2[0]=='-'){	//str2为负数 
		        	// 去掉str2的负数，转化为减法 
		            str=SUB_INT(str1,str2.erase(0,1));
		        }else{	//str2为正数 
					//把两个整数对齐，短整数前面加0补齐
		            string::size_type L1,L2;
		            int i;
		            L1=str1.size();
		            L2=str2.size();
		            if(L1<L2){	// str1的长度比str2小，str1在前补充0 
		                for(i=1;i<=L2-L1;i++){
		                	str1="0"+str1;	
						}
		            }else{	// str2的长度比str2小，str2在前面补充0 
		                for(i=1;i<=L1-L2;i++){
		                	str2="0"+str2;	
						}
		            }
		            int int1=0,int2=0; //int2 记录进位，int1记录结果 
		            for(i=str1.size()-1;i>=0;i--){
		                int1=(int(str1[i])-'0'+int(str2[i])-'0'+int2)%10;
		                int2=(int(str1[i])-'0'+int(str2[i])-'0'+int2)/10;
		                str=char(int1+'0')+str;
		            }
		            if(int2!=0){	//计算结束后还有进位，直接在前面补充 
		            	str=char(int2+'0')+str;	
					}
		        }
		    }
		    //运算后处理符号位
		    if((sign==-1)&&(str[0]!='0')){
		    	str="-"+str;	
			}
		    return str;
		}
		
		//整数减法 
		string SUB_INT(string str1,string str2){
			//str1为0，返回str2 
			if(str1.compare("0") == 0){
				//str2为正数 
				if(str2.compare("0") == 1){
					return "-"+str2; 
				}else if(str2.compare("0") == 0){
					return "0";
				}else{
					return str2.erase(0,1);
				}
			}
			//str2为0，返回str1 
			if(str2.compare("0") == 0){
				return str1;
			}	
			//高精度减法
		    int sign=1; //sign为符号位
		    string str;
		    int i,j;
			if(str2[0]=='-'){	//str2为负数
				// 去掉str2的符号，转化为加法 
		        str=ADD_INT(str1,str2.erase(0,1));
		    }else{	// str2为正数
		    	if(str1[0] == '-'){		//str1为负数，转化为加法 
		    		str = ADD_INT(str1.erase(0,1),str2);
		    		str = "-"+str;
				}else{	//str1为正数 
					// 判断str1和str2大小 
			        int res=compare(str1,str2);
			    	//cout<<"==="<<res<<endl; 
			        if(res==0){		//str1和str2相等 
						return "0";	
					}
			        if(res<0){	//str1小于str2 
			            sign=-1;	//标志位置为-1
						// str1和str2互换 
			            string temp =str1;
			            str1=str2;
			            str2=temp;
			        }
			        string::size_type tempint; 
			        tempint=str1.size()-str2.size();
			        // 按值较小者的进行遍历，tempint=0说明长度相等，大于0则str1长度大于str2 
			        // 从最后一位开始遍历 
			        for(i=str2.size()-1;i>=0;i--){
			        	// str1的对应为小于str2的对应位 
			            if(str1[i+tempint]<str2[i]){
			                j=1;	//偏移位
							//开始循环，开始寻找借位 
			                while(1){
			                	//前面的位为0，借1置成9 
								if(str1[i+tempint-j]=='0'){
			                        str1[i+tempint-j]='9';
			                        j++;
			                    }else{//前面的位为0，借1，当前位减1 
			                        str1[i+tempint-j]=char(int(str1[i+tempint-j])-1);
			                        break;
			                    }
			                }
			                str=char(str1[i+tempint]-str2[i]+':')+str;
			            }else{
			                str=char(str1[i+tempint]-str2[i]+'0')+str;
			            }
			        }
			        //把多余为进行向前补充
			        for(i=tempint-1;i>=0;i--){
			        	str=str1[i]+str;	
					}
				}
			}
		    //去除结果中多余的前导0
		    str.erase(0,str.find_first_not_of('0'));
		    if(str.empty()){
		    	str="0";	
			}
		    if((sign==-1) && (str[0]!='0')){
		    	str ="-"+str;	
			}
		    return str;
		}
		
		//整数乘法 
		string MUL_INT(string str1,string str2){
			//str1和str2至少存在1个0，直接返回0 
			if(str1.compare("0") == 0 || str2.compare("0") == 0){
				return "0";
			}
			//str1为1直接返回str2 
			if(str1.compare("1") == 0){
				return str2; 
			}
			//str2为1直接返回str1
			if(str2.compare("1") == 0){
				return str1; 
			}
			//str1为-1
			if(str1.compare("-1") == 0){
				//str2为负数 
				if(str2.compare("0") < 0){
					return str2.erase(0,1);
				}else{
					//str2为正数 
					return "-"+str2; 	
				}
			}
			//str2为1
			if(str2.compare("-1") == 0){
				//str1为负数 
				if(str1.compare("0") < 0){
					return str1.erase(0,1);
				}else{
					//str1为正数 
					return "-"+str1;	
				}  
			}
			//高精度乘法
		    int sign=1; //sign 为符号位
		    string str;
		    // str1为负数 
		    if(str1[0]=='-'){
		        sign*=-1;	// 标志位乘-1
		        str1 =str1.erase(0,1);	// 去掉负号 
		    }
		    // str2为负数 
		    if(str2[0]=='-'){
		        sign*=-1;					// 标志位乘-1
		        str2 =str2.erase(0,1); 		// 去掉负号 
		    }
		    int i,j;
		    string::size_type L1,L2;
		    L1=str1.size();
		    L2=str2.size();
		    for(i=L2-1;i>=0;i--){ //模拟手工乘法竖式
		        string tempstr;
		        int int1=0,int2=0,int3=int(str2[i])-'0';
		        if(int3!=0){
		        	// 下面的循环实现结果后端补0操作 
		            for(j=1;j<=(int)(L2-1-i);j++){
		            	tempstr="0"+tempstr;	
					}
		            for(j=L1-1;j>=0;j--){
		            	// int1记录个位数，int记录十位数 
		                int1=(int3*(int(str1[j])-'0')+int2)%10;
		                int2=(int3*(int(str1[j])-'0')+int2)/10;
		                tempstr=char(int1+'0')+tempstr;
		            }
		            if(int2!=0){
		            	tempstr=char(int2+'0')+tempstr;	
					}
		        }
		        // 当前结果和tempstr向将 
		        str=ADD_INT(str,tempstr);
		    }
		    //去除结果中的前导0
		    str.erase(0,str.find_first_not_of('0'));
		    if(str.empty()){
		    	str="0";	
			}
		    if((sign==-1) && (str[0]!='0')){
		    	str="-"+str;	
			}
		    return str;
		}
		
		//整数除法 
		string DIVIDE_INT(string str1,string str2,int flag){
			//高精度除法。flag==1时,返回商; flag==0时,返回余数
		    string quotient,residue; //定义商和余数
		    int sign1=1,sign2=1;
		    if(str2 == "0"){  //判断除数是否为0
		        quotient= "ERROR!";
		        residue = "ERROR!";
		        if(flag==1){
		        	return quotient;	
				}else{
					return residue;	
				}
		    }
		    //判断被除数是否为0
			if(str1=="0"){
				quotient="0";
				residue ="0";
				if(flag == 1){
					return quotient;
				}else{
					return residue;
				}
			}
			//被除数为1，直接返回str1 
			if(str2 == "1"){
				quotient = str1;
				residue = "0";
				if(flag == 1){
					return quotient;
				}else{
					return residue;
				}
			}
			//被除数为-1 
			if(str2 == "-1"){
				if(str1.compare("0") < 0){
					quotient = str1.erase(0,1);
				}else{
					quotient = "-"+str1;
				}
				residue = "0";
				if(flag == 1){
					return quotient;
				}else{
					return residue;
				}
			}
		    // str1为负数 
		    if(str1[0]=='-'){
		        str1 = str1.erase(0,1);		//去除前导0 
		        sign1 *= -1;
		        sign2  = -1;
		    }
		    // str2为负数 
		    if(str2[0]=='-'){
		        str2 = str2.erase(0,1);		//去除前导0 
		        sign1 *= -1;
		    }
		    //比较str1和str2的大小 
		    int res=compare(str1,str2);
		    // str1小于str2 
		    if(res<0){
		    	// 商为0，余数为str1 
		        quotient="0";
		        residue =str1;
		    }else if(res == 0){
		    	// str1与str2相等,商为1，余数为0 
		        quotient="1";
		        residue ="0";
		    }else{
		    	// str1大于str2 
		        string::size_type L1,L2;
		        L1=str1.size();
		        L2=str2.size();
		        string tempstr;
		        tempstr.append(str1,0,L2-1);
		        for(int i=L2-1;i<L1;i++){ //模拟手工除法竖式
		            tempstr=tempstr+str1[i];
		            //可能出现不够除的情况，那么必须把前导0去掉 
		            tempstr.erase(0,tempstr.find_first_not_of('0'));
		            if(tempstr.empty()){
						tempstr="0";
					}
		        	for(char ch='9';ch>='0';ch--){ //试商
		                string str;
		                str=str+ch;
		                if(compare(MUL_INT(str2,str),tempstr)<=0){
		                    quotient=quotient+ch;
		                    tempstr =SUB_INT(tempstr,MUL_INT(str2,str));
		                    break;
		                }
		            }
		        }
		        residue=tempstr;
		    }
		    //去除结果中的前导0
		    quotient.erase(0,quotient.find_first_not_of('0'));
		    if(quotient.empty()){
		    	quotient="0";	
			}
		    if((sign1==-1)&&(quotient[0]!='0')){
		    	quotient="-"+quotient;	
			}
		    if((sign2==-1)&&(residue [0]!='0')){
		    	residue ="-"+residue ;	
			}
			if(flag==1){
		    	return quotient;	
			}else{
				return residue;	
			}
		}
		
		//整数除法，返回商 
		string DIV_INT(string str1,string str2){
			//高精度除法,返回商
		    return DIVIDE_INT(str1,str2,1);
		}
		
		//整肃除法，返回余数 
		string MOD_INT(string str1,string str2){
			//高精度除法,返回余数
		    return DIVIDE_INT(str1,str2,0);
		}
		
		//判断分数与0的大小 
		int Compare2Zero(){
			int sign_fenzi = 1;
			int sign_fenmu = 1;
			int sign = 1;		//记录是否分子含有负号 
			string fenzi = this->fenzi;
			string fenmu = this->fenmu; 
			if(fenzi[0] == '-'){	//分子为负数时，改变标志位 
				sign_fenzi = -1;
				fenzi = fenzi.erase(0,1);
			}
			if(fenmu[0] == '-'){	//分子为负数时，改变标志位  
				sign_fenmu = -1;
				fenmu = fenmu.erase(0,1);
			} 
			int ans_fenzi = this->compare(fenzi,"0");		//比较分子的绝对值与0的大小 
			int ans_fenmu = this->compare(fenmu,"0");		//比较分母的绝对值与0的大小 
			if(sign_fenzi == -1 && ans_fenzi == 1){
				ans_fenzi = -1;
			}
			if(sign_fenmu == -1 && ans_fenmu == 1){
				ans_fenmu = -1;
			}
			int flag;
			if(ans_fenzi == 0 && ans_fenmu != 0){		//分子为0，分母不为0，返回0 
				flag = 0;
			}
			if(ans_fenzi*ans_fenmu > 0){				//分子分母同号，返回1 
				flag = 1;	
			}
			if(ans_fenzi*ans_fenmu < 0){				//分子分母异号，返回-1 
				flag = -1;								
			}
			return flag;
		}
		
		//两个分数之间的比较
		int Compare2Fraction(Fraction fraction){
			Fraction ans,tmp;
			/*cout<<"/////////////////////////"<<endl;
			cout<<"当前分数："<<this->result()<<endl;
			cout<<"作对比的分数:"<<fraction.result()<<endl;*/
			tmp.set(this->getFenmu(),this->getFenzi());
			//减后的结果与0作对比就是两个分数之间的大小关系 
			ans = ans.SUB(tmp,fraction); 
			/*cout<<"相减结果为："<<ans.result()<<endl; 
			cout<<"对比结果为："<<ans.Compare2Zero()<<endl;
			cout<<"/////////////////////////"<<endl;*/
			return ans.Compare2Zero();
		} 
};
