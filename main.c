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
    relation* relationIn;
    relation* relationOut;
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
    entityLeaderBoard* entities;
    struct trlb* next;
}typeRelationLeaderboard;

typedef typeRelationLeaderboard* leaderboard;
leaderboard myLeaderBoard=NULL;




char * buffer;

void addEntity(char* nameEntity);
void deleteEntity(char* nameEntity);
char* findEntityName(char* buffer);
void createEntityStructure(entity** pointerToModify, char* entityName );

void defineRelationFields( char** relSrc, char** relDst, char** relType,char* buffer);
void addRelation(char *relSrc,char *relDst, char* relType);

leaderboard addRelationTypeLeaderboard(leaderboard* myLeaderboard, char *nameRelation);
void addRelationLeaderBoard(leaderboard myTypeRelation, char* competitorName, int competitorCountsIn);
typeRelationNormal* insertTypeRelationToEntity(typeRelationNormal** relationTypes, char* nameRelation);
void insertRelationNode(relation** listOfRelationsInorOut,int inOrOut, char* relSrc, char* relDst);
int addRelationToEntity(char* relSrc, char*relDst, char* relType);










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
            defineRelationFields(&relSrc, &relDst, &relType, buffer);
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
void defineRelationFields( char** relSrcFinal, char** relDstFinal, char** relTypeFinal, char*buffer){
    char* relSrc = malloc(DIMENTITYNAME); //TODO use less memory here
    char* relDst = malloc(DIMENTITYNAME);
    char* relType = malloc(DIMENTITYNAME);
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

    *relSrcFinal=relSrc;
    *relDstFinal=relDst;
    *relTypeFinal=relType;

    }

/**
 * Add rel function
 * @param nameRelation the name for the relation
 */
void addRelation(char* relSrc,char* relDst, char* relType){

    //TODO add pointers in the relation nodes to delete faster
    typeRelationLeaderboard* returnType=NULL;

    //check if the entity exists
    if(entityVector[hashValue(relSrc,entityVectorLenght)]!=NULL&&entityVector[hashValue(relDst,entityVectorLenght)]!=NULL){
        //check if the relation already exists inside the following function
        returnType= addRelationTypeLeaderboard(&myLeaderBoard, relType);

        int counterIn=addRelationToEntity(relSrc, relDst, relType);
        addRelationLeaderBoard(returnType,relDst,counterIn);


    }



}

/**
 * The function returns the correct type of relation node in the leaderboard
 * in case it does not exist, the function creates a new node
 *
 * @param myLeaderboard the pointer to the leaderboard
 * @param nameRelation the type of relation I want to add
 * @return the pointer to the relation type node
 */
leaderboard addRelationTypeLeaderboard(leaderboard* myLeaderboard, char*nameRelation){
    typeRelationLeaderboard* current=NULL;
    typeRelationLeaderboard* previous=NULL;
    typeRelationLeaderboard* newType=NULL;
    typeRelationLeaderboard* returnType=NULL;
    if(*myLeaderboard==NULL){
        newType= malloc(sizeof(typeRelationLeaderboard));
        newType->nameTypeRelation=nameRelation;
        newType->winCount=1;
        newType->next=NULL;
        newType->entities=NULL;
        *myLeaderboard=newType;
        returnType=newType;
    } else{
        previous=*myLeaderboard;
        current=*myLeaderboard;
        newType= malloc(sizeof(typeRelationLeaderboard));
        newType->nameTypeRelation=nameRelation;
        newType->entities=NULL;
        newType->next=NULL;

        while(strcmp(nameRelation,current->nameTypeRelation)<0){
            previous=current; //TODO and if current==NULL?
            current=current->next;
        }

        if(strcmp(nameRelation,current->nameTypeRelation)==0) {
            free(newType);
            newType=NULL;
            return current;
        }

        newType->next=current;
        previous->next=newType;
        returnType=newType;
        newType=NULL;
    }

    return returnType;
}

/**
 * Add the entities in leaderboard in alfabetical order
 * @param myTypeRelation
 * @param competitorName
 * @param competitorCountsIn
 */
void addRelationLeaderBoard(leaderboard myTypeRelation, char* competitorName, int competitorCountsIn){
    entityLeaderBoard* newCompetitor;
    entityLeaderBoard* current;
    entityLeaderBoard* previous;
    if(myTypeRelation->entities==NULL){
        newCompetitor=malloc(sizeof(entityLeaderBoard));
        newCompetitor->name=competitorName;
        newCompetitor->countIn=1;
        newCompetitor->next=NULL;
        myTypeRelation->entities=newCompetitor;
    } else{
        current=myTypeRelation->entities;
        previous=current;

        while(strcmp(competitorName,current->name)<0){
            previous=current;
            current=current->next;
        }
        if(strcmp(competitorName, current->name)==0){ //when entity is already inside the leaderboard
            current->countIn++;
            if(myTypeRelation->winCount<current->countIn)myTypeRelation->winCount=current->countIn;
            return;
        } else{//when the entity is not in the leaderboard
            newCompetitor=malloc(sizeof(entityLeaderBoard));
            newCompetitor->name=competitorName;
            newCompetitor->countIn=competitorCountsIn;
            newCompetitor->next=NULL;

            newCompetitor->next=current;
            previous->next=newCompetitor;
        }
    }
}

int addRelationToEntity(char* relSrc, char*relDst, char* relType){
    entity* srcEntity = entityVector[hashValue(relSrc, entityVectorLenght)];
    entity* dstEntity = entityVector[hashValue(relDst, entityVectorLenght)];
    typeRelationNormal* relationCurrentTypeSrc = insertTypeRelationToEntity(&srcEntity->relationsType, relType);
    typeRelationNormal* relationCurrentTypeDst = insertTypeRelationToEntity(&dstEntity->relationsType, relType);

    insertRelationNode(&relationCurrentTypeSrc->relationOut, 0, relSrc, relDst);
    insertRelationNode(&relationCurrentTypeDst->relationIn, 1, relSrc, relDst);
    relationCurrentTypeDst->counterIn++;
    return relationCurrentTypeDst->counterIn;




}

/**
 * creates/inserts a new relaton node in the relation type list, alfabetical order is used
 *
 * @param listOfRelationsInorOut  list of out relation or in relation
 * @param inOrOut ==0 if in, 1 if out
 * @param relSrc relation source entity
 * @param relDst relation destination entity
 */
void insertRelationNode(relation** listOfRelationsInorOut,int inOrOut, char* relSrc, char* relDst){
    relation* current;
    relation* previous;
    relation* newRelation;

    if(*listOfRelationsInorOut==NULL){
        newRelation= (relation*)malloc(sizeof(relation));
        if(!inOrOut)
            newRelation->nameOtherEntity=relSrc;//TODO CHANGE THIS IN POINTER
        else
            newRelation->nameOtherEntity=relDst;

        newRelation->next=NULL;
    }
    else{
        current=*listOfRelationsInorOut;
        previous=current;
        if(!inOrOut){
            while (strcmp(relSrc, current->nameOtherEntity)<0){
                previous=current;
                current=current->next;
            }
            if(strcmp(relSrc, current->nameOtherEntity)==0)return;

            newRelation= (relation*)malloc(sizeof(relation));
            newRelation->nameOtherEntity=relSrc;//TODO CHANGE THIS IN POINTER
            newRelation->next=current;
            previous->next=newRelation;

        }
        else{
            while(strcmp(relDst, current->nameOtherEntity)<0){
                previous=current;
                current=current->next;
            }

            if(strcmp(relDst, current->nameOtherEntity)==0)return;

            newRelation= (relation*)malloc(sizeof(relation));
            newRelation->nameOtherEntity=relSrc;//TODO CHANGE THIS IN POINTER
            newRelation->next=current;
            previous->next=newRelation;
        }

    }
}

/**
 * Inserts or creates the type of relation for the single entity
 *
 * @param relationTypes  the pointer to entity relation type list
 * @param nameRelation the name for the relation
 * @return the pointer to the relation type node in the entity relation type list
 */
typeRelationNormal* insertTypeRelationToEntity(typeRelationNormal** relationTypes, char* nameRelation){
    typeRelationNormal* current=NULL;
    typeRelationNormal* previous=NULL;
    typeRelationNormal* newType=NULL;
    typeRelationNormal* returnType=NULL;
    if(*relationTypes==NULL){
        newType= malloc(sizeof(typeRelationNormal));
        newType->nameTypeRelation=nameRelation;
        newType->counterIn=0;
        newType->next=NULL;
        newType->relationIn=NULL;
        newType->relationOut=NULL;
        *relationTypes=newType;
        returnType=newType;
    } else{
        previous= *relationTypes;
        current= *relationTypes;

        while(strcmp(nameRelation,current->nameTypeRelation)<0){
            previous=current; //TODO and if current==NULL?
            current=current->next;
        }

        if(strcmp(nameRelation,current->nameTypeRelation)==0) {
            free(newType);
            newType=NULL;
            return current;
        }

        newType= malloc(sizeof(typeRelationNormal));
        newType->nameTypeRelation=nameRelation;
        newType->counterIn=0;
        newType->next=NULL;
        newType->relationIn=NULL;
        newType->relationOut=NULL;


        newType->next=current;
        previous->next=newType;
        returnType=newType;
        newType=NULL;
    }

    return returnType;
}





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