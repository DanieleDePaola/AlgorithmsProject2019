#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


#define HASHDEFAULTDIM 2048
#define BUFFERMAXDIM 1000
#define DIMENTITYHASHTABLE 8192
#define DIMENTITYNAME 100
#define DIMCOMMAND 10
#define FIELDSINADDREL 10

//GLOBAL VARIABLES AND PROTOTYPES

typedef struct r{
    char* nameOtherEntity;
    struct r* otherRelation;
    struct r* previous;
    struct r* next;
}relation;

typedef struct elb{
    char* name;
    int* countIn;
    struct elb* previous;
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

typedef struct trn{
    char* nameTypeRelation;
    int counterIn; //TODO I can use a  pointer here
    relation* relationIn;
    relation* relationOut;
    entityLeaderBoard* leaderboardPosition;
    typeRelationLeaderboard* typeInLeaderBoard;

    struct trn* next;

}typeRelationNormal;

typedef struct e{  //TODO I can remove this intermediate structure
    char* name;
    typeRelationNormal* relationsType;
}entity;

entity* entityVector[DIMENTITYHASHTABLE];

int entityVectorLenght = DIMENTITYHASHTABLE;






entityLeaderBoard* memoryTypeLeaderboard;
typeRelationNormal* memoryTypeEntity;



char * buffer;

void addEntity(char* nameEntity);
void deleteEntity(char* nameEntity);
char* findEntityName(char* buffer);
void createEntityStructure(entity** pointerToModify, char* entityName );

void defineRelationFields( char**relSrcFinal, char**relDstFinal, char**relTypeFinal,char* buffer);
void addRelation(char *relSrc,char *relDst, char* relType);

leaderboard addRelationTypeLeaderboard(leaderboard* myLeaderboard, char *nameRelation, int counterIn);
void addRelationLeaderBoard(leaderboard myTypeRelation, char* competitorName, int* competitorCountsIn);
typeRelationNormal* insertTypeRelationToEntity(typeRelationNormal** relationTypes, char* nameRelation);
relation* insertRelationNode(relation** listOfRelationsInorOut,int inOrOut, char* relSrc, char* relDst);
int* addRelationToEntity(char* relSrc, char*relDst, char* relType);

void cleanSystemFromEntity(relation** headOfList, relation* currentRelationNode, typeRelationNormal*currentTypeRelation);
void report();
void deleteRelation(char* relSrc, char* relDst, char* relType);
int findNewWinCount(entityLeaderBoard* currentLeaderBoardHead);











int main() {


    //Data acquisition

    char* buffer = malloc(BUFFERMAXDIM);
    char* command = malloc(DIMCOMMAND);
    char* entityName=NULL;
    char* relSrc=NULL;
    char* relDst=NULL;
    char* relType=NULL;

    while(fgets(buffer,BUFFERMAXDIM, stdin)){

        sscanf(buffer, "%s",command);
        if(!strcmp(command, "addent")) {
            entityName=findEntityName(buffer);
            addEntity(entityName);
            entityName=NULL;
        }
        else if (!strcmp(command, "addrel")) {

            defineRelationFields(&relSrc, &relDst, &relType, buffer);
            addRelation(relSrc, relDst, relType);
        }
        else if (!strcmp(command, "delent")) {
            deleteEntity(entityName);
        }
        else if (!strcmp(command, "report")) {
            report();
        }

        else if (!strcmp(command, "delrel")) {
            defineRelationFields(&relSrc, &relDst, &relType, buffer);
            deleteRelation(relSrc, relDst, relType);
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

//_______________________________________________________________________________________________
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
 * Add rel function, creates also link between the single entity relation node and the node in ht leaderboard
 * and between the type of leaderboard and the type of entity
 *
 * @param nameRelation the name for the relation
 */
void addRelation(char* relSrc,char* relDst, char* relType){

    //check if the entity exists
    if(entityVector[hashValue(relSrc,entityVectorLenght)]!=NULL&&entityVector[hashValue(relDst,entityVectorLenght)]!=NULL){
        //check if the relation already exists inside the following function
        int* counterIn=addRelationToEntity(relSrc, relDst, relType);
        typeRelationLeaderboard* returnType= addRelationTypeLeaderboard(&myLeaderBoard, relType,*counterIn);
        if(*counterIn>=returnType->winCount){
            addRelationLeaderBoard(returnType,relDst,counterIn);//TODO this can't always happen, I need an if on it
        }

        memoryTypeEntity->leaderboardPosition=memoryTypeLeaderboard; //connection between  entity and leaderboard. For each type I know if the
                                                                     //Player is in position and in which position
        memoryTypeEntity->typeInLeaderBoard=returnType; //Connection between the types of leaderBoard and entity
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
leaderboard addRelationTypeLeaderboard(leaderboard* myLeaderboard, char*nameRelation,int counterIn){
    typeRelationLeaderboard* current=NULL;
    typeRelationLeaderboard* previous=NULL;
    typeRelationLeaderboard* newType=NULL;
    typeRelationLeaderboard* returnType=NULL;
    if(*myLeaderboard==NULL){
        newType= malloc(sizeof(typeRelationLeaderboard));
        newType->nameTypeRelation=nameRelation;
        newType->winCount=counterIn;
        newType->next=NULL;
        newType->entities=NULL;
        *myLeaderboard=newType;
        returnType=newType;
    } else{
        previous=*myLeaderboard;
        current=*myLeaderboard;
        newType= malloc(sizeof(typeRelationLeaderboard));
        newType->winCount=counterIn;
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
void addRelationLeaderBoard(leaderboard myTypeRelation, char* competitorName, int* competitorCountsIn){
    entityLeaderBoard* newCompetitor;
    entityLeaderBoard* current;
    entityLeaderBoard* previous;
    if(myTypeRelation->entities==NULL){
        newCompetitor=malloc(sizeof(entityLeaderBoard));
        newCompetitor->name=competitorName;
        newCompetitor->countIn=competitorCountsIn;
        newCompetitor->next=NULL;
        newCompetitor->previous=NULL;
        myTypeRelation->entities=newCompetitor;
    } else{
        current=myTypeRelation->entities;
        previous=current;

        while(strcmp(competitorName,current->name)<0){
            previous=current;
            current=current->next;
        }
        if(strcmp(competitorName, current->name)==0){ //when entity is already inside the leaderboard
            //current->countIn++; //TODO CHECK IF IS CORRECT ERROR
            if(myTypeRelation->winCount<*current->countIn)myTypeRelation->winCount=*current->countIn;
            return;
        } else{//when the entity is not in the leaderboard
            newCompetitor=malloc(sizeof(entityLeaderBoard));
            newCompetitor->name=competitorName;
            newCompetitor->countIn=competitorCountsIn;

            newCompetitor->next=current;
            newCompetitor->previous=previous;
            previous->next=newCompetitor;
        }
    }
    memoryTypeLeaderboard=newCompetitor;
}

/**
 * This function adds the relations to both the entities and connects them with a pointer so that
 * the delent and delrel procedure could be simplified
 *
 * @param relSrc relation source entity
 * @param relDst relation destination entity
 * @param relType relation type
 * @return the counter of in relations in the destination entity
 */
int* addRelationToEntity(char* relSrc, char*relDst, char* relType){
    entity* srcEntity = entityVector[hashValue(relSrc, entityVectorLenght)];
    entity* dstEntity = entityVector[hashValue(relDst, entityVectorLenght)];
    typeRelationNormal* relationCurrentTypeSrc = insertTypeRelationToEntity(&srcEntity->relationsType, relType);
    typeRelationNormal* relationCurrentTypeDst = insertTypeRelationToEntity(&dstEntity->relationsType, relType);
    relation* relOut;
    relation* relIn;

    memoryTypeEntity=relationCurrentTypeDst;


    relIn = insertRelationNode(&relationCurrentTypeSrc->relationOut, 1, relSrc, relDst);  //relOut relazione uscente
    relOut = insertRelationNode(&relationCurrentTypeDst->relationIn, 0, relSrc, relDst); //relInr relazione entrante
    relOut->otherRelation=relIn;
    relIn->otherRelation=relOut;
    relationCurrentTypeDst->counterIn++;
    return  &relationCurrentTypeDst->counterIn;




}

/**
 * creates/inserts a new relaton node in the relation type list, alfabetical order is used
 *
 * @param listOfRelationsInorOut  list of out relation or in relation
 * @param inOrOut ==0 if in, 1 if out
 * @param relSrc relation source entity
 * @param relDst relation destination entity
 */
relation* insertRelationNode(relation** listOfRelationsInorOut,int inOrOut, char* relSrc, char* relDst){
    relation* current;
    relation* previous;
    relation* newRelation;
    relation* returnRelation;

    if(*listOfRelationsInorOut==NULL){
        newRelation= (relation*)malloc(sizeof(relation));
        if(!inOrOut)
            newRelation->nameOtherEntity=relSrc;
        else
            newRelation->nameOtherEntity=relDst;

        newRelation->next=NULL;
        newRelation->previous=NULL;
        *listOfRelationsInorOut=newRelation;
        returnRelation=newRelation;
    }


    else{
        current=*listOfRelationsInorOut;
        previous=current;
        if(!inOrOut){
            while (strcmp(relSrc, current->nameOtherEntity)<0){
                previous=current;
                current=current->next;
            }
            if(strcmp(relSrc, current->nameOtherEntity)==0)return current;

            newRelation= (relation*)malloc(sizeof(relation));
            newRelation->nameOtherEntity=relSrc;
            newRelation->next=current;
            newRelation->previous=previous;
            previous->next=newRelation;
        }


        else{
            while(strcmp(relDst, current->nameOtherEntity)<0){
                previous=current;
                current=current->next;
            }

            if(strcmp(relDst, current->nameOtherEntity)==0)return current;

            newRelation= (relation*)malloc(sizeof(relation));
            newRelation->nameOtherEntity=relDst;//TODO CHANGE THIS IN POINTER
            newRelation->next=current;
            newRelation->previous=previous;
            previous->next=newRelation;


        }
        returnRelation = newRelation;
    }
    return returnRelation;
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


//______________________________________________________________________________________________________________
//DELREL FUNCTIONS

//TODO function used to delete an entity
void deleteEntity(char* nameEntity) {
    entity* entityToDelete = entityVector[hashValue(nameEntity, entityVectorLenght)];
    typeRelationNormal* currentTypeRelation=entityToDelete->relationsType;

    relation** RelationNodeIn=&currentTypeRelation->relationIn;//TO track the head of list
    relation* currentRelationNodeIn=currentTypeRelation->relationIn;//to scan the list

    relation** RelationNodeOut= &currentTypeRelation->relationOut;
    relation* currentRelationNodeOut= currentTypeRelation->relationOut;

    while (currentTypeRelation!=NULL){
        while(currentRelationNodeIn!=NULL){
            cleanSystemFromEntity(RelationNodeIn, currentRelationNodeIn ,currentTypeRelation);
            currentRelationNodeIn=currentRelationNodeIn->next;
        }

        while(currentRelationNodeOut!=NULL){
            cleanSystemFromEntity(RelationNodeOut, currentRelationNodeOut, currentTypeRelation);
            currentRelationNodeOut=currentRelationNodeOut->next;
        }
        currentTypeRelation->counterIn=0;
        currentTypeRelation=currentTypeRelation->next;
    }

}

/**
 * It only clean the nodes linked to this relation, the main node will be cleaned in the calling function
 *
 * @param disappearingEntity the entity ehich connections must be cleaned
 */
void cleanSystemFromEntity(relation** headOfList, relation* currentRelationNode, typeRelationNormal*currentTypeRelation){
    relation* current = currentRelationNode;
    relation* next = current->next;
    relation* previous = current->previous;

    entityLeaderBoard* currentLeaderBoard=currentTypeRelation->leaderboardPosition;
    entityLeaderBoard* previosLeaderBoard=currentLeaderBoard->previous;
    entityLeaderBoard* nextLeaderBoard=currentLeaderBoard->next;

    typeRelationLeaderboard* typeRelLeaderboard=currentTypeRelation->typeInLeaderBoard;



    if(previous==NULL)
        *headOfList=current->next;
    else{
        next->previous=previous;
        previous->next=next;
        free(current);
    }

    //deleteLeadeboardEntity

    //if the entity node is the one with more in relations, I must work on the wincount
    if(*currentTypeRelation->leaderboardPosition->countIn==typeRelLeaderboard->winCount){
        int winCountMax=0;
        previosLeaderBoard->next=nextLeaderBoard;
        nextLeaderBoard->previous=previosLeaderBoard;

        free(currentLeaderBoard);

        typeRelLeaderboard->winCount = findNewWinCount(typeRelLeaderboard->entities); //head of list
    }
    else{

        //if the node is in first position
        if(previosLeaderBoard==NULL)typeRelLeaderboard->entities=currentLeaderBoard->next;
        else{
            nextLeaderBoard->previous=previosLeaderBoard;
            previosLeaderBoard->next=nextLeaderBoard;
        }
        free(currentLeaderBoard);
    }

    //If the entities list is empty at the end of these operations I must set wincount to 1
    if(typeRelLeaderboard->entities==NULL)typeRelLeaderboard->winCount=1;
}

/**
 * function used to scan all the nodes in leaderboard after the winner (or one of them) has been deleted
 * in order to find the right wincount
 * @param currentLeaderBoardHead
 * @return
 */
int findNewWinCount(entityLeaderBoard* currentLeaderBoardHead){
    int winCountMax=0;
    if(currentLeaderBoardHead==NULL) winCountMax=1;

    while(currentLeaderBoardHead!=NULL){
        if(*currentLeaderBoardHead->countIn>winCountMax)winCountMax=*currentLeaderBoardHead->countIn;
        currentLeaderBoardHead=currentLeaderBoardHead->next;
    }

    return winCountMax;
}


//___________________________________________________________________________________________________________________________________
//DEL REL FUNCTIONS

//TODO function used to delete a relation
void deleteRelation(char* relSrc, char* relDst, char* relType){
    entity* srcEntity = entityVector[hashValue(relSrc, entityVectorLenght)];
    typeRelationNormal* currentRelTypeEnt=srcEntity->relationsType;
    relation* currentRelation=NULL;

    while(currentRelTypeEnt!=NULL&&strcmp(relType, currentRelTypeEnt->nameTypeRelation)!=0){
        currentRelTypeEnt=currentRelTypeEnt->next;
    }

    if(currentRelTypeEnt==NULL) return;//If the relation does not exists

    currentRelation=currentRelTypeEnt->relationOut;
    while(currentRelation!=NULL&&strcmp(currentRelation->nameOtherEntity, relDst)!=0){
        currentRelation=currentRelation->next;
    }
    if(currentRelation==NULL)return;


    if(currentRelation->previous==NULL){
        currentRelTypeEnt->relationOut=currentRelation->next;
    } else{
        currentRelation->next->previous=currentRelation->previous;
        currentRelation->previous->next=currentRelation->next;
    }

    //OPTIMIZATION: if the player was not in the first position I remove him from leaderboard
    if(currentRelTypeEnt->leaderboardPosition!=NULL && currentRelTypeEnt->typeInLeaderBoard->winCount>currentRelTypeEnt->counterIn){
        if(currentRelTypeEnt->leaderboardPosition->previous==NULL){
            currentRelTypeEnt->typeInLeaderBoard->entities=currentRelTypeEnt->leaderboardPosition->next;
            free(currentRelTypeEnt->leaderboardPosition);
        }
        else{
            currentRelTypeEnt->leaderboardPosition->previous->next=currentRelTypeEnt->leaderboardPosition->next;
            currentRelTypeEnt->leaderboardPosition->next->previous=currentRelTypeEnt->leaderboardPosition->previous;
            free(currentRelTypeEnt->leaderboardPosition);
        }
    }

    //Assign new win count only if the relation was the one that gave victory to that entity
    if(currentRelTypeEnt->typeInLeaderBoard->winCount==currentRelTypeEnt->counterIn){
        currentRelTypeEnt->typeInLeaderBoard->winCount=findNewWinCount(currentRelTypeEnt->typeInLeaderBoard->entities);
    }
    currentRelTypeEnt->counterIn--;


    relation* dstRelation=currentRelation->otherRelation;
    if(dstRelation->previous==NULL){
        dstRelation->nameOtherEntity="Ruopp";
    } else{
        dstRelation->next->previous=dstRelation->previous;
        dstRelation->previous->next=dstRelation->next;
    }


}

//___________________________________________________________________________________________________________________________________
//REPORT FUNCTIONS
//TODO function used to explore the leaderboard and extract the winners
void report(){
    typeRelationLeaderboard* currentTypeLb=myLeaderBoard;
    entityLeaderBoard* currentEntityLb=NULL;

    while(currentTypeLb!=NULL){
        if(currentTypeLb->entities==NULL)continue;

        printf("%c", '"');
        printf("%s", currentTypeLb->nameTypeRelation);
        printf("%c ", '"');

        currentEntityLb=currentTypeLb->entities;
        while (currentEntityLb!=NULL){
            if(*currentEntityLb->countIn==currentTypeLb->winCount){
                printf("%c", '"');
                printf("%s", currentEntityLb->name);
                printf("%c ", '"');
            }
            currentEntityLb=currentEntityLb->next;
        }
        printf("%d", currentTypeLb->winCount);
        printf("%c", ';');
        printf("\n");
        currentTypeLb=currentTypeLb->next;
    }
}




//TODO function used to perform a rehashing
//void rehash(hashtableEntity);
