#include <cstdlib>
#include <string.h>
#include <iostream>
#include <fstream>
#include "main.h"
#include <vector>

using namespace std;

int midcnt=0;    //��Ԫʽ������
int varcnt = 0; //��ʱ����������
vector <char*> tempvar; //��ʱ�������ַ�������
int labelcnt = 0;

midcodee midcode[MAXMIDCODE];

void inmidcode(const char* op, const char* t1, const char* t2,const char* t3)
{
    strcpy(midcode[midcnt].op, op);
    strcpy(midcode[midcnt].var1, t1);
    strcpy(midcode[midcnt].var2, t2);
    strcpy(midcode[midcnt].var3, t3);
    midcnt++;
}
//�����µ���ʱ����
char* nextreg()
{
    char *var = (char*)malloc(sizeof(char) * 20);
    sprintf(var, "$_%d", varcnt++);
    tempvar.push_back(var);
    return var;
}
void freetempvar()
{
    for (unsigned int i = 0; i < tempvar.size(); i++ ) 
	{
        free(tempvar[i]);
    }
}
//�����µı��
char* nextlab()
{
    char *label = (char*)malloc(sizeof(char) * 20);
    strcpy(label, "_LABEL_");
    sprintf(label, "_LABEL_%d", labelcnt++);
    return label;
}