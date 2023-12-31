//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

typedef struct player
{
	int energy;
	int position;
	char name[MAX_CHARNAME];
	int accumCredit;
	int flag_graduate;
	int exe_lab;
}player_t;

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];


#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif


//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
void generatePlayers(int n, int initEnergy);
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void) //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

void printGrades(int player)
{
	int i;
	void *gradePtr;
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
	}
}

void printPlayerStatus(void)
{
	int i;
	
	for (i=0;i<player_nr;i++)
	{
		printf("%s : credit %i, energy %i, position %i\n",
					cur_player[i].name,
					cur_player[i].accumCredit,
					cur_player[i].energy,
					cur_player[i].position);
	}
}

void generatePlayers(int n, int initEnergy)
{
	int i;
	//n time loop
	for (i=0;i<n;i++)
	{
		//input name
		printf("Input player %i's name : ", i);
		scanf("%s", cur_player[i].name);
		fflush(stdin);
		
		//set position
		//player_position[i] = 0;
		cur_player[i].position = 0;
		
		//set energy
		cur_player[i].energy = initEnergy;
		cur_player[i].accumCredit = 0;
		cur_player[i].flag_graduate = 0;
	}
}

int rolldie(int player)
{
	char c;
	printf("Press any key to roll a die (press g to see grade) : ");
	c = getchar();
	fflush(stdin);
	
#if 1
	if (c == 'g')
		printGrades(player);
#endif
	return (rand()%MAX_DIE +1);
}

//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	//int type = smmObj_getNodeType( cur_player[player].position );
	int type = smmObj_getNodeType( boardPtr );
	char *name = smmObj_getNodeName( boardPtr );
	void *gradePtr;
	int success = 0;
	
	switch(type)
	{
		//case lecture:
		case SMMNODE_TYPE_LECTURE:
			printf("Let's study! You lose Energy %i\n", smmObj_getNodeEnergy(boardPtr));
			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
			cur_player[player].accumCredit += smmObj_getNodeCredit (boardPtr);
			
			//grade generation
			gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit( boardPtr ), 0, rand()%9);
			smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
			
			break;
		
		//case Restaurant
		case SMMNODE_TYPE_RESTAURANT:
		{
			printf("Player %d visited restaurant and gained %d energy.\n", smmObj_getNodeName(boardPtr), smmObj_getNodeEnergy(boardPtr));
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
			smmdb_addTail(LISTNO_NODE + player, boardPtr);
			
			break;
		}
		
		//case Home
		case SMMNODE_TYPE_HOME:
		{
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
			smmdb_addTail(LISTNO_NODE + player, boardPtr);
			printf("Player %d is home and gains %d energy.\n", smmObj_getNodeName(boardPtr), smmObj_getNodeEnergy(boardPtr));
			
			break;
		}
		
		//case Laboratory
		case SMMNODE_TYPE_LABORATORY:
		{
			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
			if (cur_player[player].exe_lab == 1)
			{
				int i = rolldie(player);
				if(i >= success)
				{
					cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
					printf("Player %d's experiment is success.\n", smmObj_getNodeName(boardPtr));
					cur_player[player].exe_lab = 0;
				}
				else
				{
					printf("Player %d's experiment is fail.\n");
				}
			break;
			}
		}
		
		//case Gotolab
		case SMMNODE_TYPE_GOTOLAB:
		{
			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
			success = rand()%MAX_DIE+1;
			smmdb_addTail(LISTNO_NODE + player, boardPtr);
			
			break;
		}
		
		//case Festival
		case SMMNODE_TYPE_FESTIVAL:
		{
			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
			rolldie(player);
			printf("%s\n", smmObj_getNodeName(smmdb_getData(LISTNO_FESTCARD, rand()%MAX_DIE)));
			smmdb_addTail(LISTNO_FESTCARD + player, boardPtr);
			
			break;
		}
		
		//case Foodchance
		case SMMNODE_TYPE_FOODCHANCE:
		{
			cur_player[player].energy += smmObj_getNodeCredit(boardPtr);
			cur_player[player].accumCredit += smmObj_getNodeEnergy(smmdb_getData(LISTNO_FOODCARD, rand()%food_nr));
			smmdb_addTail(LISTNO_NODE + player, boardPtr);
			
			break;
		}
		
		default:
			break;
	}
}
	

void goForward(int player, int step)
{
	void *boardPtr;
	cur_player[player].position += step;
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
	
	printf("%s go to node %i (name : %s)\n",
			cur_player[player].name, cur_player[player].position, smmObj_getNodeName(boardPtr));				
}


int isGraduated(void)
{
	int i;
	for (i=0;i<player_nr;i++)
	{
		if (cur_player[i].accumCredit >= GRADUATE_CREDIT)
			cur_player[i].flag_graduate = 1;
	}
	return cur_player[i].flag_graduate = 0;
}

float calcAverageGrade(int player)
{
	int i;
	float averageGrade;
	void *gradePtr;
	
	for(i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		averageGrade = averageGrade + smmObj_getNodeGrade(gradePtr);
	}
	return averageGrade;
}

int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn = 0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4) //read a node parameter set
    {
        //store the parameter set
        //(char *name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
        	initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    for (i = 0;i<board_nr;i++)
    {
    	void *boardObj = smmdb_getData(LISTNO_NODE, i);
    	
    	printf("node %i : %s, %i(%s), credit %i, energy %i\n",
					i, smmObj_getNodeName(boardObj), smmObj_getNodeType(boardObj),
					smmObj_getTypeName(smmObj_getNodeType(boardObj)),
					smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
	}
    printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    
    
    #if 0
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    char foodName[MAX_CHARNAME];
    int replenishEnergy;
    while (fscanf(fp, "%s %d", foodName, &replenishEnergy) == 2) //read a food parameter set
    {
    	food_nr++;//store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    char festCardString[MAX_CHARNAME];
    while (fgets(festCardString, sizeof(festCardString), fp) != NULL) //read a festival card string
    {
        // 줄바꿈 문자 제거
        festCardString[strcspn(festCardString, "\n")] = 0;
        // 여기에서 festCardString을 사용하여 축제 카드 객체 생성 및 추가
        festival_nr++;
		//store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    #endif
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no. : ");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);
    
    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
	generatePlayers(player_nr, initEnergy);
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
    {
    	int die_result;
    	
        //4-1. initial printing
        printPlayerStatus(); //모든플레이어의 현재 상태 출력 
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        printf("die result : $i\n", cur_player[turn].position);
        
        //4-3. go forward
        goForward(turn, die_result); //플레이어를 주사위 눈금만큼 이동
		printf("Player position : %i\n", cur_player[turn].position); 
	
		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    
    free(cur_player);
    return 0;
}
