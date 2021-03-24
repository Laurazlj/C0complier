#ifndef global_h
#define global_h

#include <iostream>
#include <fstream>
#include <vector>

#define MAXIDENLEN 50  //最长标识符长度
#define MAXSYMTABLENUM 200 //符号表元素最多个数
#define MAXMIDCODE 1000
#define MAXPARANUM 20
#define INITSTACK 0x7fffeffc
#define INITDATA 0x10010000
using namespace std;

typedef enum {
	none, mainty, intty, charty, constty, voidty, ifty, elsety, switchty, casety,//0-9
	defaultty, forty, printfty, scanfty, returnty, //10-14 保留字类别
	assignsy, plussy, subsy, multsy, divsy, lessy, leseqsy, grtsy, grteqsy, nequalsy,//15-24 
	equalsy, lsbrasy, rsbrasy, lmbrasy, rmbrasy, lbbrasy, rbbrasy, commasy, colonsy, semicolonsy,//25-34 标识符类别
	numet, charet, stringet, iden, cintet, ccharet, //35-40 数字类型 char类型 string类型 iden标识符 const int类型 const char类型
	Const, //41 常量
	variable, //42 变量
	function, //43 函数
	paraint, //44 int参数
	parachar, //45 char参数
}Symbol;

typedef struct{
    char op[10];
    char var1[200];
    char var2[30];
    char var3[30];
}midcodee;

typedef enum {
	doublequoteslack,
	unacceptatlechar,
	singlequoteslack,
	outoftable,
	symbolconflict,
	cstdefinefail,
	unknowrightsym,
	semicolonlack,
	keyworderror, 
	identifierlack,
	rightbracklack,
	functionnotfound,
	formalparanumunmatch, 
	varnotdefine,
	leftparentlack,
	rightparentlack, 
	immlack,
	rightbracelack,
	expressionerror, 
	unacceptablesentence,
	assignconst, 
	leftbracelack,
	nonereturn,
	plusminulack, 
	mainlack, 
	moreword, 
	constnotinit,
	colonlack,
	typechange,
	defineerror,
	funcdefineerror,
	unexpectedchar,
	paratypenotmatch,
	arrayoutbound,
	zeroerror,
	exclamationmark
}err;

typedef enum {
	noerror,
	quit,
	donothing,
	icv,
	icvfirst,
	cs,
	clr,
	sentenfirst,
	sfirst,
	bfirst,
	switchchjump
}errcls;

typedef struct {
	char name[100];
	int address;
	Symbol kind;
	int cnt;
}tempvaraddress;//变量表
extern tempvaraddress addrtable[1000];

typedef struct {
	char name[100];
	Symbol kind;
	int startap;
	int endap;
}funcclass;
extern vector<funcclass> fc;

extern string str;
extern int pin;
extern char ch;
extern int linenum;
extern Symbol symid;
extern string markstr;
extern int num;
extern string stret;
extern int varcnt;
extern int midcnt;
extern midcodee midcode [MAXMIDCODE];
extern int OPT;
extern ofstream mipsWrite,errWrite;
extern int errnum;
extern int mid;
extern int savespflg;
extern int isglob;
extern int sp;
extern int funcnum;
extern int tlabelnum;
extern int varreg[200];

void error(err errsig, int opt = 0);
int Rsbrasy(int symid, int op);
int Semicolonsy(int symid, int op);
int Iden(int symid, int op);
int Rmbrasy(int symid, int op);
int Lsbrasy(int symid, int op);
int Lbbrasy(int symid, int op);
int Rbbrasy(int symid, int op);

void getsym();
void conststate();
void constdef(Symbol ctpe);
void defhead();
void vardef();
void varstate();
void scanfsentence();
void printfsentence();
void expression();
void item();
void factor();
void valueofpara();
void returnsentence();
void ifsentence();
void condition();
void loopsentence();
void casesentence();
void sentence();
void sentencelist();
void complexsentence();
void program();
void parametertable();

void inmidcode(const char* op,const  char* t1,const char* t2,const char* t3);
char * nextreg();
char * nextlab();

void Tomips();
void funcasm();
void jmpasm();
void callasm();
void setlabasm();
void addasm();
void subasm();
void mulasm();
void divasm();
void greasm();
void geqasm();
void lssasm();
void leqasm();
void eqlasm();
void neqasm();
void assasm();
void aassasm();
void assaasm();
void scfasm();
void prtasm();
void fupaasm();
void retasm();
void paraasm();
void jneasm();
void intcharasm();
void constdefasm();
void intcharaasm();

int varaddrt(char *name);
int varaddr(char *name);
int findvartable(char *name);

void callbetter();
void addbetter();
void subbetter();
void mulbetter();
void divbetter();
void grebetter();
void geqbetter();
void lssbetter();
void leqbetter();
void eqlbetter();
void neqbetter();
void assbetter();
void aassbetter();
void assabetter();
void scfbetter();
void prtbetter();
void fupabetter();
void retbetter();

void load();
void save();

void better();
void delcommon();
void combine();
void delsetlab();

#endif /* global_h */