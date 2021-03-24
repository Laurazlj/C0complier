#include <cstdlib>
#include <string.h>
#include <iostream>
#include <fstream>
#include "main.h"
#include <vector>

using namespace std;

int  value;     //���泣����ֵ�������ӳ���0��ʾvoid��1��ʾint
int  address;   //�����ĵ�ַ����������ֵ�ĵ�ַ
int  paranum;   //ȫ�ֱ������������������壬��Ϊ�����Ĳ�������ֻ���ڲ������ӳ���ִ�н���֮���֪��������ֻ���ں���Ų�����ű���Ϣ
Symbol  kind;      //���ڴ���ı�־��������
int tpkind[MAXPARANUM];
int returnnum = 0;
int termtk=0;
char exprevalue[200];
char funcname[100];
int globdefine = 1;
char name[MAXIDENLEN]; //��־�������֣��������������ͺ���
 
typedef struct{
    char name[MAXIDENLEN];  //identifier name
    Symbol kind;               //��ʶ�����ͣ�intty,charty,cintet,ccharet,function,paraint,parachar
    int value;              //���ں�����˵��2��ʾ����ֵΪchar��1��ʾ����ֵΪint 0����ֵΪvoid
    int address;            //address��Ԫ���ڷ��ű�element�����е��±�
    int paranum;            //�Ժ�����˵�Ĳ�������
    int parakind[MAXPARANUM];//�洢������������
}tableelement;
typedef struct{
    tableelement element[MAXSYMTABLENUM];//���ű�����
    int index; //���ű��и���
    int ftotal; //�ֳ�������
    int findextable[MAXSYMTABLENUM];//�ֳ�����������
}symtable;
symtable symboltable;
//push���ű�
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
        //�Ǻ���   �ں����ڲ����Ƿ������ֲ�����
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
        //ȫ�ֱ�����Χ������ȫ�ֱ����Ƿ�����
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
    int x = symboltable.findextable[symboltable.ftotal];   //array[i]��ָ����ǵ�ǰ���ڱ������ĺ����ڷ��ű��е�����
    symboltable.element[x].paranum = paranum;//���뺯������������Ϣ
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
    //���ҷ��ű����Ƿ���
    while (n < symboltable.index) 
	{
        if (strcmp(symboltable.element[n].name, name) == 0 ) return symboltable.element[n].paranum;
        n++;
    }
    //����ֳ�����ű����޶��壬�����������ű�
    if (n == symboltable.index) 
	{
        n = 0;
        while (n < m) 
		{
            if (strcmp(symboltable.element[n].name, name) == 0) return symboltable.element[n].paranum;
            n++;
        }
        //���ű���û�ж���
        if (n == m) 
		{
            printf("\"%s\" ", name);
            return -1;
        }
    }
	return -1;
}
//����ű�flag=1ʱ��˵��Ҫ�ڷֳ�����������ң�flag=0ʱ���ڵ�ǰ���ڷֳ�����ű�������ȫ�ֱ����в���
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
        //���������־��û�ж���
        if (n > symboltable.ftotal) 
		{
            error(functionnotfound);
			cout << "Function not define." << endl;
            return -1;
        }
        //���������ʵ�θ������βθ�����ƥ��
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
        //���ҷ��ű����Ƿ���
        while (n < symboltable.index) 
		{
            if (strcmp(symboltable.element[n].name, name) == 0) break;
            n++;
        }
        //����ֳ�����ű����޶��壬�����������ű�
        if (n == symboltable.index) 
		{
            n = 0;
            while (n < m) 
			{
                if (strcmp(symboltable.element[n].name, name) == 0) break;
                n++;
            }
            //���ű���û�ж���
            if (n == m) 
			{
                error(varnotdefine);
				cout << "Variable not define." << endl;
                return -1;
            }
        }
        if (symboltable.element[n].kind == intty || symboltable.element[n].kind == charty)
            return symboltable.element[n].address;   //����
        if (symboltable.element[n].kind == cintet || symboltable.element[n].kind == ccharet)
            return symboltable.element[n].address;   //const
        if (symboltable.element[n].kind == paraint ||  symboltable.element[n].kind == parachar)
            return symboltable.element[n].address;    //����
        return -1;
    }
}
//����ű���kind flag=1ʱ��˵��Ҫ�ڷֳ�����������ң�flag=0ʱ���ڵ�ǰ���ڷֳ�����ű�������ȫ�ֱ����в���
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
        //���������־��û�ж���
        if (n > symboltable.ftotal) 
		{
			error(functionnotfound);
			cout << "Function not define." << endl;
            return none;
        }
        //���������ʵ�θ������βθ�����ƥ��
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
        //���ҷ��ű����Ƿ���
        while (n < symboltable.index) 
		{
            if (strcmp(symboltable.element[n].name, name) == 0) break;
            n++;
        }
        //����ֳ�����ű����޶��壬�����������ű�
        if (n == symboltable.index) 
		{
            n = 0;
            while (n < m) 
			{
                if (strcmp(symboltable.element[n].name, name) == 0) break;
                n++;
            }
            //���ű���û�ж���
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
//������˵����::= const���������壾;{const���������壾;}  const int a=1;const int b=1,c=2;
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
             //�����������һ���ַ�һ���Ƿֺ�
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
 //���������壾 ::= int����ʶ��������������{,����ʶ��������������} | char����ʶ���������ַ���{,����ʶ���������ַ���}
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
              //�޷�������
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
          //û�и�ֵ����
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
  //������ͷ���� ::= int����ʶ���� | char����ʶ���� ��int/char����ʶ���������һ������
 void defhead()
 {
     if (symid == intty || symid == charty) 
	 {
         kind = symid;
         getsym();
		 if (Iden(symid, 1)) return;
         else
		 {
            strcpy(name, markstr.c_str());//��������������;
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
//������˵���� ::= ���������壾;{���������壾;}
void varstate()
{
	vardef();
	if (Semicolonsy(symid, 1)) return;
    getsym();
    return;
}
//���������壾 ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]��) }
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
			{    //���������
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
						//���鶨���±�Ϊ0
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
				//��������
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
//������䣾 ::=  scanf ��(������ʶ����{,����ʶ����}��)��
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
//��д��䣾 ::= printf ��(�� ���ַ�����,�����ʽ�� ��)��| printf ��(�����ַ����� ��)��| printf ��(�������ʽ����)��
void printfsentence()
{
    char str1[200] = "", str2[30] = ""; //str1���ַ�����str2�Ǳ��ʽ
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
//��������䣾 ::=  return[��(�������ʽ����)��]
void returnsentence()
{
    char str3[30];
    getsym();
    if (symid == lsbrasy) 
	{
        getsym();
        expression();//���ʽ�ӳ���
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
//�����ʽ��::= �ۣ������ݣ��{���ӷ�����������}3*4+2/6-4*a�ɼӷ���������ӵ���
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
            item();//���������Ľ�������ȫ�ֱ���expervalue
            strcpy(str3, exprevalue);//����Ľ��������ʱ������λ��
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
    strcpy(exprevalue, str3);//�ѱ��ʽ������ֵ�����p֮��
    return;
}
//���::= �����ӣ�{���˷�������������ӣ�}4*b*a�ɳ˷���������ӵ�����
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
            strcpy(str3, nextreg());//������ʱ����
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
//�����ӣ� ::= ����ʶ����������ʶ������[�������ʽ����]������������|���ַ��������з���ֵ����������䣾|��(�������ʽ����)��
void factor()
{
    int t = -1;
    char names[30], str3[30];
    //����ʶ����������ʶ������[�������ʽ����]�������з���ֵ����������䣾
    if (symid == iden) 
	{
        strcpy(names, markstr.c_str());
        getsym();
        //����������ţ����ǣ��з���ֵ����������䣾
        if (symid == lsbrasy) 
		{
            getsym();
            valueofpara();
			if (Rsbrasy(symid, 2)) return;
            t = searchst(names, 1);
            if (t < 0)  return;
			if (termtk != intty) termtk == (symboltable.element[t].value == 1) ? intty : charty;
            strcpy(str3, nextreg());//������ʱ����
            inmidcode("call", names, "  ", str3);//�����õķ���ֵ�������ʱ��������
            strcpy(exprevalue, str3);
            getsym();
			cout << "This is a returnfunction diaoyong sentence!" << endl;
            return;
        }
        //����ʶ������[�������ʽ����]��
        else if (symid == lmbrasy)//����
		{      
            //����Ԫ�������ű��ʽ��termtkֻ����intty
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
            //����߽�����ж�
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
        //����ʶ����
        else
		{
            t = searchst(names, 0);      //������ҵ���ʶ����Ӧ�ĵ�ַ
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
    //'('�����ʽ��')'
    else if (symid == lsbrasy) 
	{
        getsym();
        expression();
		if(Rsbrasy(symid, 2)) return;
        getsym();
        return;
    }
    // �������������ַ��� ע��������
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
//��ֵ������::= �����ʽ��{,�����ʽ��}�����գ�
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
//������У� ::= ������䣾��
//����䣾 ::= ��������䣾����ѭ����䣾| ��{��������У���}�������з���ֵ����������䣾;
// | ���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|�������䣾����������䣾;
void sentencelist()
 {
     while ( symid == ifty || symid == forty || symid == lbbrasy
         || symid == iden || symid == returnty || symid == scanfty || symid == printfty|| symid == switchty || symid == semicolonsy ) {
         sentence();
     }
 }
 //��������䣾 ::= if ��(������������)������䣾��else����䣾��
 void ifsentence()
 {
    char label1[20], label2[20], conditionvalue[30];
    strcpy(label1, nextlab());  //���������if��������label1��label1��if�Ľ�������
    strcpy(label2, nextlab());  //�������else����else�Ľ���λ����label2
     getsym();
	 if (Lsbrasy(symid, 1)) return;
     getsym();
     //��������
     condition();
     strcpy(conditionvalue, exprevalue);  //���������ֵ��nowitem����,�˴�Ӧ���Ǹ���ʱ����$_x
     inmidcode("jne", "  ", "  ", label1);  //�Ƚϣ�Ϊ�ٵ�ʱ����ת��label1
	 if (Rsbrasy(symid, 1)) return;
     getsym();
     sentence();
     inmidcode("jmp", "  ", "  ", label2);//��ִ��else�Ĳ���,jump��lable2
     inmidcode("lab:", "  ", "  ", label1);
     if (symid == elsety) 
	 {
         getsym();
         sentence();
     }
     inmidcode("lab:", "  ", "  ", label2);
     return;
 }
//�������� ::= �����ʽ������ϵ������������ʽ���������ʽ��
void condition()
 {
     char str1[30], str2[30];
     expression();
     strcpy(str1, exprevalue);  //����������һ�����ʽ
     //��ϵ�����
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
 //��ѭ����䣾::=  for��(������ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)����������)������䣾
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
         //����ʶ����������ʶ����(+|-)��������        �������� ::= ���������֣��������֣���
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
            //�Ⱥ��ұ߲�Ϊ0
            error(unknowrightsym, 1);     //�Ⱥ��ұ߲��ǺϷ�������
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
//�������䣾::=  switch ��(�������ʽ����)�� ��{�����������ȱʡ����}��
//�������::=  ���������䣾{���������䣾}
//���������䣾 ::=  case��������������䣾
//��ȱʡ�� ::=  default : ����䣾
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
                //��ת���������
                inmidcode("jne", "  ", "  ", label1);
                getsym();
                //���û��ð��
                if (symid != colonsy) 
				{
                    error(colonlack);
					cout << "Missing colon." << endl;
                    return;
                }
                getsym();
                sentence();
                inmidcode("jmp", "  ", "  ", label2);//��������������ִ��switch��������ಿ��
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
    //���û��default
    if (symid != defaultty)
	{
        error(keyworderror,2);
		cout << "Keyword error." << endl;
        return;
    }
    getsym();
    //���û��ð��
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
//����䣾    ::= ��������䣾����ѭ����䣾| ��{��������У���}�������з���ֵ����������䣾;
//| ���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|�������䣾����������䣾;
void sentence()
{
    int s;
    //�������
    if (symid == ifty) 
	{
        ifsentence();
		cout << "This is a if sentence!" << endl;
        return;
    }
    //ѭ�����
    if (symid == forty) 
	{
        loopsentence();
		cout << "This is a loop sentence!" << endl;
        return;
    }
     //������
    if (symid == switchty)
	{
        casesentence();
		cout << "This is a switch sentence!" << endl;
        return;
    }
    //'{'�����'}'
    if (symid == lbbrasy) 
	{
        getsym();
        sentencelist();
        //ȱ��}��
		if (Rbbrasy(symid, 1)) return;
        //��ȱ��}��
        getsym();
		cout << "This is a sentence list!" << endl;
        return;
    }
    //����������������ֵ��䣾;
    if (symid == iden) 
	{
        int isVec = 0;
        char names[MAXIDENLEN];
        strcpy(names, markstr.c_str());
        char place2[100];
        getsym();
        //����ֵ��䣾 ::=  ����ʶ�����������ʽ�� | ����ʶ����'['<���ʽ>']'�������ʽ��
        if (symid == lmbrasy)
		{ //����
            getsym();
            expression();
            strcpy(place2, exprevalue);
            //����Խ���� char/int
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
                error(assignconst);            //"="����ǲ��Ϸ��ı�ʶ��
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
        //�����������
        if (symid == lsbrasy) 
		{
            getsym();
            valueofpara();  //���㲢��¼������ֵ�Լ�����
			if (Rsbrasy(symid, 1)) return;
            //�麯����
            s = searchst(names, 1);
            inmidcode("call", names, "  ", "  ");
            getsym();
			if (Semicolonsy(symid, 2)) return;
            getsym();
        }
        else 
		{
            error(unacceptablesentence);    //���Ϸ��ľ���
			cout << "Unacceptable sentence." << endl;   //���Ϸ��ľ���
            return;
        }
		cout << "This is a function diaoyong sentence!" << endl;
        return;
    }
    //�����
    if (symid == scanfty) 
	{
        scanfsentence();
		if (Semicolonsy(symid, 2))  return; 
        getsym();
		cout << "This is a scanf sentence!" << endl;
        return;
    }
    //д���
    if (symid == printfty) 
	{
        printfsentence();
		if (Semicolonsy(symid, 2))  return; 
        getsym();
		cout << "This is a printf sentence!" << endl;
        return;
    }
    //�������
    if (symid == returnty) 
	{
        returnsentence();
        //��������ȱ������
		if (Semicolonsy(symid, 2))  return;
        getsym();
		cout << "This is a return sentence!" << endl;
        return;
    }
    //�����
    if (symid == semicolonsy)
    {
        getsym();
		cout << "This is a null sentence!" << endl;
        return;
    }
    //���Ϸ��ľ���
    error(unacceptablesentence);
	cout << "Unacceptable sentence." << endl;
    return;
}
//��������䣾 ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
void complexsentence()
{
    if (symid == constty) 
	{
        conststate();
		cout << "This is a const statement!" << endl;
    }
    //�ж��Ƿ�Ϊ����˵��
    if(symid == intty || symid == charty)
	{
        while(symid == intty || symid == charty)
            varstate();
		cout << "This is a variable statement!" << endl;
    }
    //�����
    sentencelist();
    return;
}
//�������� ::=  ��������
//��������  ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
void parametertable()
{
	int i = 0;     //��¼��������
	int temp;
	int tparakind[MAXPARANUM];
	if (symid == intty || symid == charty)
	{
		do {
			if (symid == commasy) getsym();
			temp = symid;
			defhead();
			kind = (temp == intty) ? paraint : parachar;     //4��ʾΪ��������
			value = 0;
			address = i;  //��ַΪi����������λ��
			paranum = 0;
			pushtable(name, kind, value, address, paranum);  //����������������ű�
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
			i++;//����������һ
		} while (symid == commasy);//����Ƕ��ţ����������Ĳ���
		paranum = i;//��ǰ�Ĳ�������
		updatepara(paranum, tparakind);//���뺯���Ĳ�������
	}
	else if (symid == rsbrasy) return;
	else
	{
		error(rightparentlack);//ICV
		cout << "Missing right parent." << endl;
	}
	return;
}
//������ ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������
void program()
{
    if (symid == constty) 
	{
        conststate();
		cout << "This is a const statement!" << endl;
    }
	int globvari = 0;
    //����������з���ֵ�ĺ�������ͬ������ͷ�������Ԥ��3�����������ж�
    while (symid == intty || symid == charty) 
	{
        Symbol tempid = symid;//�ָ��ֳ���
        char tempch = ch;//�ָ��ֳ���
        int temppin = pin;//�ָ��ֳ���
        Symbol kind = symid;
        getsym();
        strcpy(name, markstr.c_str());//��������
        //�����ʶ���Ϸ�
		if(Iden(symid, 0)) continue;
        getsym();
        //����Ƕ���,������,�ֺţ����Ǳ�������
        if (symid == commasy || symid == lmbrasy || symid == semicolonsy) 
		{
            symid = tempid;
            pin = temppin;
            ch = tempch;
            //�ص�intty��charty������ ������������
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
    //�������岿��ͷ����
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
        //���з���ֵ�������壾  ::=  ������ͷ������(������������)��  ��{����������䣾��}��
        if (symid == intty || symid == charty) 
		{
            defhead();
			if (Lsbrasy(symid, 0)) continue; 
            varcnt = 0;         //��ʱ�������¿�ʼ����
            returnnum = 0;      //���Ƿ���ڷ���ֵ�ı�ǳ�ʼ����0��ʾ�޷���ֵ����δ����return
            kind = function;    //��ʾ��ǰ�ĺ������ӳ��򣬼���������
            value = (symid == intty) ? 1 : 2;          //����ֵ
            address = 0;
            paranum = 0;
            pushtable(name, kind, value, address, paranum);
            strcpy(funcname, name);//����������
            if (temp == intty) inmidcode("func", "int", "  ", funcname);
            if (temp == charty) inmidcode("func", "char", "  ", funcname);
            getsym();
            parametertable();
            //ȱ'('��������')'����С����
			if (Rsbrasy(symid, 0)) continue;
            getsym();
            //ȱ'{'��������䣾'}'���������
			if (Lbbrasy(symid, 0)) continue;
            getsym();
            complexsentence();
            //ȱ'{'��������䣾'}'���Ҵ�����
			if (Rbbrasy(symid, 0)) continue;
            //����ȱ�ٷ���ֵ
            if (returnnum == 0) 
			{
                error(nonereturn);//ICV
				cout << "No return." << endl;
                continue;
            }
			cout << "This is a return function statement!" << endl;
            getsym();
            inmidcode("end", "  ", "  ", funcname);//����������Ԫʽ����
            popst();//��մ��Ӻ����ķ��ű�,������Ӻ�����
        }
        //���޷���ֵ�������壾 ::= void����ʶ������(������������)����{����������䣾��}��
        if (symid == voidty) 
		{
            getsym();
            if (symid != iden && symid !=mainty) 
			{
                error(identifierlack);//ICV
				cout << "Missing identifer." << endl;
                continue;
            }
            varcnt = 0;//��ʱ�������¿�ʼ����
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
    //ȱ��main�ؼ���
    if(symid != mainty)
	{
        error(mainlack);
		cout << "Missing main." << endl;
    }
    varcnt = 0;        //��ʱ�������¿�ʼ����
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