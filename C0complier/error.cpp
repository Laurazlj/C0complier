#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

#define rsT errWrite

int errnum = 0;
void error(err errsig, int opt)
{
	errcls errclass = noerror;
	errnum++;
	rsT << "Error" << errnum << " : At Line" << linenum << ":\t";
	switch (errsig)
	{
		case  doublequoteslack:
			errclass = donothing;
			rsT << "Missing double quotes." << endl;
			break;        //1 少双引号 忽略
		case  unacceptatlechar:
			errclass = donothing;
			rsT << "Unacceptable char." << endl;
			break;        //2 不可接受字符 忽略
		case  singlequoteslack:
			errclass = donothing;
			rsT << "Missing single quotes." << endl;
			break;        //3 少单引号 忽略
		case  outoftable:
			errclass = quit;
			rsT << "Idtable full." << endl;
			break;        //4 符号表满 退出程序
		case  symbolconflict:
			errclass = donothing;
			rsT << "Variable name conflict." << endl;
			break;        //5 变量名冲突 忽略
		case  cstdefinefail:
			errclass = cs;
			rsT << "Const definition failed." << endl;
			break;        //6 const定义失败 跳到下一个逗号或者分号
		case  unknowrightsym:
			errclass = (opt == 0 ? cs : opt == 1 ? sfirst : noerror); //7 等号右边字符非法 常量定义里跳过，数字不合法跳到下一个位置
			rsT << "Illegal rightsym." << endl;
			break;
		case  semicolonlack:
			errclass = (opt == 0 ? icvfirst : opt == 1 ? sentenfirst : opt == 2 ? sfirst : noerror);
			rsT << "Semicolon lack." << endl;//8 丢失; 
			break;
		case  keyworderror:
			errclass = (opt == 0 ? icvfirst : opt == 1 ? clr : opt == 2 ? sentenfirst : opt == 3 ? sfirst : noerror);
			rsT << "Keyword error." << endl;//9 关键字错误
			break;
		case  identifierlack:
			errclass = (opt == 0 ? icv : opt == 1 ? clr : opt == 2 ? sentenfirst : opt == 3 ? sfirst : opt == 4 ? bfirst : noerror);
			rsT << "Missing identifer." << endl;//10 缺少标识符
			break;
		case  rightbracklack:
			errclass = (opt == 0 ? bfirst : opt == 1 ? sentenfirst : noerror);
			rsT << "Missing right brack." << endl;//11 丢失]
			break;
		case  functionnotfound:
			errclass = (opt == 0 ? sfirst : opt == 1 ? bfirst : noerror); //12 函数未定义	
			rsT << "Function not define." << endl;
			break;
		case  formalparanumunmatch:
			errclass = (opt == 0 ? donothing : opt == 1 ? sfirst : noerror);  //13 形参个数不匹配			
			rsT << "The number of parameters does not match." << endl;
			break;
		case  varnotdefine:
			errclass = (opt == 0 ? donothing : opt == 1 ? sfirst : noerror); //14 变量未定义			
			rsT << "Variable not define." << endl;
			break;
		case  leftparentlack:
			errclass = (opt == 0 ? icv : opt == 1 ? sfirst : opt == 2 ? bfirst : noerror);//15 少左小括号			
			rsT << "Missing left parent." << endl;
			break;
		case  rightparentlack:
			errclass = (opt == 0 ? icv : opt == 1 ? sfirst : opt == 2 ? bfirst : noerror);//16 少右小括号
			rsT << "Missing right parent." << endl;
			break;
		case  immlack:
			errclass = (opt == 0 ? sentenfirst : switchchjump);
			rsT << "Missing imm." << endl;
			break;                    //17 少立即数
		case  rightbracelack:
			errclass = (opt == 0 ? icv : opt == 1 ? sfirst : noerror); //18 少右大括号			
			rsT << "Missing right brace." << endl;
			break;
		case  expressionerror:
			errclass = bfirst;
			rsT << "Expression error." << endl;
			break;             // 19 表达式错误
		case  leftbracelack:
			errclass = icv;
			rsT << "Missing left brace." << endl;
			break;                       //20 少左大括号
		case  nonereturn:
			errclass = icv;
			rsT << "No return." << endl;
			break;                          //21 无返回值
		case  unacceptablesentence:
			errclass = sfirst;
			rsT << "Unacceptable sentence." << endl;
			break;       //22 不可接受句子
		case  assignconst:
			errclass = (opt == 0 ? sfirst : opt == 1 ? sfirst : noerror); //23 赋值常量
			rsT << "Assign const." << endl;
			break;
		case  plusminulack:
			errclass = sfirst;
			rsT << "Missing Plus or minus." << endl;
			break;            //24 少加号或减号
		case  mainlack:
			errclass = donothing;
			rsT << "Missing main." << endl;
			break;                     //25 无main函数
		case  moreword:
			errclass = donothing;
			rsT << "Extra characters." << endl;
			break;                     //26 main函数后面有多余字符
		case  defineerror:
			errclass = icv;
			rsT << "Variable define error." << endl;//27 变量定义错误
			break;
		case  funcdefineerror:
			errclass = icv;
			rsT << "Function define error." << endl;//28 函数定义错误
			break;
		case  constnotinit:
			errclass = cs;
			rsT << "Const not init." << endl;
			break;                           //29 常量未初始化
		case colonlack:
			errclass = switchchjump;
			rsT << "Missing colon." << endl;//30  缺冒号
			break;
		case  typechange:
			errclass = donothing;
			rsT << "Int cannot assigh to char." << endl;//31 类型不对
			break;
		case  unexpectedchar:
			errclass = donothing;
			rsT << "Unexpected char." << endl;//32 不是需要的char类型
			break;
		case paratypenotmatch:
			errclass = donothing;
			rsT << "Para type not match." << endl;//33 参数类型不匹配
			break;
		case arrayoutbound:
			errclass = donothing;
			rsT << "Warning: index of array out of boundary." << endl; //34 数组越界
			break;
		case zeroerror:
			errclass = donothing;
			rsT << "Number starts with zero." << endl;//35 数字以0开头
			break;
		case exclamationmark:
			errclass = donothing;
			rsT << "! without = backward." << endl;// 36！后面没有等于号
			break;
		default:
			errclass = quit;
			rsT << "Unknow error." << endl; //37 未知错误
	}
	switch (errclass)
	{
		case quit:
			exit(0);
			break;
		case donothing:
			break;
		case icv:
			while (symid != intty && symid != charty && symid != voidty)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
			break;
		case icvfirst:
			while (symid != constty && symid != intty && symid != charty && symid != voidty && symid != ifty
				&& symid != forty && symid != returnty && symid != scanfty && symid != printfty)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
			break;
		case cs:
			while (symid != commasy && symid != semicolonsy)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
			break;
		case clr:
			while (symid != commasy && symid != lsbrasy && symid != rsbrasy)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
			break;
		case sentenfirst:
			while (symid != lbbrasy && symid != ifty && symid != switchty && symid != forty && symid != returnty && symid != scanfty
				&& symid != printfty && symid != intty && symid != charty && symid != voidty)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
			break;
		case sfirst:
			while (symid != ifty && symid != forty && symid != switchty && symid != lmbrasy && symid != iden
				&& symid != returnty && symid != scanfty && symid != printfty && symid != elsety && symid != rbbrasy)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
			break;
		case bfirst:
			while (symid != ifty && symid != forty && symid != switchty && symid != lmbrasy && symid != iden && symid != returnty
				&& symid != scanfty && symid != printfty && symid != semicolonsy && symid != elsety && symid != rsbrasy
				&& symid != commasy && symid != plussy && symid != subsy && symid != multsy && symid != divsy
				&& symid != lessy && symid != leseqsy && symid != grtsy && symid != grteqsy && symid != nequalsy && symid != equalsy)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
			break;
		case switchchjump:
			while (symid != defaultty && symid != rbbrasy)
			{
				if (symid == none)
				{
					exit(0);
				}
				getsym();
			}
		default:
			break;
	}
}
int Rsbrasy(int symid,int op)//)
{
	if (symid != rsbrasy)
	{
		error(rightparentlack, op);
		cout << "Missing right parent." << endl;
		return 1;
	}
	return 0;
}
int Semicolonsy(int symid, int op)//分号
{
	if (symid != semicolonsy)
	{
		error(semicolonlack, op);
		cout << "Semicolon lack." << endl;
		return 1;
	}
	return 0;
}
int Iden(int symid, int op)
{
	if (symid != iden)
	{
		error(identifierlack, op);
		cout << "Missing identifer." << endl;
		return 1;
	}
	return 0;
}
int Rmbrasy(int symid, int op)//]
{
	if (symid != rmbrasy)
	{
		error(rightbracklack, op);
		cout << "Missing right brack." << endl;
		return 1;
	}
	return 0;
}
int Lsbrasy(int symid, int op)//(
{
	if (symid != lsbrasy)
	{
		error(leftparentlack, op);
		cout << "Missing left parent." << endl;
		return 1;
	}
	return 0;
}
int Lbbrasy(int symid, int op)//{
{
	if (symid != lbbrasy)
	{
		error(leftbracelack, op);
		cout <<  "Missing left brace." << endl;
		return 1;
	}
	return 0;
}
int Rbbrasy(int symid, int op)//}
{
	if (symid != rbbrasy)
	{
		error(rightbracelack, op);
		cout << "Missing right brace." << endl;
		return 1;
	}
	return 0;
}