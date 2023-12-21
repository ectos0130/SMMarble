//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE		100

static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
		"강의"
		"식당"
		"실험실"
		"집"
		"실험실로 이동"
		"음식찬스"
		"축제시간"
};

char* smmObj_getTypeName(int type)
{
	return (char*)smmNodeName[type];
} 

typedef enum smmObjGrade
{
	smmObjGrade_Ap = 0,
	smmObjGrade_A0 = 1,
	smmObjGrade_Am = 2,
	smmObjGrade_Bp = 3,
	smmObjGrade_B0 = 4,
	smmObjGrade_Bm = 5,
	smmObjGrade_Cp = 6,
	smmObjGrade_C0 = 7,
	smmObjGrade_Cm = 8
} smmObjGrade_e;

//1. 구조체 형식 정의
typedef struct smmObject
{
	char name[MAX_CHARNAME];
	smmObjType_e objType;
	int type;
	int credit;
	int energy;
	smmObjGrade_e grade;
} smmObbjet_t;

//2. 구조체 배열 변수 정의
static smmObject_t smm_node[MAX_NODE];
static int smmObj_noNode = 0;

//3. 관련 함수 변경
//object generation
void smmObj_getNode(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{
	smmObject_t* ptr;
	
	ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
	
	strcpy(ptr -> name, name);
	ptr -> objType = objType;
	ptr -> type = type;
	ptr -> credit = credit;
	ptr -> energy = energy;
	ptr -> grade = grade;
	
	return ptr;
}

//3. 관련 함수 변경 
char* smmObj_getNodeName(void* obj)
{
 	smmObject_t* ptr = (smmObject_t*)obj;
 	
	return ptr -> name;
}

int smmObj_getNodeType(int* type)
{
	smmObject_t* ptr = (smmObject_t*)type;
	
	return ptr -> type;
}

int smmObj_getNodeCredit(int* credit)
{
	smmObject_t* ptr = (smmObject_t*)credit;
	
	return ptr -> credit;
}

int smmObj_getNodeEnergy(int* energy)
{
	smmObject_t* ptr = (smmObject_t*)energy;
	
	return ptr -> energy;
}
