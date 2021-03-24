#include <cstdlib>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "main.h"

#define rsT mipsWrite

int OPT = 0;
int funcnum = 0;
int mid = 0;    //四元式处理的行数
int sp = INITSTACK;    //栈指针,此地址为即将分配的！！相对fp！！地址
int fp = 0;//帧指针
int ap;//地址表计数器
int ismain = 0;
int globedge = 0;
int tlabelnum = 0;
int isglob;
int paran = 0;
int tmi;
int funcedge = 0;
int savespflg = 0;
int vrnum;
tempvaraddress addrtable[1000];//临时变量在栈中的地址表
vector<funcclass> fc;//函数表
int varreg[200];

//优化计数 计算局部变量出现的频数
void cntplus(char *varname)
{
    int t = ap - 1;
    isglob = 0;
    if (varname[0] == '+' || varname[0] == '-' || ( varname[0] >= '0'&& varname[0] <= '9' ) || varname[0] == ' ' || varname[0] == '\t' || varname[0] == '\0')
        return;
    while (t >= globedge) 
	{
        if (strcmp(addrtable[t].name, varname) == 0) 
		{
            addrtable[t].cnt++;
            break;
        }
        t--;
    }
}
int findinvec(vector<int> vec,int num)
{
    vector<int>::iterator ret;
    ret = find(vec.begin(), vec.end(), num);
    if(ret == vec.end()) return 0;//未找到
    else return 1;//找到
}
//引用计数分配全局寄存器
void regopt() 
{
    //引用计数
    int tmi;
    tmi = mid;
    while (strcmp(midcode[tmi].op, "end") != 0) 
	{
        if (strcmp(midcode[tmi].op, "=")==0 || strcmp(midcode[tmi].op, "+")==0 ||
                strcmp(midcode[tmi].op, "-")==0 || strcmp(midcode[tmi].op, "*")==0 || strcmp(midcode[tmi].op, "/")==0 )
        {
            cntplus(midcode[tmi].var1);
            cntplus(midcode[tmi].var2);
            cntplus(midcode[tmi].var3);
        }
        tmi++;
    }
    //排序
    vector<int> sortindex;
    int maxcnt = 0;
    int maxindex = 0;
    for(int rn = 0; rn < 7; rn++)
	{
        int flg=0;
        maxcnt = 0;
		maxindex = 0;
        for(int i = globedge; i < ap; i++)
		{
            if(addrtable[i].cnt >= maxcnt  && !findinvec(sortindex,i))
            {
                maxindex = i;
                maxcnt = addrtable[i].cnt;
                flg=1;
            }
        }
        if(flg == 1) sortindex.push_back(maxindex);
        else sortindex.push_back(-1);
    }
    //分配全局寄存器
    for (int i = 0; i < 7; i++) 
	{
        if(sortindex[i] != -1)
		{
            varreg[sortindex[i]] = i+1;
            int addr = addrtable[sortindex[i]].address;
            rsT << "lw\t$s" << i+1 << "\t" << addr << "($fp)" << endl;
        }
    }
}
//查找汇编变量地址
int varaddr(char *name) 
{
    int t = ap - 1;
    isglob = 0;
    if (name[0] == '+' || name[0] == '-' || name[0] >= '0'&&name[0] <= '9' ) return -1;
    while (t >= 0) 
	{
        if (strcmp(addrtable[t].name, name) == 0) 
		{
            if (t < globedge)  isglob = 1;
            return addrtable[t].address;
        }
        t--;
    }
    return -1;
}
//查找汇编变量在地址表中的位置
int varaddrt(char *name)
{
    int t = ap - 1;
    isglob = 0;
    if (name[0] == '+' || name[0] == '-' || name[0] >= '0'&&name[0] <= '9') return -1;
    while (t >= 0) 
	{
        if (strcmp(addrtable[t].name, name) == 0) 
		{
            if (t < globedge) isglob = 1;
            return t;
        }
        t--;
    }
    return -1;
}
void pushstack(const char* item,int len) //压栈
{
    if (len == 1) 
	{
        rsT << "li\t$t0\t" << item << "\t#" << midcode[mid].var3  <<endl; //li  $t0 item
        rsT << "sw\t$t0\t($sp)" << endl;      //sw    $t0 ($sp)  将数据写入$sp所在现在的内存地址
    }
    rsT << "subi\t$sp\t$sp\t" << 4 * len << endl; //subi  $sp $sp 4 上移4
    return;
}
//给地址表插入地址（上行）
void insertaddress(Symbol kind, int index,int len) 
{
    strcpy(addrtable[ap].name, midcode[index].var3);
    addrtable[ap].address = sp;
    addrtable[ap].kind = kind;
    sp -= ( 4 * len );
    ap++;
}
//给地址表插入参数地址，sp不变（下行）
void parainsertaddress(Symbol kind,int addr) 
{
    strcpy(addrtable[ap].name, midcode[mid].var3);
    addrtable[ap].address = addr;
    addrtable[ap].kind = kind;
    ap++;
}
Symbol funckind(char *fname) //函数返回值
{
    for (unsigned int i = 0; i < fc.size(); i++ ) 
	{
        if (strcmp(fc[i].name, fname) == 0)  return fc[i].kind;
    }
	return none;
}
int findvartable(char *name)//返回地址表里的位置
{
    int t = ap - 1;
    if (name[0] == '+' || name[0] == '-' || name[0] >= '0'&&name[0] <= '9') return -1;
    while (t >= 0) 
	{
        if (strcmp(addrtable[t].name, name) == 0) return t;
        t--;
    }
    return -1;
}
Symbol varkind(char *name) //变量类型
{
    int t = ap - 1;
    if (name[0] == '+' || name[0] == '-' || name[0] >= '0'&&name[0] <= '9' )
        return none;
    while (t >= 0) 
	{
        if ( strcmp(addrtable[t].name, name) == 0 ) return addrtable[t].kind;
        t--;
    }
    return none;
}
void Tomips()
{
    memset(varreg, -1, sizeof(int) * 200);
    rsT << ".data" << endl;
    rsT << "\tEnter: .asciiz  \"\\n\"" <<endl;
    rsT << ".text" << endl;
    rsT << "ori\t$fp\t$sp\t0" << endl;//ori fp,sp,0,即fp=sp 7fffeffc，Mars中显示的
    rsT << "li\t$t9\t0x7fffeffc\t#global stack bottom" << endl; //li $t9,0x7fffeffc
    rsT << "li\t$t8\t0x10010000\t#save word" << endl;           //li $t8,0x10010000 .data
    mid = ap = 0;
	sp = 0;
    while (mid < midcnt)
	{
        //全局常量定义
        while(strcmp(midcode[mid].op, "const") == 0) 
		{
            //sp在pushstack中已经减去
            pushstack(midcode[mid].var2,1);
            if (strcmp(midcode[mid].var1, "int") == 0)  insertaddress(intty,mid,1);
            else  insertaddress(charty,mid,1);       
            mid++;
        }
        //全局变量定义
        while (strcmp(midcode[mid].op, "int") == 0 || strcmp(midcode[mid].op, "char") == 0
               || strcmp(midcode[mid].op, "inta") == 0 || strcmp(midcode[mid].op, "chara") == 0)
		{
            if(strcmp(midcode[mid].op, "int") == 0 || strcmp(midcode[mid].op, "char") == 0)
			{
                pushstack("0",1);//初始化为0
                if (strcmp(midcode[mid].op, "int") == 0)  insertaddress(intty,mid,1);
                else  insertaddress(charty,mid,1);               
                mid++;
            }
             //全局数组定义
            else if(strcmp(midcode[mid].op, "inta") == 0 || strcmp(midcode[mid].op, "chara") == 0 )
			{
                pushstack("0", atoi(midcode[mid].var2));//初始化为0
                if (strcmp(midcode[mid].op, "inta") == 0)  insertaddress(intty,mid,atoi(midcode[mid].var2));
                else  insertaddress(charty,mid,atoi(midcode[mid].var2));          
                mid++;
            }
        }
        rsT << "j\t__main" << endl;
        globedge = ap;
        //函数定义
        funcclass tfc;
        while (strcmp(midcode[mid].op, "func") == 0) 
		{
            funcnum++;
            if (strcmp(midcode[mid].var1, "char") == 0) 
			{
                tfc.kind = charty;
                strcpy(tfc.name, midcode[mid].var3);
                tfc.startap=ap;
                fc.push_back(tfc);
            }
			else if (strcmp(midcode[mid].var1, "int") == 0) 
			{
                tfc.kind = intty;
                strcpy(tfc.name, midcode[mid].var3);
                tfc.startap=ap;
                fc.push_back(tfc);
            } 
			else 
			{
                tfc.kind = voidty;
                strcpy(tfc.name, midcode[mid].var3);
                tfc.startap=ap;
                fc.push_back(tfc);
            }
            if (strcmp(midcode[mid].var3, "main") == 0) 
			{
                ismain = 1;
                rsT << "__main:" << endl;
            }
			else
			{
                ismain = 0;
                rsT << midcode[mid].var3 << ":" << endl;
            }
            mid++;
            funcasm();
        }
    }
}
//处理函数内容，不处理最后的end和开始的func
void funcasm() 
{
    memset(varreg, -1, sizeof(int) * 200);
    sp = 0;//相对偏移为0
    //保存现场
    rsT << "#Save Register" << endl;
    save();
    rsT << "sw\t$s0\t0($sp)" <<endl;     //保存上一个函数的sp
    rsT << "subi\t$sp\t$sp\t4" << endl;//$sp-=4
    rsT << "sw\t$fp\t0($sp)" << endl;//保存上一个函数的$fp
    rsT << "subi\t$sp\t$sp\t4" << endl;//$sp-=4
    rsT << "add\t$fp\t$sp\t$0" << endl;//设置本函数$fp：$fp=$sp
    rsT << "sw\t$ra\t($sp)" << endl;//保存$ra
    rsT << "subi\t$sp\t$sp\t4" << endl;//$sp-=4
    sp -= 4;
    rsT << "#Save Register Done!" << endl;
    //临时变量定义
	tmi = mid;
	vrnum = 0;
    while (strcmp(midcode[tmi].op, "end") != 0) 
	{
        char v1[100], v2[100], v3[100];
        strcpy(v1, midcode[tmi].var1);
        strcpy(v2, midcode[tmi].var2);
        strcpy(v3, midcode[tmi].var3);
        if (v1[0] == '$' && varaddr(v1) == -1) //临时标记
		{
            pushstack("0",1);
            insertaddress(intty, tmi, 1);
        }
        if (v2[0] == '$' && varaddr(v2) == -1) 
		{
            pushstack("0",1);
            insertaddress(intty, tmi, 1);
        }
        if (v3[0] == '$' && varaddr(v3) == -1) 
		{
            pushstack("0",1);
            if (strcmp(midcode[tmi].op, "call") == 0)  insertaddress(funckind(midcode[tmi].var1), tmi, 1);
            else if (strcmp(midcode[tmi].op, "geta") == 0) insertaddress(varkind(v1), tmi, 1);       
            else insertaddress(intty, tmi, 1);          
        }
        tmi++;
        vrnum++;
    }
    while (strcmp(midcode[mid].op, "end") != 0) 
	{
        paran = 0;
        for (unsigned int i = 0; i < strlen(midcode[mid].op); i++ ) 
		{
            if (midcode[mid].op[i] == ' ')
			{
                midcode[mid].op[i] = '\0';
                break;
            }
        }
        if(OPT != 1)
		{
            if (strcmp(midcode[mid].op, "+") == 0) addasm();
            if (strcmp(midcode[mid].op, "-") == 0) subasm();
            if (strcmp(midcode[mid].op, "*") == 0) mulasm();
            if (strcmp(midcode[mid].op, "/") == 0) divasm();
            if (strcmp(midcode[mid].op, ">") == 0) greasm();
            if (strcmp(midcode[mid].op, ">=") == 0) geqasm();
            if (strcmp(midcode[mid].op, "<") == 0) lssasm();
            if (strcmp(midcode[mid].op, "<=") == 0) leqasm();
            if (strcmp(midcode[mid].op, "!=") == 0) neqasm();
            if (strcmp(midcode[mid].op, "==") == 0) eqlasm();
            if (strcmp(midcode[mid].op, "=") == 0) assasm();
            if (strcmp(midcode[mid].op, "[]=") == 0) aassasm();
            if (strcmp(midcode[mid].op, "geta") == 0) assaasm();
            if (strcmp(midcode[mid].op, "lab:") == 0) setlabasm();
            if (strcmp(midcode[mid].op, "scf") == 0) scfasm();
            if (strcmp(midcode[mid].op, "prt") == 0) prtasm();           
            if (strcmp(midcode[mid].op, "jne") == 0) jneasm();
            if (strcmp(midcode[mid].op, "jmp") == 0) jmpasm();
            if (strcmp(midcode[mid].op, "fupa") == 0) fupaasm();
            if (strcmp(midcode[mid].op, "call") == 0) callasm();
            if (strcmp(midcode[mid].op, "ret") == 0) retasm();
            if (strcmp(midcode[mid].op, "para") == 0) 
			{ 
				paraasm();
				vrnum++; 
			}
            if (strcmp(midcode[mid].op, "int") == 0|| strcmp(midcode[mid].op, "char") == 0) 
			{
				intcharasm();
				vrnum++; 
            }
            if (strcmp(midcode[mid].op, "const") == 0) 
			{
				constdefasm();
				vrnum++;
			}
            if (strcmp(midcode[mid].op, "inta") == 0 || strcmp(midcode[ mid ].op, "chara") == 0) 
			{
				intcharaasm();
				vrnum++;
			}
            mid++;
        }
        //优化
        else
		{
            if (strcmp(midcode[mid].op, "+") == 0) addbetter();
            if (strcmp(midcode[mid].op, "-") == 0) subbetter();
            if (strcmp(midcode[mid].op, "*") == 0) mulbetter();
            if (strcmp(midcode[mid].op, "/") == 0) divbetter();
            if (strcmp(midcode[mid].op, ">") == 0) grebetter();
            if (strcmp(midcode[mid].op, ">=") == 0) geqbetter();
            if (strcmp(midcode[mid].op, "<") == 0) lssbetter();
            if (strcmp(midcode[mid].op, "<=") == 0) leqbetter();
            if (strcmp(midcode[mid].op, "!=") == 0) neqbetter();
            if (strcmp(midcode[mid].op, "==") == 0) eqlbetter();
            if (strcmp(midcode[mid].op, "=") == 0) assbetter();
            if (strcmp(midcode[mid].op, "[]=") == 0) aassbetter();
            if (strcmp(midcode[mid].op, "geta") == 0) assabetter();
            if (strcmp(midcode[mid].op, "lab:") == 0) setlabasm();
            if (strcmp(midcode[mid].op, "scf") == 0) scfbetter();
            if (strcmp(midcode[mid].op, "prt") == 0) prtbetter();         
            if (strcmp(midcode[mid].op, "jne") == 0) jneasm();
            if (strcmp(midcode[mid].op, "jmp") == 0) jmpasm();
            if (strcmp(midcode[mid].op, "fupa") == 0) fupabetter();
            if (strcmp(midcode[mid].op, "call") == 0) callbetter();
            if (strcmp(midcode[mid].op, "ret") == 0) retbetter();
            if (strcmp(midcode[mid].op, "para") == 0) 
			{
				paraasm();
				vrnum++;
			}
            if (strcmp(midcode[mid].op, "int") == 0|| strcmp(midcode[mid].op, "char") == 0) 
			{
                int flag = 0;
                while (strcmp(midcode[mid].op, "int") == 0|| strcmp(midcode[mid].op, "char") == 0) 
				{
                    flag = 1;
                    intcharasm();
                    mid++;
                }
                if (flag)   mid--;
                regopt();
            }
            if (strcmp(midcode[mid].op, "const") == 0) 
			{
				constdefasm();
				vrnum++;
			}
            if (strcmp(midcode[mid].op, "inta") == 0 || strcmp(midcode[mid].op, "chara") == 0) 
			{
				intcharaasm();
				vrnum++;
			}
            mid++;
        }
    }
    ap = globedge;
    fc.back().endap = ap;
    //恢复现场
    rsT << "__FEND_LAB_" << funcnum << ":" << endl;//结束开始
    rsT << "lw\t$ra\t0($fp)" << endl;//恢复$ra
    rsT << "lw\t$sp\t8($fp)" << endl;//退栈,恢复$sp
    rsT << "lw\t$fp\t4($fp)" << endl;//恢复上一个函数的fp
    load();
    if (ismain) 
	{
        rsT << "li\t$v0\t10" << endl;
        rsT << "syscall" << endl;//终止程序
    } 
	else 
	{
        rsT << "jr\t$ra\t" << endl;//返回
    }
    mid = mid + 1;
    return;
}
void asmaddr(int &addr1, int &addr2)
{
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;//加载立即数
	}
	else
	{
		addr1 = varaddr(midcode[mid].var1);
		if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;//取值
		else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		addr2 = varaddr(midcode[mid].var2);
		if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
		else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
	}
	return;
}
//para, int, , a == > f(int a, ...)
void paraasm() 
{
    paran = 0;
    for (int i = mid; i < midcnt; i++) 
	{
        if (strcmp(midcode[i].op, "para") == 0) paran++;
        else  break;
    }
    for (int i = 0; i < paran; i++) 
	{
        Symbol kind = (strcmp(midcode[mid].var1, "int") == 0) ? intty : charty;
        parainsertaddress(kind, 8 + 4 * ( paran - i ));//上一级函数调用，取值储存在上一级函数里，所以需要加8
        mid++;
    }
    mid--;
}
//fupa,   ,   , a     ==> a is a function parameter
void fupaasm() 
{
    if(savespflg == 0)
	{
        rsT << "move\t$s0\t$sp" << endl;
        savespflg=1;
    }
    if (isdigit(midcode[mid].var3[0])) 
	{
        rsT << "li\t$t0\t" << midcode[mid].var3 << endl;
    } 
	else 
	{
        rsT << "lw\t$t0\t" << varaddr(midcode[mid].var3); //lw  $t0 存放的偏移地址
        if(isglob) rsT << "($t9)" << endl;
        else rsT << "($fp)" << endl;       
    }
    rsT << "sw\t$t0\t($sp)" << endl;      //sw    $t0 $sp
    rsT << "subi\t$sp\t$sp\t4" << endl; //subi  $sp $sp 4
    sp -= 4;
}
//jmp ,  ,  ,
void jmpasm() 
{
    rsT << "j\t" << midcode[mid].var3 << endl;
}
// jne
void jneasm() 
{
    rsT << "bne\t$t0\t1\t" << midcode[mid].var3 << endl;   //bne,$t0,1,label
}
//call, f ,   , a
void callasm() 
{
    if(savespflg == 0)
	{
        rsT << "move\t$s0\t$sp" << endl;
		savespflg = 1;
    }
    rsT << "jal\t" << midcode[mid].var1 << endl;
    rsT << "nop\n";
    if (midcode[mid].var3[0] != ' ' && midcode[mid].var3[0] != '\0') 
	{
        int addr2;
        addr2 = varaddr(midcode[mid].var3);
        if (isglob) rsT << "sw\t$v0\t" << addr2 << "($t9)" << endl; // sw v0,addr(t9)
        else  rsT << "sw\t$v0\t" << addr2 << "($fp)" << endl; // sw v0,addr(fp)
    }
    savespflg=0;
}
//ret ,   ,   , (a)   ==> return a / return
void retasm() 
{
    if (midcode[mid].var3[0] != ' ' && midcode[mid].var3[0] != '\0') 
	{
        if (isdigit(midcode[mid].var3[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+') 
		{
            rsT << "li\t$v0\t" << midcode[mid].var3 << endl;
        } 
		else 
		{
            int addr2 = varaddr(midcode[mid].var3);
            if (isglob) rsT << "lw\t$v0\t" << addr2 << "($t9)" << endl;
            else rsT << "lw\t$v0\t" << addr2 << "($fp)" << endl;
        }
    }
    rsT << "j\t__FEND_LAB_" << funcnum << endl;//跳至结束
}
// lab,  ,  ,
void setlabasm() 
{
    rsT << midcode[mid].var3 << ":\n";
}
//add, a, b, c
void addasm() 
{
    int addr1, addr2, addr3;
	asmaddr(addr1, addr2);
    addr3 = varaddr(midcode[mid].var3);
    rsT << "add\t$t0\t$t0\t$t1" << endl;
    if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl;
    else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
}
//sub, a, b, c
void subasm() 
{
    int addr1, addr2, addr3;
	asmaddr(addr1, addr2);
    addr3 = varaddr(midcode[mid].var3);
    rsT << "sub\t$t0\t$t0\t$t1" << endl;
    if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl; //sw t0,addr3(t9)
    else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
}
//mul, a, b, c
void mulasm() 
{
    int addr1, addr2, addr3;
	asmaddr(addr1, addr2);
    addr3 = varaddr(midcode[mid].var3);
    rsT << "mul\t$t0\t$t0\t$t1" << endl;
    if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl;
    else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
}
//div, a, b, c
void divasm()
{
    int addr1, addr2, addr3;
	asmaddr(addr1, addr2);
    addr3 = varaddr(midcode[mid].var3);
    rsT << "div\t$t0\t$t0\t$t1" << endl;
    if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl;
    else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
}
//> , a, b, ,
void greasm()
{
    int addr1, addr2;
	asmaddr(addr1, addr2);
    // slt t0,t1,t0
    rsT << "slt\t$t0\t$t1\t$t0" << endl;
}
// >=
void geqasm()
{
    int addr1, addr2;
	asmaddr(addr1, addr2);
    rsT << "slt\t$t0\t$t0\t$t1" << endl;    //slt t0,t0,t1
    rsT << "li\t$t1\t1" << endl;            //li t1,1
    rsT << "sub\t$t0\t$t1\t$t0" << endl;   //sub t0,t1,t0
}
// <
void lssasm() 
{
    int addr1, addr2;
	asmaddr(addr1, addr2);
    rsT << "slt\t$t0\t$t0\t$t1" << endl;
}
//<=
void leqasm() 
{
    int addr1, addr2;
	asmaddr(addr1, addr2);
    rsT << "slt\t$t0\t$t1\t$t0" << endl;
    rsT << "li\t$t1\t1" << endl;
    rsT << "sub\t$t0\t$t1\t$t0" << endl;
}
//==
void eqlasm() 
{
    int addr1, addr2;
	asmaddr(addr1, addr2);
    int t1 = tlabelnum++;
    int t2 = tlabelnum++;
    rsT << "bne\t$t0\t$t1\t__tLABEL" << t1 << endl; //bne t0 t1 __tLABEL0
    rsT << "li\t$t0\t1" << endl;                    //li t0,1
    rsT << "j\t__tLABEL" << t2 << endl;             //j __tLABEL1
    rsT << "__tLABEL" << t1 << ":" << endl;         //__tLABEL0:
    rsT << "li\t$t0\t0" << endl;                    //li $t0,0
    rsT << "__tLABEL" << t2 << ":" << endl;         //__tLABEL1:
}
//!=
void neqasm() 
{
    int addr1, addr2;
	asmaddr(addr1, addr2);
    int t1 = tlabelnum++;
    int t2 = tlabelnum++;
    rsT << "beq\t$t0\t$t1\t__tLABEL" << t1 << endl;
    rsT << "li\t$t0\t1" << endl;
    rsT << "j\t__tLABEL" << t2 << endl;
    rsT << "__tLABEL" << t1 << ":" << endl;
    rsT << "li\t$t0\t0" << endl;
    rsT << "__tLABEL" << t2 << ":" << endl;
}
// =
void assasm() 
{
    int addr1, addr2;
    if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+') 
	{
        rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
    } 
	else 
	{
        addr1 = varaddr(midcode[mid].var1);
        if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
        else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    addr2 = varaddr(midcode[mid].var3);
    if (isglob) rsT << "sw\t$t0\t" << addr2 << "($t9)" <<"  #   "<< midcode[mid].var3<< endl;
    else rsT << "sw\t$t0\t" << addr2 << "($fp)" <<"  #   "<< midcode[mid].var3<< endl;
}
// []= , a , i , t a[i]=t
void aassasm() 
{
    int addr1, addr2, addri;
    addr1 = varaddr(midcode[mid].var1);
    int tisglob = isglob;
    //[i]不是表达式
    if (isdigit(midcode[mid].var2[0])) 
	{
        addr1 -= (atoi(midcode[mid].var2) * 4);
        if (isdigit(midcode[mid].var3[0]) || midcode[mid].var3[0] == '-' || midcode[mid].var3[0] == '+')
		{
            rsT << "li\t$t0\t" << midcode[mid].var3 << endl;       //t0=var3
        } 
		else
		{
            addr2 = varaddr(midcode[mid].var3);
            if (isglob)  rsT << "lw\t$t0\t" << addr2 << "($t9)" << endl;
            else rsT << "lw\t$t0\t" << addr2 << "($fp)" << endl;
        }
        if (tisglob) rsT << "sw\t$t0\t" << addr1 << "($t9)" << endl;
        else rsT << "sw\t$t0\t" << addr1 << "($fp)" << endl;
    } 
	else 
	{
        //求数组元素a[i]地址
        addri = varaddr(midcode[mid].var2);//addrt = &i
        if (isglob) rsT << "lw\t$t1\t" << addri << "($t9)" << endl;    //t1 = i
        else rsT << "lw\t$t1\t" << addri << "($fp)" << endl;    //t1 = i
        rsT << "mul\t$t1\t$t1\t-4\n";    //t1 = t1 * (-4) (t1 = offset)
        rsT << "addi\t$t1\t$t1\t" << addr1 << endl;    //t1 = addr1+t1
        if (isdigit(midcode[mid].var3[0]) || midcode[mid].var3[0] == '-' || midcode[mid].var3[0] == '+') 
		{
            rsT << "li\t$t0\t" << midcode[mid].var3 << endl;
        } 
		else 
		{
            addr2 = varaddr(midcode[mid].var3);
            if (isglob) rsT << "lw\t$t0\t" << addr2 << "($t9)" << endl;
            else rsT << "lw\t$t0\t" << addr2 << "($fp)" << endl;         
        }
        if (tisglob)
		{
            rsT << "add\t$t1\t$t1\t$t9" << endl;
            rsT << "sw\t$t0\t0($t1)" << endl;
        }
        else
		{
            rsT << "add\t$t1\t$t1\t$fp" << endl;
            rsT << "sw\t$t0\t0($t1)" << endl;
        }
    }
}
//geta, a, n, b         b=a[n]
void assaasm() 
{
    int addr1 = varaddr(midcode[mid].var1);
    int tisglob = isglob;
    int addr2;
    int t1 = findvartable(midcode[mid].var3);
    if(midcode[mid].var3[0] == '$' && addrtable[t1].kind == none)
	{
            int t0 = findvartable(midcode[mid].var1);
            Symbol kind0 = addrtable[t0].kind;
			addrtable[t1].kind = kind0;
    }
    if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[ 0 ] == '+') 
	{
        addr1 -= (atoi(midcode[mid].var2) * 4);    //addr1 = &a[n]
        addr2 = varaddr(midcode[mid].var3);        //addr2 = &b
        if (tisglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;   
		else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
        if (isglob) rsT << "sw\t$t0\t" << addr2 << "($t9)" << endl;
        else rsT << "sw\t$t0\t" << addr2 << "($fp)" << endl;
    } 
	else 
	{
        //求数组元素a[i]地址
        int addri = varaddr(midcode[mid].var2);//addrt = &i
        if (isglob) rsT << "lw\t$t1\t" << addri << "($t9)" << endl;    //t1 = i
        else rsT << "lw\t$t1\t" << addri << "($fp)" << endl;    //t1 = i
        rsT << "mul\t$t1\t$t1\t-4\n";    //t1 = t1 * (-4) (t1 = offset)
        rsT << "addi\t$t1\t$t1\t" << addr1 << endl;    //t1=$a[i]偏移量
        addr2 = varaddr(midcode[mid].var3);        //addr2 = &b
        if (tisglob) 
		{
            rsT << "add\t$t1\t$t1\t$t9" << endl;
            rsT << "lw\t$t0\t0"  << "($t1)" << endl;
        }
		else
		{
            rsT << "add\t$t1\t$t1\t$fp" << endl;
            rsT << "lw\t$t0\t0"  << "($t1)" << endl;
        }
        if (isglob) rsT << "sw\t$t0\t" << addr2 << "($t9)" << endl;
        else rsT << "sw\t$t0\t" << addr2 << "($fp)" << endl;
    }
}
//scf ,   ,   , a
void scfasm() 
{
    int addr = varaddr(midcode[mid].var3);
    int ti = findvartable(midcode[mid].var3);
    Symbol kind = addrtable[ti].kind;
    if (kind == intty)
	{
        rsT << "li\t$v0\t5" << endl;//5是输入int指令
        rsT << "syscall" << endl;
        if (isglob) rsT << "sw\t$v0\t" << addr << "($t9)" << endl;
        else rsT << "sw\t$v0\t" << addr << "($fp)" << endl;
    }
	else 
	{
        rsT << "li\t$v0\t12" << endl;//12是输入char
        rsT << "syscall" << endl;
        if (isglob) rsT << "sw\t$v0\t" << addr << "($t9)" << endl;
        else rsT << "sw\t$v0\t" << addr << "($fp)" << endl;
    }
}
//prt,string,expression,int/char
void prtasm() 
{
    int addr;
    if (midcode[mid].var1[0] != '\0') 
	{
        int len = strlen(midcode[mid].var1);
        for (int i = 1; i < len - 1; i++) 
		{
            rsT << "li\t$v0\t11" << endl;//11是输出char
            rsT << "li\t$a0\t" << int(midcode[mid].var1[i]) << endl;
            rsT << "syscall" << endl;
        }
    }
    if (midcode[mid].var2[0] != ' ' && midcode[mid].var2[0] != '\0') 
	{
        if (isdigit(midcode[mid].var2[0]) && strcmp(midcode[mid].var3, "char") == 0) 
		{
            rsT << "li\t$v0\t11" << endl;
            rsT << "li\t$a0\t" << int(midcode[mid].var2) << endl;
            rsT << "syscall" << endl;
            rsT << "li\t$v0\t4" << endl;
            rsT << "la\t$a0\t" << "Enter" << endl;
            rsT << "syscall" << endl;
            return;
        } 
		else if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+') 
		{
            rsT << "li\t$v0\t1" << endl;
            rsT << "li\t$a0\t" << midcode[mid].var2 << endl;
            rsT << "syscall" << endl;
            rsT << "li\t$v0\t4" << endl;
            rsT << "la\t$a0\t" << "Enter" << endl;
            rsT << "syscall" << endl;
            return;
        }
        addr = varaddr(midcode[mid].var2);
        int ti = findvartable(midcode[mid].var2);
        Symbol kind = addrtable[ti].kind;
        if (kind == intty) 
		{
            rsT << "li\t$v0\t1" << endl;    //li $v0,1 输出int 
            if (isglob) rsT << "lw\t$a0\t" << addr << "($t9)" << endl;
            else rsT << "lw\t$a0\t" << addr << "($fp)" << endl;
            rsT << "syscall" << endl;
        } 
		else 
		{
            rsT << "li\t$v0\t11" << endl;
            if (isglob)  rsT << "lw\t$a0\t" << addr << "($t9)" << endl;
            else  rsT << "lw\t$a0\t" << addr << "($fp)" << endl;
            rsT << "syscall" << endl;
        }
        rsT << "li\t$v0\t4" << endl;
        rsT << "la\t$a0\t" << "Enter" << endl;
        rsT << "syscall" << endl;
    }
}
void intcharasm() 
{
    if (isdigit(midcode[mid].var2[0])) pushstack(midcode[mid].var2,1);
    else pushstack("0",1);  
    if (strcmp(midcode[mid].op, "int") == 0) insertaddress(intty,mid,1);
    else insertaddress(charty,mid,1);   
}
void constdefasm() 
{
    //常量定义
    while (strcmp(midcode[mid].op, "const") == 0) 
	{
        pushstack(midcode[mid].var2,1);
        if (strcmp(midcode[mid].var1, "int") == 0) insertaddress(intty,mid,1);
        else insertaddress(charty,mid,1);       
        mid++;
    }
    mid--;
}
void intcharaasm() 
{
    //数组定义
    while (strcmp(midcode[mid].op, "inta") == 0 || strcmp(midcode[mid].op, "chara") == 0) 
	{
        pushstack("0", atoi(midcode[mid].var2));
        if (strcmp(midcode[mid].op, "inta") == 0) insertaddress(intty,mid,atoi(midcode[mid].var2));
        else insertaddress(charty,mid,atoi(midcode[mid].var2));        
        mid++;
    }
    mid--;
}
void save() 
{
    if (OPT) 
	{
        for (int i = 1; i < 8; i++) 
		{
            rsT << "sw\t$s" << i << "\t" << 4 * i << "($t8)" << endl;//.data段
        }
    }
}
void load() 
{
    if (OPT)
	{
        for (int i = 1; i < 8; i++)
		{
            rsT << "lw\t$s" << i << "\t" << 4 * i << "($t8)" << endl;
        }
    }
}