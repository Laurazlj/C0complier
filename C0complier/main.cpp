#include "main.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

using namespace std;

ofstream mipsWrite, midWrite, errWrite, midbetterWrite;
fstream infile;
string str = "";

int main(int argc, const char * argv[]) 
{
	string file;
	cout << "请输入文件名：";
	cin >> file;
	infile.open(file.data());
	if (!infile.is_open()) return 0;
	char temp;
	while (1)
	{
		if (infile.eof()) break;
		infile.get(temp);
		str += temp;
	}
	str = str.substr(0, str.length() - 1);
	infile.close();
	mipsWrite.open("mipsWrite.asm");
	midWrite.open("midWrite.txt");
	errWrite.open("errWrite.txt");
	midbetterWrite.open("midWriteBetter.txt");
    getsym();
    program();
    for(int i = 0; i < midcnt; i++)
	{
		midWrite << midcode[i].op << '\t' <<midcode[i].var1 << '\t' << midcode[i].var2 << '\t' << midcode[i].var3 << endl;
    }
    Tomips();
    OPT=1;
    mipsWrite.close();
    mipsWrite.open("mipsWriteBetter.asm");
    Tomips();
    better();
    for(int i = 0; i < midcnt; i++)
	{
		midbetterWrite << midcode[i].op << '\t' <<midcode[i].var1 << '\t' << midcode[i].var2 << '\t' << midcode[i].var3 << endl;
    }
	midWrite.close();
	errWrite.close();
	midbetterWrite.close();
	mipsWrite.close();
    return 0;
}