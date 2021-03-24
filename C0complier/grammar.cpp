#include <cstdlib>
#include <string.h>
#include <iostream>
#include <fstream>
#include "main.h"
#include <vector>

using namespace std;

int  value;     //保存常量的值；对于子程序，0表示void，1表示int
int  address;   //变量的地址；函数返回值的地址
int  paranum;   //全局变量，用来处理函数定义，因为函数的参数个数只能在参数表子程序执行结束之后才知道，所以只能在后面才插入符号表信息
Symbol  kind;      //正在处理的标志符的类型
int tpkind[MAXPARANUM];
int returnnum = 0;
int termtk=0;
char exprevalue[200];
char funcname[100];
int globdefine = 1;
char name[MAXIDENLEN]; //标志符的名字，包括常量变量和函数
 
typedef struct{
    char name[MAXIDENLEN];  //identifier name
    Symbol kind;               //标识符类型：intty,charty,cintet,ccharet,function,paraint,parachar
    int value;              //对于函数来说，2表示返回值为char，1表示返回值为int 0返回值为void
    int address;            //address，元素在符号表element数组中的下标
    int paranum;            //对函数来说的参数个数
    int parakind[MAXPARANUM];//存储各个参数类型
}tableelement;
typedef struct{
    tableelement element[MAXSYMTABLENUM];//符号表内容
    int index; //符号表中个数
    int ftotal; //分程序总数
    int findextable[MAXSYMTABLENUM];//分程序索引数组
}symtable;
symtable symboltable;
//push符号表
void pushtable(char* name, Symbol kind, int value, int address, int paranum, int isVec = 0)
{
    //table is full
    if (symboltable.index >= MAXSYMTABLENUM )
	{
        error(outoftable);
		cout << "Idtable full." << endl;
		return;
    }
    //conflict
    //function
    if (kind == function) 
	{
        int iter = 1;
        for (iter = 1; iter <= symboltable.ftotal; iter++ )
		{
            if (strcmp(symboltable.element[symboltable.findextable[iter]].name, name)==0 )
			{
				error(symbolconflict);
				cout << "Variable name conflict." << endl;
                return;
            }
        }
        symboltable.findextable[++symboltable.ftotal] = symboltable.index;
    }
    else
	{
        //非函数   在函数内部找是否重名局部变量
        int iter = symboltable.findextable[symboltable.ftotal];
        for (;iter < symboltable.index;iter++) 
		{
            if (strcmp(symboltable.element[iter].name, name) == 0) 
			{
                error(symbolconflict);
				cout << "Variable name conflict." << endl;
                return;
            }
        }
        //全局变量范围内找与全局变量是否重名
        if (globdefine) 
		{
            iter = 0;
            while (iter < symboltable.findextable[1])
			{
                if (strcmp(symboltable.element[iter].name, name) ==0 ) 
				{
                    error(symbolconflict);
					cout << "Variable name conflict." << endl;
                    return;
                }
                iter++;
            }
        }
    }
    strcpy(symboltable.element[symboltable.index].name, name);
	symboltable.element[symboltable.index].kind = kind;
    symboltable.element[symboltable.index].value = value;
    symboltable.element[symboltable.index].address = symboltable.index;
    symboltable.element[symboltable.index].paranum = isVec ? isVec : paranum;
    symboltable.index++;
}
void updatepara(int paranum,int* tkind)
{
    int x = symboltable.findextable[symboltable.ftotal];   //array[i]，指向的是当前正在被分析的函数在符号表中的索引
    symboltable.element[x].paranum = paranum;//插入函数参数个数信息
    for(int i = 0; i < paranum; i++)
	{
		symboltable.element[x].parakind[i] = tkind[i];
    }
    return;
}
int arrnum(char *name)
{
    int n;
    int m = symboltable.findextable[1];
    n = symboltable.findextable[symboltable.ftotal];
    //查找符号表中是否有
    while (n < symboltable.index) 
	{
        if (strcmp(symboltable.element[n].name, name) == 0 ) return symboltable.element[n].paranum;
        n++;
    }
    //如果分程序符号表中无定义，查找最外层符号表
    if (n == symboltable.index) 
	{
        n = 0;
        while (n < m) 
		{
            if (strcmp(symboltable.element[n].name, name) == 0) return symboltable.element[n].paranum;
            n++;
        }
        //符号表中没有定义
        if (n == m) 
		{
            printf("\"%s\" ", name);
            return -1;
        }
    }
	return -1;
}
//查符号表：flag=1时，说明要在分程序索引里查找，flag=0时，在当前所在分程序符号表和最外层全局变量中查找
int searchst(char* name, int flag)
{
    int n;
    int m = symboltable.findextable[1];
    if (flag == 1) 
	{
        n = 1;
        while (n <= symboltable.ftotal) 
		{
            if (strcmp(symboltable.element[symboltable.findextable[n]].name, name) == 0) break;
            n++;
        }
        //如果函数标志符没有定义
        if (n > symboltable.ftotal) 
		{
            error(functionnotfound);
			cout << "Function not define." << endl;
            return -1;
        }
        //如果函数的实参个数与形参个数不匹配
        if (symboltable.element[symboltable.findextable[n]].paranum != paranum)
		{
            error(formalparanumunmatch);
			cout << "The number of parameters does not match." << endl;
            return -1;
        }
        for(int j = 0; j < paranum; j++)
		{
            if(symboltable.element[symboltable.findextable[n]].parakind[j] != tpkind[j])
			{
                error(paratypenotmatch);
				cout << "Para type not match." << endl;
            }
        }
        return symboltable.element[symboltable.findextable[n]].address;
    }
    else
	{
        n = symboltable.findextable[symboltable.ftotal];
        //查找符号表中是否有
        while (n < symboltable.index) 
		{
            if (strcmp(symboltable.element[n].name, name) == 0) break;
            n++;
        }
        //如果分程序符号表中无定义，查找最外层符号表
        if (n == symboltable.index) 
		{
            n = 0;
            while (n < m) 
			{
                if (strcmp(symboltable.element[n].name, name) == 0) break;
                n++;
            }
            //符号表中没有定义
            if (n == m) 
			{
                error(varnotdefine);
				cout << "Variable not define." << endl;
                return -1;
            }
        }
        if (symboltable.element[n].kind == intty || symboltable.element[n].kind == charty)
            return symboltable.element[n].address;   //常量
        if (symboltable.element[n].kind == cintet || symboltable.element[n].kind == ccharet)
            return symboltable.element[n].address;   //const
        if (symboltable.element[n].kind == paraint ||  symboltable.element[n].kind == parachar)
            return symboltable.element[n].address;    //参数
        return -1;
    }
}
//查符号表返回kind flag=1时，说明要在分程序索引里查找，flag=0时，在当前所在分程序符号表和最外层全局变量中查找
int searchkind(char* name, int flag)
{
    int n;
    int m = symboltable.findextable[1];
    if (flag == 1) 
	{
        n = 1;
        while (n <= symboltable.ftotal) 
		{
            if (strcmp(symboltable.element[symboltable.findextable[n]].name, name) == 0 ) break;
            n++;
        }
        //如果函数标志符没有定义
        if (n > symboltable.ftotal) 
		{
			error(functionnotfound);
			cout << "Function not define." << endl;
            return none;
        }
        //如果函数的实参个数与形参个数不匹配
        if (symboltable.element[symboltable.findextable[n]].paranum != paranum) 
		{
			error(formalparanumunmatch);
			cout << "The number of parameters does not match." << endl;
			return none;
        }
        return symboltable.element[symboltable.findextable[n]].value;
    }
    else
	{
        n = symboltable.findextable[symboltable.ftotal];
        //查找符号表中是否有
        while (n < symboltable.index) 
		{
            if (strcmp(symboltable.element[n].name, name) == 0) break;
            n++;
        }
        //如果分程序符号表中无定义，查找最外层符号表
        if (n == symboltable.index) 
		{
            n = 0;
            while (n < m) 
			{
                if (strcmp(symboltable.element[n].name, name) == 0) break;
                n++;
            }
            //符号表中没有定义
            if (n == m) 
			{
                error(varnotdefine);
				cout << "Variable not define." << endl;
                return none;
            }
        }
        if (symboltable.element[n].kind == intty || symboltable.element[n].kind == cintet || symboltable.element[n].kind == paraint)
            return intty;
        if (symboltable.element[n].kind == charty || symboltable.element[n].kind == ccharet || symboltable.element[n].kind == parachar)
            return charty;    
        return none;
    }
}
void popst()
{
    int x = symboltable.index - 1;
    while ( ( symboltable.element[x].kind == intty || symboltable.element[x].kind == charty || symboltable.element[x].kind == cintet
            || symboltable.element[x].kind == ccharet || symboltable.element[x].kind == paraint || symboltable.element[x].kind == parachar)
            && strcmp(symboltable.element[x].name, funcname) != 0 ) 
	{
        symboltable.element[x].kind = none;
        symboltable.element[x].address = 0;
        symboltable.element[x].paranum = 0;
        symboltable.element[x].value = 0;
        memset(symboltable.element[x].name,'\0',sizeof(symboltable.element[x].name));
        x--;
    }
    symboltable.index = x + 1;
    return;
}
//＜常量说明＞::= const＜常量定义＞;{const＜常量定义＞;}  const int a=1;const int b=1,c=2;
void conststate()
{
     while (symid == constty) 
	 {
         getsym();
         Symbol ctpe;
         if (symid == intty || symid == charty) 
		 {
             kind = (symid == intty) ? cintet : ccharet;
             address = 0;
             paranum = 0;
             ctpe = symid;
             getsym();
             constdef(ctpe);
             while (symid == commasy) 
			 {
                 getsym();
                 constdef(ctpe);
             }
             //定义结束，下一个字符一定是分号
			 if (Semicolonsy(symid, 0)) continue;
             getsym();
         }
         else
		 {
             error(keyworderror,0);
			 cout << "Keyword error." << endl;
             return;
         }
     }
     return;
 }
 //＜常量定义＞ ::= int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 void constdef(Symbol ctpe)
 {
     //without int or char   a=123/char='a'
      char temp[200];
      if (symid == iden) 
	  {
          strcpy(name, markstr.c_str());
          getsym();
          //if is '='
          if (symid == assignsy) 
		  {
              getsym();
              //'+' or '-'
              if (symid == plussy || symid == subsy) 
			  {
                  Symbol symb = symid;
                  getsym();
                  // if is integer
                  if (ctpe == intty && symid == numet) 
				  {
                      if (symb == plussy)  value = num;
                      else value = 0 - num;
                      pushtable(name, kind, value, address, paranum);
                      sprintf(temp, "%d", value);
                      inmidcode("const", "int", temp, name);
                  }
              }
              // if is char
              else if (symid == charet && ctpe==charty)
			  {
                value = stret.c_str()[1];
                pushtable(name, kind, value, address, paranum);
                sprintf(temp, "%d", stret.c_str()[1]);
                inmidcode("const", "char", temp, name);
              }
              //无符号整数
              else if (symid == numet && ctpe == intty)
			  {
                value = num;
                pushtable(name, kind, value, address, paranum);
                sprintf(temp, "%d", value);
                inmidcode("const", "int", temp, name);
              }
              else
			  {
                  error(unknowrightsym,0);
				  cout << "Illegal rightsym." << endl;
			      return;
              }
          }
          //没有赋值符号
          else
		  {
              error(constnotinit);
			  cout << "Const not init." << endl;
              return;
          }
		  num = 0;
      }
      else
      {
         error(cstdefinefail);
		 cout << "Const definition failed." << endl;
         return;
      }
      getsym();
      return;
  }
  //＜声明头部＞ ::= int＜标识符＞ | char＜标识符＞ 读int/char、标识符、后面的一个单词
 void defhead()
 {
     if (symid == intty || symid == charty) 
	 {
         kind = symid;
         getsym();
		 if (Iden(symid, 1)) return;
         else
		 {
            strcpy(name, markstr.c_str());//拷贝函数的名字;
         }
     }
     else
	 {
         error(keyworderror, 1);
		 cout << "Keyword error." << endl;
         return;
     }
     getsym();
     return;
}
//＜变量说明＞ ::= ＜变量定义＞;{＜变量定义＞;}
void varstate()
{
	vardef();
	if (Semicolonsy(symid, 1)) return;
    getsym();
    return;
}
//＜变量定义＞ ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
// int a/int a[22]/int a,b[11]
void vardef()
{
    if (symid == intty || symid == charty) 
	{
		kind = symid;
		value = 0;
		address = 0;
		paranum = 0;
		do {
			getsym();
			if (Iden(symid, 2)) return;
			strcpy(name, markstr.c_str());
			getsym();
			int isArr = (symid == lmbrasy);
			if (isArr)
			{    //如果是数组
				getsym();
				if (symid != numet)
				{
					error(immlack);
					cout << "Missing imm." << endl;
					return;
				}
				else
				{
					if (num == 0)
					{
						//数组定义下标为0
						error(immlack);
						cout << "Missing imm." << endl;
						return;
					}
					pushtable(name, kind, value, address, paranum, num);
					if (kind == intty)
					{
						char tempnum[20];
						sprintf(tempnum, "%d", num);
						inmidcode("inta", "  ", tempnum, name);
					}
					else if (kind == charty)
					{
						char tempnum[20];
						sprintf(tempnum, "%d", num);
						inmidcode("chara", "  ", tempnum, name);
					}
					getsym();
					if (Rmbrasy(symid, 1)) return;
					else getsym();
				}
			}
			else
			{
				//不是数组
				pushtable(name, kind, value, address, paranum, isArr);
				if (kind == intty) inmidcode("int", "  ", "  ", name);
				else if (kind == charty) inmidcode("char", "  ", "  ", name);
			}
			num = 0;
		} while (symid == commasy);
        
    }
    else
	{
        error(keyworderror, 2);
		cout << "Keyword error." << endl;
        return;
    }
    return;
}
//＜读语句＞ ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
void scanfsentence()
{
    char names[30];
    int s;
    getsym();
	if (Lsbrasy(symid, 1)) return;
    do {
        getsym();
		if (Iden(symid, 3)) return;
        strcpy(names, markstr.c_str());
        s = searchst(names, 0);
        if (s < 0)  return;
        inmidcode("scf", "  ", "  ", names);
        getsym();
    } while (symid == commasy);
	if (Rsbrasy(symid, 1)) return;
    getsym();
    return;
}
//＜写语句＞ ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
void printfsentence()
{
    char str1[200] = "", str2[30] = ""; //str1是字符串，str2是表达式
    getsym();
	if (Lsbrasy(symid, 1)) return;
    getsym();
    if (symid == stringet) 
	{
        strcpy(str1, stret.c_str());
        getsym();
        if (symid == commasy) 
		{
            getsym();
            expression();
            strcpy(str2, exprevalue);
        }
    }
    else 
	{
        expression();
        strcpy(str2, exprevalue);
    }
	if (Rsbrasy(symid, 1)) return;
    string tempstr = (termtk == charty) ? "char" : "int";
    inmidcode("prt", str1, str2, (char*)tempstr.data());
    getsym();
    return;
}
//＜返回语句＞ ::=  return[‘(’＜表达式＞‘)’]
void returnsentence()
{
    char str3[30];
    getsym();
    if (symid == lsbrasy) 
	{
        getsym();
        expression();//表达式子程序
        strcpy(str3, exprevalue);
		if (Rsbrasy(symid, 1)) return;
        returnnum++;
        inmidcode("ret", "  ", "  ", exprevalue);
        getsym();
        return;
    }
    inmidcode("ret", "  ", "  ", "  ");
    return;
}
//＜表达式＞::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}3*4+2/6-4*a由加法运算符连接的项
void expression()
{
    termtk = 0;
    char str1[30],str2[30], str3[30];
    if (symid == plussy || symid == subsy) 
	{
        termtk = intty;
        if (symid == plussy) 
		{
            getsym();
            item();//项，结束后，项的结果会放入全局变量expervalue
            strcpy(str3, exprevalue);//把项的结果放入临时变量的位置
        }
        else if (symid == subsy) 
		{
            getsym();
            item();
            strcpy(str1, exprevalue);
            strcpy(str3, nextreg());
            inmidcode("-", "0 ", str1, str3); //place3 = 0 - (place1)
        }
    }
    else 
	{
        item();
        strcpy(str3, exprevalue);
    }
    while (symid == plussy || symid == subsy)
	{
        termtk = intty;
        strcpy(str1, str3);
        if (symid == plussy) 
		{
            getsym();
            item();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());
            inmidcode("+", str1, str2, str3);
            continue;
        }
        else 
		{
            getsym();
            item();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());
            inmidcode("-", str1, str2, str3);
            continue;
        }
    }
    strcpy(exprevalue, str3);//把表达式的最终值存放在p之中
    return;
}
//＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}4*b*a由乘法运算符连接的因子
void item()
{
    char str1[200], str2[200], str3[200];
    factor();
    strcpy(str3, exprevalue);
    while (symid == multsy || symid == divsy) 
	{
        termtk = intty;
        strcpy(str1, str3);
        if (symid == multsy) 
		{
            getsym();
            factor();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());//申请临时变量
            inmidcode("*", str1, str2, str3);//str3=str1*str2
            continue;
        }
        else if (symid == divsy) 
		{
            getsym();
            factor();
            strcpy(str2, exprevalue);
            strcpy(str3, nextreg());
            inmidcode("/", str1, str2, str3);
            continue;
        }
    }
    strcpy(exprevalue, str3);  
    return;
}
//＜因子＞ ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
void factor()
{
    int t = -1;
    char names[30], str3[30];
    //＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜有返回值函数调用语句＞
    if (symid == iden) 
	{
        strcpy(names, markstr.c_str());
        getsym();
        //如果有左括号，则是＜有返回值函数调用语句＞
        if (symid == lsbrasy) 
		{
            getsym();
            valueofpara();
			if (Rsbrasy(symid, 2)) return;
            t = searchst(names, 1);
            if (t < 0)  return;
			if (termtk != intty) termtk == (symboltable.element[t].value == 1) ? intty : charty;
            strcpy(str3, nextreg());//生成临时变量
            inmidcode("call", names, "  ", str3);//将调用的返回值存放在临时变量里面
            strcpy(exprevalue, str3);
            getsym();
			cout << "This is a returnfunction diaoyong sentence!" << endl;
            return;
        }
        //＜标识符＞‘[’＜表达式＞‘]’
        else if (symid == lmbrasy)//数组
		{      
            //数组元素中括号表达式里termtk只能是intty
            getsym();
            t = searchst(names, 0);
            int tk = searchkind(names,0);
            expression();
            if(termtk != intty)
            {
				error(immlack);
				cout << "Missing imm." << endl;
                return;
            }
            if(termtk != intty) termtk = tk;
            char temp[30];
            strcpy(temp, exprevalue);
            //数组边界溢出判断
            if (isdigit(exprevalue[0]))
			{
                int arrnums = 0;
                arrnums = arrnum(names);
                if (atoi(temp) >= arrnums)
				{              
                    error(arrayoutbound);
					printf("Warning: Line:%d index of array out of boundary, which should be less than %d.\n", linenum, atoi(temp));
                }
            }
			if (Rmbrasy(symid, 0)) return;
            else
			{
                strcpy(exprevalue, nextreg());
                inmidcode("geta", names, temp, exprevalue);
                getsym();
            }
        }
        //＜标识符＞
        else
		{
            t = searchst(names, 0);      //查表，查找到标识符对应的地址
            if (t < 0) 
			{
                error(identifierlack, 4);
				cout << "Missing identifer." << endl;
                return;
            }
            if (termtk!=intty) termtk = searchkind(names,0);
            strcpy(exprevalue, names);
        }
        return;
    }
    //'('＜表达式＞')'
    else if (symid == lsbrasy) 
	{
        getsym();
        expression();
		if(Rsbrasy(symid, 2)) return;
        getsym();
        return;
    }
    // ＜整数＞｜＜字符＞ 注意正负号
    else if (symid == plussy || symid == subsy || symid == numet || symid == charet)
	{
        if(symid == plussy || symid == subsy)
		{
			int t = (symid == plussy) ? 1 : -1;
            getsym();
            termtk = intty;
            sprintf(exprevalue, "%d", t * num);
            num = 0;
            getsym();
            return;
        }
        else if(symid == numet)
		{
            termtk = intty;
            sprintf(exprevalue, "%d",  num);
            num=0;
            getsym();
            return;
        }
        else if(symid == charet)
		{
            if (termtk != intty) 
			{
                termtk = charty;
            }
            sprintf(exprevalue, "%d", stret.c_str()[1]);
            getsym();
            return;
        }
    }
    error(expressionerror);     
	cout << "Expression error." << endl;
    return;
}
//＜值参数表＞::= ＜表达式＞{,＜表达式＞}｜＜空＞
void valueofpara()
{
    int j = 0;
    vector<string> paraqueue;
    do {
        if (symid == commasy)
            getsym();
        if (symid == plussy || symid == subsy || symid == iden || symid == lsbrasy || symid == charet || symid == numet) 
		{
            expression();
            paraqueue.push_back(exprevalue);
            tpkind[j]=termtk;
            j++;
        }
    } while (symid == commasy);
    char tc[20];
    for (unsigned int i = 0; i < paraqueue.size(); i++ ) 
	{
        strcpy(tc, paraqueue[ i ].c_str());
        inmidcode("fupa", "  ", "  ", tc);
    }
    paranum = j;
    paraqueue.~vector();
    return;
}
//＜语句列＞ ::= ｛＜语句＞｝
//＜语句＞ ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;
// | ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;
void sentencelist()
 {
     while ( symid == ifty || symid == forty || symid == lbbrasy
         || symid == iden || symid == returnty || symid == scanfty || symid == printfty|| symid == switchty || symid == semicolonsy ) {
         sentence();
     }
 }
 //＜条件语句＞ ::= if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
 void ifsentence()
 {
    char label1[20], label2[20], conditionvalue[30];
    strcpy(label1, nextlab());  //如果不满足if，则跳过label1，label1是if的结束部分
    strcpy(label2, nextlab());  //如果还有else，则else的结束位置是label2
     getsym();
	 if (Lsbrasy(symid, 1)) return;
     getsym();
     //＜条件＞
     condition();
     strcpy(conditionvalue, exprevalue);  //条件计算的值在nowitem里面,此处应该是个临时变量$_x
     inmidcode("jne", "  ", "  ", label1);  //比较，为假的时候跳转到label1
	 if (Rsbrasy(symid, 1)) return;
     getsym();
     sentence();
     inmidcode("jmp", "  ", "  ", label2);//不执行else的部分,jump到lable2
     inmidcode("lab:", "  ", "  ", label1);
     if (symid == elsety) 
	 {
         getsym();
         sentence();
     }
     inmidcode("lab:", "  ", "  ", label2);
     return;
 }
//＜条件＞ ::= ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
void condition()
 {
     char str1[30], str2[30];
     expression();
     strcpy(str1, exprevalue);  //条件至少有一个表达式
     //关系运算符
     if (symid == lessy || symid == leseqsy || symid == grtsy || symid == grteqsy || symid == equalsy || symid == nequalsy) 
	 {
         if (symid == lessy) 
		 {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             inmidcode("<", str1, str2, "  ");
         }
         else if (symid == leseqsy) 
		 {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             inmidcode("<=", str1, str2, "  ");
         }
         else if (symid == grtsy) 
		 {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             inmidcode(">", str1, str2, "  ");
         }
         else if (symid == grteqsy) 
		 {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             inmidcode(">=", str1, str2, "  ");
         }
         else if (symid == equalsy) 
		 {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             inmidcode("==", str1, str2, "  ");
         }
         else if (symid == nequalsy) 
		 {
             getsym();
             expression();
             strcpy(str2, exprevalue);
             inmidcode("!=", str1, str2, "  ");
         }
        return;
     }
     strcpy(str2, "0");
     inmidcode("!=", str1, str2,"  ");
     return;
 }
 //＜循环语句＞::=  for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
  void loopsentence()
{
    int s = 0;
    char names[30], names1[30], names2[30], place2[30], place3[30];
    char label1[20], label2[20], label3[20] , op_flag[5];
    strcpy(label1, nextlab());
    strcpy(label2, nextlab());
    strcpy(label3, nextlab());
    if ( symid == forty )
	{    //for|(...)
        getsym();
		if (Lsbrasy(symid, 1)) return;
        getsym();
		if (Iden(symid, 0)) return; 
        strcpy(names, markstr.c_str());
        getsym();
        if (symid != assignsy) 
		{    //for(i=..;...;...)
            error(unacceptablesentence);//IWFLIRSPE
			cout << "Unacceptable sentence." << endl;
            return;
        }
        s = searchst(names, 0);
        if (s < 0)  return;
        getsym();
        expression();    //for(i=a+b|;...;...)
        inmidcode("=", exprevalue, "  ", names);
		if (Semicolonsy(symid, 2)) return;
        getsym();//for(i=a+b;|...;...)
        inmidcode("jmp", "  ", "  ", label3);
        inmidcode("lab:", "  ", "  ", label1);
        condition();//for(i=a+b;...;|...)
        inmidcode("jne", "  ", "  ", label2);
        inmidcode("lab:", "  ", "  ", label3);
		if (Semicolonsy(symid, 2)) return; 
        getsym();        //for(i=a+b;...;|...)
		if (Iden(symid, 3)) return;
        strcpy(names1, markstr.c_str());
        s = searchst(names1, 0);
        if (s < 0) return;
         //＜标识符＞＝＜标识符＞(+|-)＜步长＞        ＜步长＞ ::= ＜非零数字＞｛＜数字＞｝
        getsym();
        if (symid != assignsy) 
		{
            error(unacceptablesentence);
			cout << "Unacceptable sentence." << endl;
            return;
        }
        getsym();
		if (Iden(symid, 3)) return;
        strcpy(names2, markstr.c_str());
        s = searchst(names2, 0);
        if (s < 0)  return;
        getsym();
        if (symid != plussy && symid != subsy) 
		{
            error(plusminulack);
			cout << "Missing Plus or minus." << endl;
            return;
        }
        if (symid == plussy) strcpy(op_flag, "+");
        else if (symid == subsy) strcpy(op_flag, "-");
        getsym();
        if (symid != numet || num==0) 
		{
            //等号右边不为0
            error(unknowrightsym, 1);     //等号右边不是合法的整数
			cout << "Illegal rightsym." << endl;
            return;
        }
        sprintf(place2,"%d",num);
        getsym();
		if (Rsbrasy(symid, 1)) return; 
        getsym();
        sentence();
        strcpy(place3, nextreg());
        inmidcode(op_flag, names2, place2, place3);
        inmidcode("=", place3, "  ", names1);
        inmidcode("jmp", "  ", "  ", label1);
        inmidcode("lab:", "  ", "  ", label2);
        return;
    }
}
//＜情况语句＞::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
//＜情况表＞::=  ＜情况子语句＞{＜情况子语句＞}
//＜情况子语句＞ ::=  case＜常量＞：＜语句＞
//＜缺省＞ ::=  default : ＜语句＞
void casesentence()
{
    char label1[20],label2[20];
    char place1[50];
    int flg=0;
    strcpy(label2, nextlab());
    getsym();
	if (Lsbrasy(symid, 1)) return;
    getsym();
    expression();
    strcpy(place1,exprevalue);
	if (Rsbrasy(symid, 1)) return; 
    getsym();
	if (Lbbrasy(symid, 1)) return;
    getsym();
    do{
        strcpy(label1, nextlab());
        char place2[50];
        if(symid == casety)
		{
            getsym();
            char ttt[50];
            if(symid == subsy || symid== plussy|| symid == numet || symid == charet)
			{
                int t=1;
                if(symid == subsy || symid == plussy)
				{
                    t = (symid == subsy) ? -1 : 1 ;
                    getsym();
                }
                if(symid == numet) sprintf(ttt,"%d",t*num);
                else
				{
                    place2[0]=stret[1];
                    place2[1]='\0';
                    sprintf(ttt,"%d",place2[0]);
                }
                inmidcode("==", place1, ttt, "  ");
                //跳转如果不满足
                inmidcode("jne", "  ", "  ", label1);
                getsym();
                //如果没有冒号
                if (symid != colonsy) 
				{
                    error(colonlack);
					cout << "Missing colon." << endl;
                    return;
                }
                getsym();
                sentence();
                inmidcode("jmp", "  ", "  ", label2);//满足条件，不再执行switch语句中其余部分
            }
            else
			{
                error(immlack,1);
				cout << "Missing imm." << endl;
            }
            inmidcode("lab:", "  ", "  ", label1);
        }
        else{
            error(keyworderror,2);
			cout << "Keyword error." << endl;
            return;
        }

    }while(symid==casety);
    //如果没有default
    if (symid != defaultty)
	{
        error(keyworderror,2);
		cout << "Keyword error." << endl;
        return;
    }
    getsym();
    //如果没有冒号
    if (symid != colonsy) 
	{
        error(colonlack);
		cout << "Missing colon." << endl;
        return;
    }
    getsym();
    sentence();
	if(Rbbrasy(symid, 1)) return;
    getsym();
    inmidcode("lab:", "  ", "  ", label2);
    return;
}
//＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;
//| ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;
void sentence()
{
    int s;
    //条件语句
    if (symid == ifty) 
	{
        ifsentence();
		cout << "This is a if sentence!" << endl;
        return;
    }
    //循环语句
    if (symid == forty) 
	{
        loopsentence();
		cout << "This is a loop sentence!" << endl;
        return;
    }
     //情况语句
    if (symid == switchty)
	{
        casesentence();
		cout << "This is a switch sentence!" << endl;
        return;
    }
    //'{'语句列'}'
    if (symid == lbbrasy) 
	{
        getsym();
        sentencelist();
        //缺“}”
		if (Rbbrasy(symid, 1)) return;
        //不缺“}”
        getsym();
		cout << "This is a sentence list!" << endl;
        return;
    }
    //函数调用语句｜＜赋值语句＞;
    if (symid == iden) 
	{
        int isVec = 0;
        char names[MAXIDENLEN];
        strcpy(names, markstr.c_str());
        char place2[100];
        getsym();
        //＜赋值语句＞ ::=  ＜标识符＞＝＜表达式＞ | ＜标识符＞'['<表达式>']'＝＜表达式＞
        if (symid == lmbrasy)
		{ //数组
            getsym();
            expression();
            strcpy(place2, exprevalue);
            //数组越界检查 char/int
            if (isdigit(exprevalue[0]))
			{
                int arrnums = 0;
                arrnums = arrnum(names);
                if (atoi(exprevalue) >= arrnums)
				{    
                    error(arrayoutbound);
					printf("Warning: Line:%d index of array out of boundary, which should be less than %d.\n", linenum, atoi(exprevalue));
                }
            }
            isVec = 1;
			if (Rmbrasy(symid, 1)) return;
            getsym();
        }
        if (symid == assignsy) 
		{
            s = searchst(names, 0);
            int pretk = searchkind(names,0);
            if(symboltable.element[s].kind == cintet || symboltable.element[s].kind == ccharet)
			{
                error(assignconst);            //"="左边是不合法的标识符
				cout << "Assign const." << endl;
                return;
            }
            getsym();
            expression();
            if(termtk == intty && pretk == charty)
			{
                error(typechange);
				cout << "Int cannot assigh to char." << endl;
            }
            if (isVec) inmidcode("[]=", names, place2, exprevalue);
            else inmidcode("=", exprevalue, "  ", names);
			if (Semicolonsy(symid, 2)) return;
            getsym();
			cout << "This is a assign sentence!" << endl;
            return;
        }
        //函数调用语句
        if (symid == lsbrasy) 
		{
            getsym();
            valueofpara();  //计算并记录参数的值以及个数
			if (Rsbrasy(symid, 1)) return;
            //查函数表
            s = searchst(names, 1);
            inmidcode("call", names, "  ", "  ");
            getsym();
			if (Semicolonsy(symid, 2)) return;
            getsym();
        }
        else 
		{
            error(unacceptablesentence);    //不合法的句子
			cout << "Unacceptable sentence." << endl;   //不合法的句子
            return;
        }
		cout << "This is a function diaoyong sentence!" << endl;
        return;
    }
    //读语句
    if (symid == scanfty) 
	{
        scanfsentence();
		if (Semicolonsy(symid, 2))  return; 
        getsym();
		cout << "This is a scanf sentence!" << endl;
        return;
    }
    //写语句
    if (symid == printfty) 
	{
        printfsentence();
		if (Semicolonsy(symid, 2))  return; 
        getsym();
		cout << "This is a printf sentence!" << endl;
        return;
    }
    //返回语句
    if (symid == returnty) 
	{
        returnsentence();
        //返回语句后缺“；”
		if (Semicolonsy(symid, 2))  return;
        getsym();
		cout << "This is a return sentence!" << endl;
        return;
    }
    //空语句
    if (symid == semicolonsy)
    {
        getsym();
		cout << "This is a null sentence!" << endl;
        return;
    }
    //不合法的句子
    error(unacceptablesentence);
	cout << "Unacceptable sentence." << endl;
    return;
}
//＜复合语句＞ ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
void complexsentence()
{
    if (symid == constty) 
	{
        conststate();
		cout << "This is a const statement!" << endl;
    }
    //判断是否为变量说明
    if(symid == intty || symid == charty)
	{
        while(symid == intty || symid == charty)
            varstate();
		cout << "This is a variable statement!" << endl;
    }
    //语句列
    sentencelist();
    return;
}
//＜参数＞ ::=  ＜参数表＞
//＜参数表＞  ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
void parametertable()
{
	int i = 0;     //记录参数个数
	int temp;
	int tparakind[MAXPARANUM];
	if (symid == intty || symid == charty)
	{
		do {
			if (symid == commasy) getsym();
			temp = symid;
			defhead();
			kind = (temp == intty) ? paraint : parachar;     //4表示为函数参数
			value = 0;
			address = i;  //地址为i，即参数的位置
			paranum = 0;
			pushtable(name, kind, value, address, paranum);  //将行数参数插入符号表
			if (temp == intty)
			{
				inmidcode("para", "int", "  ", name);
				tparakind[i] = intty;
			}
			else if (temp == charty)
			{
				inmidcode("para", "char", "  ", name);
				tparakind[i] = charty;
			}
			i++;//参数个数加一
		} while (symid == commasy);//如果是逗号，则还有其他的参数
		paranum = i;//当前的参数个数
		updatepara(paranum, tparakind);//插入函数的参数个数
	}
	else if (symid == rsbrasy) return;
	else
	{
		error(rightparentlack);//ICV
		cout << "Missing right parent." << endl;
	}
	return;
}
//＜程序＞ ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
void program()
{
    if (symid == constty) 
	{
        conststate();
		cout << "This is a const statement!" << endl;
    }
	int globvari = 0;
    //变量定义和有返回值的函数是相同的声明头部。因此预读3个单词用于判断
    while (symid == intty || symid == charty) 
	{
        Symbol tempid = symid;//恢复现场用
        char tempch = ch;//恢复现场用
        int temppin = pin;//恢复现场用
        Symbol kind = symid;
        getsym();
        strcpy(name, markstr.c_str());//拷贝名字
        //如果标识符合法
		if(Iden(symid, 0)) continue;
        getsym();
        //如果是逗号,中括号,分号，则是变量定义
        if (symid == commasy || symid == lmbrasy || symid == semicolonsy) 
		{
            symid = tempid;
            pin = temppin;
            ch = tempch;
            //回到intty和charty处调用 变量声明函数
            varstate();
            globvari=1;
            continue;
        }
        else if(symid == lsbrasy)
		{
            symid = tempid;
            pin = temppin;
            ch = tempch;
            break;
        }
        else
		{
            error(defineerror);
			cout << "Variable define error." << endl;
            continue;
        }
    }
	if (globvari == 1)
		cout << "This is a variable statement!" << endl;
    //函数定义部分头出错
    if(symid != intty && symid != charty && symid != voidty)
	{
        error(funcdefineerror);
		cout << "Function define error." << endl;
    }
	int ismain = 0;
    globdefine = 0;
    while (symid == intty || symid == charty || symid == voidty) 
	{
        Symbol temp = symid;
        //＜有返回值函数定义＞  ::=  ＜声明头部＞‘(’＜参数＞‘)’  ‘{’＜复合语句＞‘}’
        if (symid == intty || symid == charty) 
		{
            defhead();
			if (Lsbrasy(symid, 0)) continue; 
            varcnt = 0;         //临时变量重新开始计数
            returnnum = 0;      //将是否存在返回值的标记初始化，0表示无返回值，即未出现return
            kind = function;    //表示当前的函数是子程序，即非主函数
            value = (symid == intty) ? 1 : 2;          //返回值
            address = 0;
            paranum = 0;
            pushtable(name, kind, value, address, paranum);
            strcpy(funcname, name);//函数名拷贝
            if (temp == intty) inmidcode("func", "int", "  ", funcname);
            if (temp == charty) inmidcode("func", "char", "  ", funcname);
            getsym();
            parametertable();
            //缺'('＜参数＞')'的右小括号
			if (Rsbrasy(symid, 0)) continue;
            getsym();
            //缺'{'＜复合语句＞'}'的左大括号
			if (Lbbrasy(symid, 0)) continue;
            getsym();
            complexsentence();
            //缺'{'＜复合语句＞'}'的右大括号
			if (Rbbrasy(symid, 0)) continue;
            //函数缺少返回值
            if (returnnum == 0) 
			{
                error(nonereturn);//ICV
				cout << "No return." << endl;
                continue;
            }
			cout << "This is a return function statement!" << endl;
            getsym();
            inmidcode("end", "  ", "  ", funcname);//函数部分四元式结束
            popst();//清空此子函数的符号表,不清除子函数名
        }
        //＜无返回值函数定义＞ ::= void＜标识符＞‘(’＜参数＞‘)’‘{’＜复合语句＞‘}’
        if (symid == voidty) 
		{
            getsym();
            if (symid != iden && symid !=mainty) 
			{
                error(identifierlack);//ICV
				cout << "Missing identifer." << endl;
                continue;
            }
            varcnt = 0;//临时变量重新开始计数
            if (symid == mainty)
			{
                ismain=1;
                break;
            }
            strcpy(name, markstr.c_str());
            kind = function;
            value = 0;
            address = 0;
            paranum = 0;
            pushtable(name, kind, value, address, paranum);
            strcpy(funcname, name);
            inmidcode("func", "void", "  ", funcname);
            getsym();
			if (Lsbrasy(symid, 0)) continue;
            getsym();
            parametertable();
			if (Rsbrasy(symid, 0)) continue;
            getsym();
			if (Lbbrasy(symid, 0)) continue; 
            getsym();
            complexsentence();
			if (Rbbrasy(symid, 0)) continue;
			cout << "This is a nonreturn function statement!" << endl;
            getsym();
            inmidcode("end", "  ", "  ", funcname);
            popst();
        }
    }
    if(symid == voidty) getsym();
    //缺少main关键字
    if(symid != mainty)
	{
        error(mainlack);
		cout << "Missing main." << endl;
    }
    varcnt = 0;        //临时变量重新开始计数
    strcpy(name, "main");
    kind = function;
    value = 0;
    address = 0;
    paranum = 0;
    pushtable(name, kind, value, address, paranum);
    strcpy(funcname, name);
    inmidcode("func", "  ", "  ", funcname);
    getsym();
	Lsbrasy(symid, 0);
    getsym();
	Rsbrasy(symid, 0); 
    getsym();
	Lbbrasy(symid, 0);
    getsym();
    complexsentence();
    inmidcode("end", "  ", "  ", funcname);
	Rbbrasy(symid, 0);
	cout << "This is a main function!" << endl;
	cout << "This is the program!" << endl;
    return;
}