#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


#define HASHDEFAULTDIM 2048
#define BUFFERMAXDIM 1000
#define DIMENTITYHASHTABLE 1024
#define DIMENTITYNAME 100
#define DIMCOMMAND 10
#define FIELDSINADDREL 10

//GLOBAL VARIABLES AND PROTOTYPES

typedef struct r{
    char* nameOtherEntity;
    struct r* next;
}relation;

typedef struct trn{
    char* nameTypeRelation;
    int counterIn; //TODO I can use a  pointer here
    struct trn* next;

}typeRelationNormal;

typedef struct e{  //TODO I can remove this intermediate structure
    char* name;
    typeRelationNormal* relationsType;
}entity;

entity* entityVector[DIMENTITYHASHTABLE];

int entityVectorLenght = DIMENTITYHASHTABLE;

typedef struct elb{
    char* name;
    int countIn;
    struct elb* next;
}entityLeaderBoard;

typedef struct trlb{
    char* nameTypeRelation;
    int winCount;
    struct trlb* next;
}typeRelationLeaderboard;

typedef typeRelationLeaderboard* leaderboard;




char * buffer;

void addEntity(char* nameEntity);
void deleteEntity(char* nameEntity);
char* findEntityName(char* buffer);
void createEntityStructure(entity** pointerToModify, char* entityName );

void defineRelationFields( char* relSrc, char* relDst, char* relType,char* buffer);
void addRelation(char *relSrc,char *relDst, char* relType);

void addRelationTypeLeaderboard(leaderboard myLeaderboard, char *nameRelation);
void addRelationTypeToEntity(char* relSrc, char*relDst);









int main() {


    //Data acquisition

    char* buffer = malloc(BUFFERMAXDIM);
    char* command = malloc(DIMCOMMAND);
    char* entityName=NULL;

    while(fgets(buffer,BUFFERMAXDIM, stdin)){

        sscanf(buffer, "%s",command);
        if(!strcmp(command, "addent")) {
            entityName=findEntityName(buffer);
            addEntity(entityName);
        }
        else if (!strcmp(command, "addrel")) {
            char* relSrc=NULL;
            char* relDst=NULL;
            char* relType=NULL;
            defineRelationFields(relSrc, relDst, relType, buffer);
            addRelation(relSrc, relDst, relType);
        }
        else if (!strcmp(command, "delent")) {
            deleteEntity(entityName);
        }
        else if (!strcmp(command, "report")) {
            deleteEntity(entityName);
        }

        else if (!strcmp(command, "delrel")) {
            deleteEntity(entityName);
        }

    }
    return 0;

}

//Other functions

//GENERAL FUNCTIONS
/**
 * Support function used to collect info by input
 * @param stringLocation the memory portion where the buffer is saved
 * @param buffer the buffer conten
 */
char* findEntityName(char* buffer){
    char* stringLocation = malloc(DIMENTITYNAME);
    int indexBuffer=0;
    int indexMemory=0;
    int flag=0;
    while(buffer[indexBuffer]!='\0'){
        if(buffer[indexBuffer]=='"'&&!flag) flag=1;
        else if(buffer[indexBuffer]=='"'&&!flag)flag=0;
        if(buffer[indexBuffer]!='"'&&buffer[indexBuffer]!='\n'&&buffer[indexBuffer]&&flag){
            stringLocation[indexMemory]=buffer[indexBuffer];
            indexMemory++;
        }

        indexBuffer++;
    }
    stringLocation[indexMemory]='\0';
    return stringLocation;
}

/**
 * Function used to hash all the values to
 * @param valueToInsert the string I want to hash
 * @param hashTableLenght the hashTable lenght
 * @return the index in the hash table for the current value
 */
int hashValue(char* valueToInsert, int hashTableLenght){
    int position=0;
    int result=0;
    while(valueToInsert[position]!='\0'){
        result+= valueToInsert[position]*position;
        position++;
    }
    result = result%hashTableLenght;
    return result;
}

//ADD ENTITY FUNCTIONS

/**
 * Add entity function
 * @param nameEntity the name of the entity I want to add
 */
void addEntity(char* nameEntity){
    int hashedIndex= hashValue(nameEntity,entityVectorLenght);
    if(entityVector[hashedIndex]==NULL){
        createEntityStructure(&entityVector[hashedIndex], nameEntity);
    }
    else{
        if(entityVector[hashedIndex]->name!=nameEntity) {  //if entity does not exists yet
            //solveConflicts();
        }
    }
}

/**
 * Function that creates the entity struct vector
 *
 * @param pointerToModify the the hashindex-cell where I want to save a pointer to the relative entity structure
 * @param entityName
 */
void createEntityStructure(entity** pointerToModify, char* entityName){
    entity* newEntity= (entity*)malloc(sizeof(entity));
    newEntity->name= entityName;
    newEntity->relationsType=NULL;
    *pointerToModify=newEntity;

}


//ADDREL FUNCTIONS

/**
 * Assigns the correct string to
 * @param relSrc relation source entity name
 * @param relDst destination source entity name
 * @param relType relation type
 * @param buffer buffer read in input
 */
void defineRelationFields( char* relSrc, char* relDst, char* relType, char*buffer){
    relSrc = malloc(DIMENTITYNAME); //TODO use less memory here
    relDst = malloc(DIMENTITYNAME);
    relType = malloc(DIMENTITYNAME);
    int indexBuffer=0;
    int flagRelSrcRead=0;
    int flagRelDstRead=0;
    int flagRelTypeRead=0;
    int flagRead=0;

    int indexSrc=0;
    int indexDst=0;
    int indexType=0;



    while(buffer[indexBuffer]!='\0'){
        if(buffer[indexBuffer]=='"') {
            flagRead = !flagRead;

            if (buffer[indexBuffer] == '"' && !flagRelSrcRead && !flagRelDstRead && !flagRelTypeRead&&flagRead)flagRelSrcRead = 1;
            else if (buffer[indexBuffer] == '"' && flagRelSrcRead && !flagRelDstRead&&!flagRead) flagRelDstRead = 1;
            else if (buffer[indexBuffer] == '"' && flagRelDstRead && !flagRelTypeRead&&!flagRead) flagRelTypeRead = 1;
        } else {
            if (buffer[indexBuffer] != '\n' && buffer[indexBuffer] && flagRead && flagRelSrcRead&&!flagRelDstRead) {
                relSrc[indexSrc] = buffer[indexBuffer];
                indexSrc++;
            } else if (buffer[indexBuffer] != '\n' && buffer[indexBuffer] && flagRead && flagRelDstRead&&!flagRelTypeRead) {
                relDst[indexDst] = buffer[indexBuffer];
                indexDst++;
            } else if (buffer[indexBuffer] != '\n' && buffer[indexBuffer] && flagRead && flagRelTypeRead) {
                relType[indexType] = buffer[indexBuffer];
                indexType++;
            }
        }
        indexBuffer++;
    }

    relSrc[indexSrc]='\0';
    relDst[indexDst]='\0';
    relType[indexType]='\0';

    }

/**
 * Add rel function
 * @param nameRelation the name for the relation
 */
void addRelation(char* relSrc,char* relDst, char* relType){
    //TODO check if the relation already exists
    //TODO check if the entities exists
}

void addRelationTypeLeaderboard(leaderboard myLeaderboard, char*nameRelation){
    typeRelationLeaderboard* current=NULL;
    typeRelationLeaderboard* previous=NULL;
    typeRelationLeaderboard* newType=NULL;
    if(myLeaderboard==NULL){
        newType= malloc(sizeof(typeRelationLeaderboard));
        newType->nameTypeRelation=nameRelation;
        newType->winCount=1;
        newType->next=NULL;
    } else{

    }
}
//Add node relation in correct place
void addRelationLeaderBoard();
void addRelationTypeToEntity(char* relSrc, char*relDst);





//TODO function used to delete an entity
void deleteEntity(char* nameEntity){
    printf("Deleting entity");
}

//TODO function used to delete a relation
void deleteRelation(relation* nameRelation);

//TODO function used to explore the leaderboard and extract the winners
void report();

//TODO function used to perform a rehashing
//void rehash(hashtableEntity);

//TODO function used to insert an element at the edge of a list
void listInsertHead();

//TODO function used to delete a list Head
void listDeleteHead();

//TODO function used to find a winner in the leaderboard fo the single relation, it is used by report()
//listName findWinnerRelation();