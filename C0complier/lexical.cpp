#include <cstdlib>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include "main.h"

#define IsLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')
#define IsDigit(a) (a >= '0' && a <= '9')

using namespace std;

char ch = ' ';           ///存放最新读进的源程序字符
string strToken;       ///存放构成单词的字符串
int k = 0;
string markstr;  //记录全局变量标识符
int num = 0;          //记录全局变量数字
string stret = "";          //记录全局变量 字符，字符串
int linenum = 1;
Symbol symid = none;
int pin = 0;
map<string, Symbol> Key;
map<string, Symbol> Ssymbol;

string  key[14] = {"main","int","char","const","void",
	"if","else","switch","case","default",
	"for","printf","scanf","return" };

string  symbol[20] = {
	"=","+","-","*","/",
	"<","<=",">",">=","!=",
	"==","(",")","[","]",
	"{","}",",",":",";" };

Symbol keyType[14] = {mainty,intty,charty,constty,voidty,
	ifty,elsety,switchty,casety,defaultty,
	forty,printfty,scanfty,returnty};

Symbol symbolType[20] = {assignsy,plussy,subsy,multsy,divsy,
	lessy,leseqsy,grtsy,grteqsy,nequalsy,
	equalsy,lsbrasy,rsbrasy,lmbrasy,rmbrasy,
	lbbrasy,rbbrasy,commasy,colonsy,semicolonsy};

void init()
{
	for (int i = 0; i < 14; i++)
	{
		Key[key[i]] = keyType[i];
	}
	for (int i = 0; i < 20; i++)
	{
		Ssymbol[symbol[i]] = symbolType[i];
	}
}
void GetChar()
{
	ch = str[pin];
	pin++;
}
void GetSingleChar()
{
	while (1)
	{
		if (ch == ' ' || ch == '\t' || ch == '\n')
		{
			if (ch == '\n')
			{
				linenum++;
			}
			GetChar();
		}
		else break;
	}
}
void Concat()
{
	strToken += ch;
}
void Clear()
{
	ch = ' ';
	pin--;
}
void getsym()
{
	init();
	int cnt;
	strToken = "";
	GetChar();
	GetSingleChar();
     //标识符和关键字 字母下划线开头，只包含字母和数字和_
	if (IsLetter(ch))
	{
		while (IsLetter(ch) || IsDigit(ch))
		{
			Concat();
			GetChar();
		}
		Clear();
		cnt = Key.count(strToken);
		if (cnt)  //关键字
		{
			symid = Key[strToken];
			cout << ++k << " " << symid << " " << strToken << endl;
		}
		else //标识符
		{
			cout << ++k << " " << "IDENTIFIER" << " " << strToken << endl;
			symid = iden;
			markstr = strToken;
		}
	}
	//无符号整数、0型常量
	else if (IsDigit(ch) && ch != '0')
	{
		num = 0;
		while (IsDigit(ch))
		{
			num = num * 10 + (int)(ch - '0');
			Concat();
			GetChar();
		}
		Clear();
		cout << ++k << " " << "numet" << " " << strToken << endl;
		symid = numet;
	}
	else if (IsDigit(ch) && ch == '0')
	{
		GetChar();
		//单个0
		if (!IsDigit(ch))
		{
			cout << ++k << " " << "numet" << " " << 0 << endl;
			symid = numet;
			num = 0;
		}
		else
		{
			error(zeroerror);
			cout << "Number starts with zero." << endl;
		}
		Clear();
	}
	//字符''
	else if (ch == '\'')
	{
		Concat();
		GetChar();
		if (IsLetter(ch) || IsDigit(ch) || ch == '+' || ch == '-' || ch == '*' || ch == '/')
		{
			Concat();
			GetChar();
			if (ch == '\'') Concat();
			else
			{				
				error(singlequoteslack);
				cout << "Missing single quotes." << endl;
			}
			cout << ++k << " " << "charet" << " " << strToken << endl;//输出信息
			symid = charet;
			stret = strToken;
		}
		else
		{
			error(unacceptatlechar);
			cout << "Unacceptable char." << endl;
			exit(1);
		}
	}
	//字符串""
	else if (ch == '\"')
	{
		Concat();
		GetChar();
		while (ch != '"')
		{
			if (ch == 32 || ch == 33 || (ch >= 35 && ch <= 126))
			{
				Concat();
				GetChar();
			}
			else
			{
				error(doublequoteslack);
				cout << "Missing double quotes." << endl;
			}
		}
		Concat();
		cout << ++k << " " << "stringet" << " " << strToken << endl;
		stret = strToken;
		symid = stringet;
	}
	//赋值，相等=、== 小于，小于等于<,<= 大于，大于等于>,>= 不等于!=
	else if (ch == '='|| ch == '<'|| ch == '>'||ch == '!')
	{
		char tempch = ch;
		Concat();
		GetChar();
		if (ch == '=')
		{
			if (tempch == '=')//==
			{
				Concat();
				cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
				symid = equalsy;
			}
			else if (tempch == '<')//<=
			{
				Concat();
				cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
				symid = leseqsy;
			}
			else if(tempch == '>')//>=
			{
				Concat();
				cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
				symid = grteqsy;
			}
			else//!=
			{
				Concat();
				cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
				symid = nequalsy;
			}
		}
		else
		{
			if (tempch == '=')//=
			{
				cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
				symid = assignsy;
				Clear();
			}
			else if (tempch == '<')//<
			{
				cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
				symid = lessy;
				Clear();
			}
			else if (tempch == '>')//>
			{
				cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
				symid = grtsy;
				Clear();
			}
			else
			{
				error(exclamationmark);
				cout << "! without = backward." << endl;
			}
		}
	}
	else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == '{' || ch == '}'
		|| ch == '[' || ch == ']' || ch == ';' || ch == ':' || ch == ',')
	{
		Concat();
		cout << ++k << " " << Ssymbol[strToken] << " " << strToken << endl;
		symid = Ssymbol[strToken];
	}
	else if (ch == '\0')
	{
		symid = none;
		return;
	}
	else
	{
		error(unexpectedchar);
		cout << "Unexpected char." << endl;
	}
}