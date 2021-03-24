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
			break;        //1 ��˫���� ����
		case  unacceptatlechar:
			errclass = donothing;
			rsT << "Unacceptable char." << endl;
			break;        //2 ���ɽ����ַ� ����
		case  singlequoteslack:
			errclass = donothing;
			rsT << "Missing single quotes." << endl;
			break;        //3 �ٵ����� ����
		case  outoftable:
			errclass = quit;
			rsT << "Idtable full." << endl;
			break;        //4 ���ű��� �˳�����
		case  symbolconflict:
			errclass = donothing;
			rsT << "Variable name conflict." << endl;
			break;        //5 ��������ͻ ����
		case  cstdefinefail:
			errclass = cs;
			rsT << "Const definition failed." << endl;
			break;        //6 const����ʧ�� ������һ�����Ż��߷ֺ�
		case  unknowrightsym:
			errclass = (opt == 0 ? cs : opt == 1 ? sfirst : noerror); //7 �Ⱥ��ұ��ַ��Ƿ� �������������������ֲ��Ϸ�������һ��λ��
			rsT << "Illegal rightsym." << endl;
			break;
		case  semicolonlack:
			errclass = (opt == 0 ? icvfirst : opt == 1 ? sentenfirst : opt == 2 ? sfirst : noerror);
			rsT << "Semicolon lack." << endl;//8 ��ʧ; 
			break;
		case  keyworderror:
			errclass = (opt == 0 ? icvfirst : opt == 1 ? clr : opt == 2 ? sentenfirst : opt == 3 ? sfirst : noerror);
			rsT << "Keyword error." << endl;//9 �ؼ��ִ���
			break;
		case  identifierlack:
			errclass = (opt == 0 ? icv : opt == 1 ? clr : opt == 2 ? sentenfirst : opt == 3 ? sfirst : opt == 4 ? bfirst : noerror);
			rsT << "Missing identifer." << endl;//10 ȱ�ٱ�ʶ��
			break;
		case  rightbracklack:
			errclass = (opt == 0 ? bfirst : opt == 1 ? sentenfirst : noerror);
			rsT << "Missing right brack." << endl;//11 ��ʧ]
			break;
		case  functionnotfound:
			errclass = (opt == 0 ? sfirst : opt == 1 ? bfirst : noerror); //12 ����δ����	
			rsT << "Function not define." << endl;
			break;
		case  formalparanumunmatch:
			errclass = (opt == 0 ? donothing : opt == 1 ? sfirst : noerror);  //13 �βθ�����ƥ��			
			rsT << "The number of parameters does not match." << endl;
			break;
		case  varnotdefine:
			errclass = (opt == 0 ? donothing : opt == 1 ? sfirst : noerror); //14 ����δ����			
			rsT << "Variable not define." << endl;
			break;
		case  leftparentlack:
			errclass = (opt == 0 ? icv : opt == 1 ? sfirst : opt == 2 ? bfirst : noerror);//15 ����С����			
			rsT << "Missing left parent." << endl;
			break;
		case  rightparentlack:
			errclass = (opt == 0 ? icv : opt == 1 ? sfirst : opt == 2 ? bfirst : noerror);//16 ����С����
			rsT << "Missing right parent." << endl;
			break;
		case  immlack:
			errclass = (opt == 0 ? sentenfirst : switchchjump);
			rsT << "Missing imm." << endl;
			break;                    //17 ��������
		case  rightbracelack:
			errclass = (opt == 0 ? icv : opt == 1 ? sfirst : noerror); //18 ���Ҵ�����			
			rsT << "Missing right brace." << endl;
			break;
		case  expressionerror:
			errclass = bfirst;
			rsT << "Expression error." << endl;
			break;             // 19 ���ʽ����
		case  leftbracelack:
			errclass = icv;
			rsT << "Missing left brace." << endl;
			break;                       //20 ���������
		case  nonereturn:
			errclass = icv;
			rsT << "No return." << endl;
			break;                          //21 �޷���ֵ
		case  unacceptablesentence:
			errclass = sfirst;
			rsT << "Unacceptable sentence." << endl;
			break;       //22 ���ɽ��ܾ���
		case  assignconst:
			errclass = (opt == 0 ? sfirst : opt == 1 ? sfirst : noerror); //23 ��ֵ����
			rsT << "Assign const." << endl;
			break;
		case  plusminulack:
			errclass = sfirst;
			rsT << "Missing Plus or minus." << endl;
			break;            //24 �ټӺŻ����
		case  mainlack:
			errclass = donothing;
			rsT << "Missing main." << endl;
			break;                     //25 ��main����
		case  moreword:
			errclass = donothing;
			rsT << "Extra characters." << endl;
			break;                     //26 main���������ж����ַ�
		case  defineerror:
			errclass = icv;
			rsT << "Variable define error." << endl;//27 �����������
			break;
		case  funcdefineerror:
			errclass = icv;
			rsT << "Function define error." << endl;//28 �����������
			break;
		case  constnotinit:
			errclass = cs;
			rsT << "Const not init." << endl;
			break;                           //29 ����δ��ʼ��
		case colonlack:
			errclass = switchchjump;
			rsT << "Missing colon." << endl;//30  ȱð��
			break;
		case  typechange:
			errclass = donothing;
			rsT << "Int cannot assigh to char." << endl;//31 ���Ͳ���
			break;
		case  unexpectedchar:
			errclass = donothing;
			rsT << "Unexpected char." << endl;//32 ������Ҫ��char����
			break;
		case paratypenotmatch:
			errclass = donothing;
			rsT << "Para type not match." << endl;//33 �������Ͳ�ƥ��
			break;
		case arrayoutbound:
			errclass = donothing;
			rsT << "Warning: index of array out of boundary." << endl; //34 ����Խ��
			break;
		case zeroerror:
			errclass = donothing;
			rsT << "Number starts with zero." << endl;//35 ������0��ͷ
			break;
		case exclamationmark:
			errclass = donothing;
			rsT << "! without = backward." << endl;// 36������û�е��ں�
			break;
		default:
			errclass = quit;
			rsT << "Unknow error." << endl; //37 δ֪����
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
int Semicolonsy(int symid, int op)//�ֺ�
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