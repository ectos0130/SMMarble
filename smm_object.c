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

//1. 구조체 형식 정의
typedef struct smmObject
{
	char name[MAX_CHARNAME];
	int type;
	int credit;
	int energy;
} smmObbjet_t;

//2. 구조체 배열 변수 정의
static smmObject_t smm_node[MAX_NODE];

ststic int smmObj_noNode = 0;

//3. 관련 함수 변경
//object generation
void smmObj_getNode(char* name, int type, int credit, int energy)
{
	strcpy(smm_node[smmObj_noNode].name, name);
	smm_node[smmObj_noNode].type = type;
	smm_node[smmObj_noNode].credit = credit;
	smm_node[smmObj_noNode].energy = energy;
	
	smmObj_noNode++;
}

//3. 관련 함수 변경 
char* smmObj_getNodeName(int node_nr)
{
 	return smmObj_name[node_nr];
}

int smmObj_getNodeType(int node_nr)
{
	return smm_node[node_nr].type;
}

int smmObj_getNodeCredit(int node_nr)
{
	return smm_node[node_nr].credit;
}

int smmObj_getNodeEnergy(int node_nr)
{
	return smm_node[node_nr].energy;
}
