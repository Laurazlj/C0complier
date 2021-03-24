#include <cstdlib>
#include <string.h>
#include <iostream>
#include <fstream>
#include "main.h"
#include <vector>

using namespace std;

int tindex = 0;

#define rsT mipsWrite

//优化部分
void better()
{
	while (strcmp(midcode[tindex].op, "func") != 0) tindex++;//找到函数，函数外不会有数据操作
	while (tindex < midcnt)
	{
		combine();
		delcommon();
		tindex++;
	}
	delsetlab();
	return;
}
int ifasscom(int i) 
{
	int tempi = i + 1;
	int flg = 0;
	do 
	{
		if (strcmp(midcode[tempi].op, "=") == 0) 
		{//检测第一个操作数，第二个操作数是否是下面操作的第三个操作数
			if (strcmp(midcode[tempi].var3, midcode[i].var1) == 0 || strcmp(midcode[tempi].var3, midcode[i].var2) == 0)
			{
				flg = 1;
				break;
			}
		}
		tempi++;
	} while (strcmp(midcode[tempi].op, "end") != 0);
	if (flg == 1)return 1;
	return 0;
}
//删除公共子表达式
void delcommon()
{
	int i, j, h, k;
	//划分基本块
	//基本块中的子表达式A
	for (i = tindex; strcmp(midcode[i].op, "+") == 0 || strcmp(midcode[i].op, "-") == 0 || 
		strcmp(midcode[i].op, "*") == 0 || strcmp(midcode[i].op, "/") == 0 || strcmp(midcode[i].op, "=") == 0; i++) 
	{
		if (midcode[i].var3[0] == '$') 
		{
			if (ifasscom(i) == 1) continue;
		}
		if (i >= midcnt)  return;
		//i的结果操作数为中间变量
		//t1=m-n - ,m ,n ,t1
		//m=1  = ,1 , , m 
	    //t2=m-n - ,m, n ,t2
	    //t3=m-n
		//t4=t3-x
		if (midcode[i].var3[0] == '$') 
		{
			//寻找四元式基本块中序号i以后与i相同的子表达式B
			for (j = i + 1; strcmp(midcode[j].op, "+") == 0 || strcmp(midcode[j].op, "-") == 0 || strcmp(midcode[j].op, "*") == 0 
				|| strcmp(midcode[j].op, "/") == 0 || strcmp(midcode[j].op, "=") == 0; j++)
			{
				if (j >= midcnt)  return;
				//寻找公共子表达式(A,B)，B的结果数也为临时变量
				if (strcmp(midcode[j].op, midcode[i].op) == 0 && strcmp(midcode[j].var1, midcode[i].var1) == 0
					&& strcmp(midcode[j].var2, midcode[i].var2) == 0 && midcode[j].var3[0] == '$') 
				{
					//找到之后寻找B之后的子表达式与B的var3相同的中间变量，修改Ci中变量的名字为A的结果数
					for (h = j + 1; strcmp(midcode[h].op, "+") == 0 || strcmp(midcode[h].op, "-") == 0 || 
						strcmp(midcode[h].op, "*") == 0 || strcmp(midcode[h].op, "/") == 0 || strcmp(midcode[h].op, "=") == 0; h++) 
					{
						if (h >= midcnt)  return;
						if (strcmp(midcode[h].var1, midcode[j].var3) == 0)
							strcpy(midcode[h].var1, midcode[i].var3);
						if (strcmp(midcode[h].var2, midcode[j].var3) == 0)
							strcpy(midcode[h].var2, midcode[i].var3);
					}
					//删除B
					for (k = j; k < midcnt; k++) 
					{
						strcpy(midcode[k].op, midcode[k + 1].op);
						strcpy(midcode[k].var1, midcode[k + 1].var1);
						strcpy(midcode[k].var2, midcode[k + 1].var2);
						strcpy(midcode[k].var3, midcode[k + 1].var3);
					}
					midcnt--;
					j--;
				}
			}
		}
	}
}
int IsDigit(char* name)
{
	int i = 0;
	if (name[i] == '-') i++;
	while (name[i] != '\0') 
	{
		if (name[i] > '9' || name[i] < '0') return 0;
		i++;
	}
	return 1;
}
//合并常数
void combine() 
{
	int i = tindex, num1, num2, temp;
	char sum[30];
	while (strcmp(midcode[i].op, "+") == 0 || strcmp(midcode[i].op, "-") == 0 || strcmp(midcode[i].op, "*") == 0 || strcmp(midcode[i].op, "/") == 0) 
	{
		if (IsDigit(midcode[i].var1) && IsDigit(midcode[i].var2)) 
		{
			num1 = atoi(midcode[i].var1);
			num2 = atoi(midcode[i].var2);
			if (strcmp(midcode[i].op, "+") == 0) temp = num1 + num2;
			if (strcmp(midcode[i].op, "-") == 0) temp = num1 - num2;
			if (strcmp(midcode[i].op, "*") == 0) temp = num1 * num2;
			if (strcmp(midcode[i].op, "/") == 0) temp = num1 / num2;
			sprintf(sum, "%d", temp);
			strcpy(midcode[i].op, "=");
			strcpy(midcode[i].var1, sum);
			strcpy(midcode[i].var2, "  ");
		}
		i++;
	}
}
//删除冗余跳转代码
void delsetlab()
{
	int i, k, j, t, flag;
	char temp[30][10];
	for (i = 0; i < midcnt; i++) 
	{
		if (i >= midcnt) return;
		if (strcmp(midcode[i].op, "lab:") == 0) 
		{
			j = 0;
			flag = i;
			i = i + 1;
			while (strcmp(midcode[i].op, "lab:") == 0) 
			{
				strcpy(temp[j++], midcode[i].var3);
				for (k = i; k < midcnt; k++) 
				{
					strcpy(midcode[k].op, midcode[k + 1].op);     
					strcpy(midcode[k].var1, midcode[k + 1].var1);
					strcpy(midcode[k].var2, midcode[k + 1].var2);
					strcpy(midcode[k].var3, midcode[k + 1].var3);
				}
				midcnt--;
			}
			if (j == 0) continue;
			for (k = 0; k <= midcnt; k++)
			{
				if (k >= midcnt) return;
				if (strcmp(midcode[k].op, "jmp") == 0 || strcmp(midcode[k].op, "jne") == 0) 
				{
					for (t = 0; t < j; t++) 
					{
						if(strcmp(midcode[k].var3, temp[t]) == 0)
							strcpy(midcode[k].var3, midcode[flag].var3);
					}
				}
			}
		}
	}
}
void fupabetter()
{
	if (savespflg == 0)
	{
		rsT << "move\t$s0\t$sp" << endl;
		savespflg = 1;
	}
	if (isdigit(midcode[mid].var3[0]))  rsT << "li\t$t0\t" << midcode[mid].var3 << endl;
	else
	{
		if (varreg[varaddrt(midcode[mid].var3)] == -1)
		{
			rsT << "lw\t$t0\t" << varaddr(midcode[mid].var3); //li    $t0 item
			if (isglob)  rsT << "($t9)" << endl;
			else  rsT << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var3)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	rsT << "sw\t$t0\t($sp)" << endl;      //sw    $t0 $sp
	rsT << "subi\t$sp\t$sp\t4" << endl; //subi  $sp $sp 4
	sp -= 4;
}
//call, f ,   , a
void callbetter()
{
	if (savespflg == 0)
	{
		rsT << "move\t$s0\t$sp" << endl;
		savespflg = 1;
	}
	rsT << "jal\t" << midcode[mid].var1 << endl;
	rsT << "nop\n";
	if (midcode[mid].var3[0] != ' ' && midcode[mid].var3[0] != '\0')
	{
		if (varreg[varaddrt(midcode[mid].var3)] == -1)
		{
			int addr2;
			addr2 = varaddr(midcode[mid].var3);
			if (isglob) rsT << "sw\t$v0\t" << addr2 << "($t9)" << endl; // sw v0,0(t9)
			else rsT << "sw\t$v0\t" << addr2 << "($fp)" << endl; // sw v0,0(fp)
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var3)];
			rsT << "add\t$s" << regnum << "\t$v0\t$0" << endl;//add $si $v0 $0
		}
	}
	savespflg = 0;
}
//ret ,   ,   , (a)   ==> return a / return
void retbetter() 
{
	if (midcode[mid].var3[0] != ' ' && midcode[mid].var3[0] != '\0') 
	{
		if (isdigit(midcode[mid].var3[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+') 
		{
			rsT << "li\t$v0\t" << midcode[mid].var3 << endl;
		}
		else 
		{
			if (varreg[varaddrt(midcode[mid].var3)] == -1) 
			{
				int addr2 = varaddr(midcode[mid].var3);
				if (isglob) rsT << "lw\t$v0\t" << addr2 << "($t9)" << endl;
				else rsT << "lw\t$v0\t" << addr2 << "($fp)" << endl;
			}
			else 
			{
				int regnum = varreg[varaddrt(midcode[mid].var3)];
				rsT << "add\t$v0\t$s" << regnum << "\t$0" << endl;//add $v0 $si $0
			}
		}
	}
	rsT << "j\t__FEND_LAB_" << funcnum << endl;//跳至结束
}
//add, a, b, c
void addbetter()
{
	int addr1, addr2, addr3;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{	
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;//li t0,var1
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (varreg[varaddrt(midcode[mid].var3)] == -1)
	{
		addr3 = varaddr(midcode[mid].var3);
		rsT << "add\t$t0\t$t0\t$t1" << endl;
		if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl;
		else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
	}
	else
	{
		rsT << "add\t$t0\t$t0\t$t1" << endl;
		int regnum = varreg[varaddrt(midcode[mid].var3)];
		rsT << "add\t$s" << regnum << "\t$t0\t$0" << endl;//add $si $t0 $0
	}
}
//sub, a, b, c
void subbetter()
{
	int addr1, addr2, addr3;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}

	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (varreg[varaddrt(midcode[mid].var3)] == -1)
	{
		addr3 = varaddr(midcode[mid].var3);
		rsT << "sub\t$t0\t$t0\t$t1" << endl;
		if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl; //sw t0,addr3(t9)
		else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
	}
	else
	{
		rsT << "sub\t$t0\t$t0\t$t1" << endl;
		int regnum = varreg[varaddrt(midcode[mid].var3)];
		rsT << "add\t$s" << regnum << "\t$t0\t$0" << endl;//add $si $t0 $0
	}
}
//mul, a, b, c
void mulbetter()
{
	int addr1, addr2, addr3;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (varreg[varaddrt(midcode[mid].var3)] == -1)
	{
		addr3 = varaddr(midcode[mid].var3);
		rsT << "mul\t$t0\t$t0\t$t1" << endl;
		if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl;
		else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
	}
	else
	{
		rsT << "mul\t$t0\t$t0\t$t1" << endl;
		int regnum = varreg[varaddrt(midcode[mid].var3)];
		rsT << "add\t$s" << regnum << "\t$t0\t$0" << endl;//add $si $t0 $0
	}
}
// div, a, b, c
void divbetter()
{
	int addr1, addr2, addr3;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (varreg[varaddrt(midcode[mid].var3)] == -1)
	{
		addr3 = varaddr(midcode[mid].var3);
		rsT << "div\t$t0\t$t0\t$t1" << endl;
		if (isglob) rsT << "sw\t$t0\t" << addr3 << "($t9)" << endl;
		else rsT << "sw\t$t0\t" << addr3 << "($fp)" << endl;
	}
	else
	{
		rsT << "div\t$t0\t$t0\t$t1" << endl;
		int regnum = varreg[varaddrt(midcode[mid].var3)];
		rsT << "add\t$s" << regnum << "\t$t0\t$0" << endl;//add $si $t0 $0
	}
}
// > , a, b, c
void grebetter()
{
	int addr1, addr2;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	rsT << "slt\t$t0\t$t1\t$t0" << endl;	// slt t0,t1,t0
}
// >=
void geqbetter()
{
	int addr1, addr2;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	rsT << "slt\t$t0\t$t0\t$t1" << endl;    //slt t0,t0,t1
	rsT << "li\t$t1\t1" << endl;            //li t1,1
	rsT << "sub\t$t0\t$t1\t$t0" << endl;   //sub t0,t1,t0
}
// <
void lssbetter()
{
	int addr1, addr2;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	rsT << "slt\t$t0\t$t0\t$t1" << endl;
}
//    <=
void leqbetter()
{
	int addr1, addr2;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	rsT << "slt\t$t0\t$t1\t$t0" << endl;
	rsT << "li\t$t1\t1" << endl;
	rsT << "sub\t$t0\t$t1\t$t0" << endl;
}
// ==
void eqlbetter()
{
	int addr1, addr2;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	int t1 = tlabelnum++;
	int t2 = tlabelnum++;
	rsT << "bne\t$t0\t$t1\t__tLABEL" << t1 << endl; //bne t0 t1 __tLABEL0
	rsT << "li\t$t0\t1" << endl;                    //li t0,1
	rsT << "j\t__tLABEL" << t2 << endl;             //j __tLABEL1
	rsT << "__tLABEL" << t1 << ":" << endl;             //__tLABEL0:
	rsT << "li\t$t0\t0" << endl;                    //li $t0,0
	rsT << "__tLABEL" << t2 << ":" << endl;             //__tLABEL1:
}
//    !=
void neqbetter()
{
	int addr1, addr2;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		rsT << "li\t$t1\t" << midcode[mid].var2 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addr2 = varaddr(midcode[mid].var2);
			if (isglob) rsT << "lw\t$t1\t" << addr2 << "($t9)" << endl;
			else rsT << "lw\t$t1\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	int t1 = tlabelnum++;
	int t2 = tlabelnum++;
	rsT << "beq\t$t0\t$t1\t__tLABEL" << t1 << endl;
	rsT << "li\t$t0\t1" << endl;
	rsT << "j\t__tLABEL" << t2 << endl;
	rsT << "__tLABEL" << t1 << ":" << endl;
	rsT << "li\t$t0\t0" << endl;
	rsT << "__tLABEL" << t2 << ":" << endl;
}
//=
void assbetter()
{
	int addr1, addr2;
	if (isdigit(midcode[mid].var1[0]) || midcode[mid].var1[0] == '-' || midcode[mid].var1[0] == '+')
	{
		rsT << "li\t$t0\t" << midcode[mid].var1 << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var1)] == -1)
		{
			addr1 = varaddr(midcode[mid].var1);
			if (isglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
			else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var1)];
			rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
	}
	if (varreg[varaddrt(midcode[mid].var3)] == -1)
	{
		addr2 = varaddr(midcode[mid].var3);
		if (isglob) rsT << "sw\t$t0\t" << addr2 << "($t9)" << "  #   " << midcode[mid].var3 << endl;
		else rsT << "sw\t$t0\t" << addr2 << "($fp)" << "  #   " << midcode[mid].var3 << endl;
	}
	else
	{
		int regnum = varreg[varaddrt(midcode[mid].var3)];
		rsT << "add\t$s" << regnum << "\t$t0\t$0" << endl;//add $si $t0 $0
	}
}
// []= , a , i , t
void aassbetter()
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
			if (varreg[varaddrt(midcode[mid].var3)] == -1)
			{
				addr2 = varaddr(midcode[mid].var3);
				if (isglob) rsT << "lw\t$t0\t" << addr2 << "($t9)" << endl;
				else rsT << "lw\t$t0\t" << addr2 << "($fp)" << endl;
			}
			else
			{
				int regnum = varreg[varaddrt(midcode[mid].var3)];
				rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
			}
		}
		if (tisglob) rsT << "sw\t$t0\t" << addr1 << "($t9)" << endl;
		else rsT << "sw\t$t0\t" << addr1 << "($fp)" << endl;
	}
	else
	{
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			addri = varaddr(midcode[mid].var2);//addrt = &i
			if (isglob) rsT << "lw\t$t1\t" << addri << "($t9)" << endl;    //t1 = i
			else rsT << "lw\t$t1\t" << addri << "($fp)" << endl;    //t1 = i
		}
		//求数组元素a[i]地址
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
		rsT << "mul\t$t1\t$t1\t-4\n";    //t1 = t1 * 4 (t1 = offset)
		rsT << "addi\t$t1\t$t1\t" << addr1 << endl;    //t1 = addr1+t1
		if (isdigit(midcode[mid].var3[0]) || midcode[mid].var3[0] == '-' || midcode[mid].var3[0] == '+')
		{
			rsT << "li\t$t0\t" << midcode[mid].var3 << endl;
		}
		else
		{
			//t0=var3
			if (varreg[varaddrt(midcode[mid].var3)] == -1)
			{
				addr2 = varaddr(midcode[mid].var3);
				if (isglob) rsT << "lw\t$t0\t" << addr2 << "($t9)" << endl;
				else rsT << "lw\t$t0\t" << addr2 << "($fp)" << endl;
			}
			else
			{
				int regnum = varreg[varaddrt(midcode[mid].var3)];
				rsT << "add\t$t0\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
			}
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
void assabetter()
{
	int addr1 = varaddr(midcode[mid].var1);
	int tisglob = isglob;
	int addr2;
	int t1 = findvartable(midcode[mid].var3);
	if (midcode[mid].var3[0] == '$' && addrtable[t1].kind == none)
	{
		int t0 = findvartable(midcode[mid].var1);
		Symbol kind0 = addrtable[t0].kind;
		addrtable[t1].kind = kind0;
	}
	if (isdigit(midcode[mid].var2[0]) || midcode[mid].var2[0] == '-' || midcode[mid].var2[0] == '+')
	{
		addr1 -= (atoi(midcode[mid].var2) * 4);    //addr1 = &a[n]
		addr2 = varaddr(midcode[mid].var3);        //addr2 = &b
		if (tisglob) rsT << "lw\t$t0\t" << addr1 << "($t9)" << endl;
		else rsT << "lw\t$t0\t" << addr1 << "($fp)" << endl;
		if (varreg[varaddrt(midcode[mid].var3)] == -1)
		{
			if (isglob) rsT << "sw\t$t0\t" << addr2 << "($t9)" << endl;
			else rsT << "sw\t$t0\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var3)];
			rsT << "add\t$s" << regnum << "\t$t0\t$0" << endl;//add $si $t0 $0
		}
	}
	else
	{
		//求数组元素a[i]地址
		if (varreg[varaddrt(midcode[mid].var2)] == -1)
		{
			int addri = varaddr(midcode[mid].var2);//addrt = &i
			if (isglob) rsT << "lw\t$t1\t" << addri << "($t9)" << endl;    //t1 = i
			else rsT << "lw\t$t1\t" << addri << "($fp)" << endl;    //t1 = i
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var2)];
			rsT << "add\t$t1\t$s" << regnum << "\t$0" << endl;//add $t0 $si $0
		}
		rsT << "mul\t$t1\t$t1\t-4\n";    //t1 = t1 * 4 (t1 = offset)
		rsT << "addi\t$t1\t$t1\t" << addr1 << endl;    //t1=$a[i]偏移量
		addr2 = varaddr(midcode[mid].var3);        //addr2 = &b
		if (tisglob)
		{
			rsT << "add\t$t1\t$t1\t$t9" << endl;
			rsT << "lw\t$t0\t0" << "($t1)" << endl;
		}
		else
		{
			rsT << "add\t$t1\t$t1\t$fp" << endl;
			rsT << "lw\t$t0\t0" << "($t1)" << endl;
		}
		if (varreg[varaddrt(midcode[mid].var3)] == -1)
		{
			if (isglob) rsT << "sw\t$t0\t" << addr2 << "($t9)" << endl;
			else rsT << "sw\t$t0\t" << addr2 << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var3)];
			rsT << "add\t$s" << regnum << "\t$t0\t$0" << endl;//add $si $t0 $0
		}
	}
}
//scf ,   ,   , a
void scfbetter()
{
	int addr = varaddr(midcode[mid].var3);
	int ti = findvartable(midcode[mid].var3);
	Symbol kind = addrtable[ti].kind;
	if (kind == intty)
	{
		rsT << "li\t$v0\t5" << endl;
		rsT << "syscall" << endl;
		if (varreg[varaddrt(midcode[mid].var3)] == -1)
		{
			if (isglob) rsT << "sw\t$v0\t" << addr << "($t9)" << endl;
			else rsT << "sw\t$v0\t" << addr << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var3)];
			rsT << "add\t$s" << regnum << "\t$v0\t$0" << endl;//add $si $t0 $0
		}
	}
	else
	{
		rsT << "li\t$v0\t12" << endl;
		rsT << "syscall" << endl;
		if (varreg[varaddrt(midcode[mid].var3)] == -1)
		{
			if (isglob) rsT << "sw\t$v0\t" << addr << "($t9)" << endl;
			else rsT << "sw\t$v0\t" << addr << "($fp)" << endl;
		}
		else
		{
			int regnum = varreg[varaddrt(midcode[mid].var3)];
			rsT << "add\t$s" << regnum << "\t$v0\t$0" << endl;//add $si $t0 $0
		}
	}
}
//prt,string,expression,int/char
void prtbetter()
{
	int addr;
	if (midcode[mid].var1[0] != '\0')
	{
		int len = strlen(midcode[mid].var1);
		for (int i = 1; i < len - 1; i++)
		{
			rsT << "li\t$v0\t11" << endl;
			rsT << "li\t$a0\t" << int(midcode[mid].var1[i]) << endl;
			rsT << "syscall" << endl;
		}
	}
	if (midcode[mid].var2[0] != ' ' && midcode[mid].var2[0] != '\0')
	{
		if (isdigit(midcode[mid].var2[0]) && strcmp(midcode[mid].var3, "char") == 0)
		{
			rsT << "li\t$v0\t11" << endl;
			rsT << "li\t$a0\t" << midcode[mid].var2 << endl;
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
			rsT << "li\t$v0\t1" << endl;    //li $v0,1
			if (varreg[varaddrt(midcode[mid].var2)] == -1)
			{
				if (isglob) rsT << "lw\t$a0\t" << addr << "($t9)" << endl;
				else rsT << "lw\t$a0\t" << addr << "($fp)" << endl;
			}
			else
			{
				int regnum = varreg[varaddrt(midcode[mid].var2)];
				rsT << "add\t$a0\t$s" << regnum << "\t$0" << endl;//add $a0 $si $0
			}
			rsT << "syscall" << endl;
		}
		else
		{
			rsT << "li\t$v0\t11" << endl;
			if (varreg[varaddrt(midcode[mid].var2)] == -1)
			{
				if (isglob) rsT << "lw\t$a0\t" << addr << "($t9)" << endl;
				else  rsT << "lw\t$a0\t" << addr << "($fp)" << endl;
			}
			else
			{
				int regnum = varreg[varaddrt(midcode[mid].var2)];
				rsT << "add\t$a0\t$s" << regnum << "\t$0" << endl;//add $a0 $si $0
			}
			rsT << "syscall" << endl;
		}
		rsT << "li\t$v0\t4" << endl;
		rsT << "la\t$a0\t" << "Enter" << endl;
		rsT << "syscall" << endl;
	}
}