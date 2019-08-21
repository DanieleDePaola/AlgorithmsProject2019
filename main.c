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
#define DELTAREASHING 313
#define MAXREHASHINGATTEMPTS 10

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
    struct trlb* previous;
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
    struct trn* previous;
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
int findHashedValues(char* stringToFind, int hashTableLenght);
void deleteEntity(char* nameEntity);
void deleteEntityOld(char* nameEntity);
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
relation* deleteRelationInSrc(entity* srcEntity, char*relType, char* relDst);
void deleteRelationInDst(relation* dstNode,char* relType, typeRelationNormal* relationTypesEntityDst);
int findNewWinCount(entityLeaderBoard* currentLeaderBoardHead);
relation* insertRelationNodeInSrc(relation** listOfRelationsInorOut, char* relSrc, char* relDst);
relation* insertRelationNodeInDst(relation** listOfRelationsInorOut, char* relSrc, char* relDst);


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
        }
        else if (!strcmp(command, "addrel")) {

            defineRelationFields(&relSrc, &relDst, &relType, buffer);
            addRelation(relSrc, relDst, relType);
        }
        else if (!strcmp(command, "delent")) {
            entityName=findEntityName(buffer);
            deleteEntity(entityName);
        }
        else if (!strcmp(command, "report")) {
            report();
        }

        else if (!strcmp(command, "delrel")) {
            defineRelationFields(&relSrc, &relDst, &relType, buffer);
            deleteRelation(relSrc, relDst, relType);
        }

//        else if(!strcmp(command, "break")){
//            printf("stop");
//        }
        else if (!strcmp(command, "end"))
            break;


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
            if(entityVector[hashedIndex]->name!=NULL&&strcmp(entityVector[hashedIndex]->name, nameEntity)==0)
                return;
           //if entity does not exists yet
            int countMaxRehash=0;
            //TODO implement a more effective strategy for reashing

            //TODO V0 removed entityVector[hashedIndex]->name!=null in condition
            while(entityVector[hashedIndex]!=NULL){

                if(entityVector[hashedIndex]->name!=NULL&&strcmp(entityVector[hashedIndex]->name, nameEntity)==0)
                    return; //TODO V0 fix: the addEnt function must not work in case the entity already exists

                countMaxRehash++;
                if(countMaxRehash>MAXREHASHINGATTEMPTS)
                    printf("IT WAS NOT POSSIBLE TO INSERT A NEW ELEMENT IN TABLE, MAX ATTEMPTS REACHED!");

                hashedIndex= hashedIndex+DELTAREASHING;

                if(hashedIndex>=entityVectorLenght)
                    hashedIndex=hashedIndex-entityVectorLenght;

            }
            createEntityStructure(&entityVector[hashedIndex], nameEntity);


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


int findHashedValues(char* stringToFind, int hashTableLenght){
    int position=0;
    int result=0;
    while(stringToFind[position]!='\0'){
        result+= stringToFind[position]*position;
        position++;
    }
    result = result%hashTableLenght;
    if(entityVector[result]!=NULL&&entityVector[result]->name!=NULL&&strcmp(entityVector[result]->name, stringToFind)==0)
        return result;
    else{
        int countMaxRehash=0;
        while((entityVector[result]!=NULL&&entityVector[result]->name==NULL)||(entityVector[result]!=NULL&&entityVector[result]->name!=NULL&&strcmp(entityVector[result]->name, stringToFind)!=0)){
        //while(!(entityVector[result]!=NULL&&entityVector[result]->name!=NULL&&strcmp(entityVector[result]->name, stringToFind)==0)){
            countMaxRehash++;
            if(countMaxRehash>MAXREHASHINGATTEMPTS) {
                printf("ERROR IN FIND HASHING");
                return 0;
            }
            result= result+DELTAREASHING;
            if(result>=entityVectorLenght)result=result-entityVectorLenght;
        }
        return result;
    }

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
    entity* entitySrc = entityVector[findHashedValues(relSrc,entityVectorLenght)];
    entity* entityDst = entityVector[findHashedValues(relDst,entityVectorLenght)];
    if(entitySrc!=NULL&&entityDst!=NULL&&entitySrc->name!=NULL&&entityDst->name!=NULL){
        //check if the relation already exists inside the following function
        int* counterIn = addRelationToEntity(relSrc, relDst, relType);
        if(counterIn==0)return; //If the relation has been added yet

        typeRelationLeaderboard* returnType= addRelationTypeLeaderboard(&myLeaderBoard, relType,*counterIn);

        //TODO V0 choice1: insertion only the elements  with a certain wincount in leaderboard
//        if(*counterIn>=returnType->winCount){
//        if(*counterIn>=returnType->winCount){
//            addRelationLeaderBoard(returnType,relDst,counterIn);
//
//            //TODO V0
//            //the if tries to avoid the case in which the dest is already in leaderboard, in that case a wrong leaderboard position assignment was spotted
//
//            if(memoryTypeEntity->leaderboardPosition==NULL || strcmp(memoryTypeLeaderboard->name,memoryTypeEntity->leaderboardPosition->name)==0)
//                memoryTypeEntity->leaderboardPosition=memoryTypeLeaderboard; //connection between  entity and leaderboard. For each type I know if the
//                                                                             //Player is in position and in which position
//            memoryTypeEntity->typeInLeaderBoard=returnType; //Connection between the types of leaderBoard and entity
//
//        } else{
//
//            //TODO V0
//            memoryTypeEntity->leaderboardPosition=NULL;
//            memoryTypeEntity->typeInLeaderBoard=returnType;
//        }

        //TODO V0 choice2: insert all the elements with at least one relation in in the leaderboard
        addRelationLeaderBoard(returnType,relDst,counterIn);


        //the if tries to avoid the case in which the dest is already in leaderboard, in that case a wrong leaderboard position assignment was spotted

        if(memoryTypeEntity->leaderboardPosition==NULL || strcmp(memoryTypeLeaderboard->name,memoryTypeEntity->leaderboardPosition->name)==0)
            memoryTypeEntity->leaderboardPosition=memoryTypeLeaderboard; //connection between  entity and leaderboard. For each type I know if the
                                                                        //Player is in position and in which position
        memoryTypeEntity->typeInLeaderBoard=returnType; //Connection between the types of leaderBoard and entity

        //TODO end choice 2

        if(*counterIn>returnType->winCount)
            returnType->winCount=*counterIn;


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
leaderboard addRelationTypeLeaderboard(leaderboard* myLeaderboard, char*nameRelation, int counterIn){
    typeRelationLeaderboard* current=NULL;
    typeRelationLeaderboard* previous=NULL;
    typeRelationLeaderboard* newType=NULL;
    typeRelationLeaderboard* returnType=NULL;
    if(*myLeaderboard==NULL){
        newType= malloc(sizeof(typeRelationLeaderboard));
        newType->nameTypeRelation=nameRelation;
        newType->winCount=counterIn;
        newType->next=NULL;
        newType->previous=NULL;
        newType->entities=NULL;
        *myLeaderboard=newType;
        returnType=newType;
    } else{
        previous=NULL;
        current=*myLeaderboard;

        while(current!=NULL&&strcmp(nameRelation,current->nameTypeRelation)>0){
            previous=current; //TODO and if current==NULL?
            current=current->next;
        }

        if(current!=NULL&&strcmp(nameRelation,current->nameTypeRelation)==0) {
            free(newType);
            return current;
        }

        newType= malloc(sizeof(typeRelationLeaderboard));
        newType->winCount=counterIn;
        newType->nameTypeRelation=nameRelation;
        newType->entities=NULL;
        newType->next=current;
        newType->previous=previous;
        if(previous!=NULL){
            previous->next=newType;
        } else *myLeaderboard=newType;
        returnType=newType;

        if(current!=NULL)current->previous=newType;
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
        previous=NULL;

        while(current!=NULL&&strcmp(competitorName,current->name)>0){
            previous=current;
            current=current->next;
        }
        if(current!=NULL&&strcmp(competitorName, current->name)==0){ //when entity is already inside the leaderboard
            //current->countIn++;
            if(myTypeRelation->winCount<*current->countIn)myTypeRelation->winCount=*current->countIn;
            memoryTypeLeaderboard=current; //TODO CHECK IF IS CORRECT ERROR
            return;
        } else{//when the entity is not in the leaderboard
            newCompetitor=malloc(sizeof(entityLeaderBoard));
            newCompetitor->name=competitorName;
            newCompetitor->countIn=competitorCountsIn;

            newCompetitor->next=current;
            newCompetitor->previous=previous;
            if(previous!=NULL)previous->next=newCompetitor;
            else myTypeRelation->entities=newCompetitor;

            if(current!=NULL)current->previous=newCompetitor;

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
    entity* srcEntity = entityVector[findHashedValues(relSrc, entityVectorLenght)];
    entity* dstEntity = entityVector[findHashedValues(relDst, entityVectorLenght)];
    relation* relOut;
    relation* relIn;

    typeRelationNormal* relationCurrentTypeSrc = insertTypeRelationToEntity(&srcEntity->relationsType, relType);
    typeRelationNormal* relationCurrentTypeDst = insertTypeRelationToEntity(&dstEntity->relationsType, relType);

    memoryTypeEntity=relationCurrentTypeDst;

    //TODO in case the relation already exists the program has problems
    relIn = insertRelationNodeInSrc(&relationCurrentTypeSrc->relationOut, relSrc, relDst);//relOut relazione uscente
    if(relIn==NULL) return 0;  //The relation exist yet

    relOut = insertRelationNodeInDst(&relationCurrentTypeDst->relationIn, relSrc, relDst); //relInr relazione entrante

    relOut->otherRelation=relIn;
    relIn->otherRelation=relOut;
    relationCurrentTypeDst->counterIn++;

    return  &relationCurrentTypeDst->counterIn;




}


relation* insertRelationNodeInSrc(relation** listOfRelationsInorOut, char* relSrc, char* relDst){
    relation* current;
    relation* previous;
    relation* newRelation;
    relation* returnRelation;

    if(*listOfRelationsInorOut==NULL){
        newRelation= (relation*)malloc(sizeof(relation));
        newRelation->nameOtherEntity=relDst;
        newRelation->next=NULL;
        newRelation->previous=NULL;
        *listOfRelationsInorOut=newRelation;
        returnRelation=newRelation;
    }

    else{
        current=*listOfRelationsInorOut;
        previous=NULL;


        while(current!=NULL&&strcmp(relDst, current->nameOtherEntity)>0){
            previous=current;
            current=current->next;
        }

        if(current!=NULL&&strcmp(relDst, current->nameOtherEntity)==0) {
            return NULL;  //TODO check if is correct, In case relation exists yet
        }

        newRelation= (relation*)malloc(sizeof(relation));
        newRelation->nameOtherEntity=relDst;
        newRelation->next=current;
        newRelation->previous=previous;
        if(current!=NULL)current->previous=newRelation;

        if(previous!=NULL)previous->next=newRelation;
        else *listOfRelationsInorOut=newRelation;

        returnRelation = newRelation;
    }
    return returnRelation;
}
relation* insertRelationNodeInDst(relation** listOfRelationsInorOut, char* relSrc, char* relDst){
    relation* current;
    relation* previous;
    relation* newRelation;
    relation* returnRelation;

    if(*listOfRelationsInorOut==NULL){
        newRelation= (relation*)malloc(sizeof(relation));
        newRelation->nameOtherEntity=relSrc;

        newRelation->next=NULL;
        newRelation->previous=NULL;
        *listOfRelationsInorOut=newRelation;
        returnRelation=newRelation;
    }


    else{
        current=*listOfRelationsInorOut;
        previous=NULL;
        //if(!inOrOut){
        while (current!=NULL&&strcmp(relSrc, current->nameOtherEntity)>0){
            previous=current;
            current=current->next;
        }
        if(current!=NULL&&strcmp(relSrc, current->nameOtherEntity)==0){
            return  current;
        }


        newRelation= (relation*)malloc(sizeof(relation));
        newRelation->nameOtherEntity=relSrc;
        newRelation->next=current;
        newRelation->previous=previous;
        if(current!=NULL)current->previous=newRelation;

        if(previous!=NULL){
            previous->next=newRelation;
        }
        else{
            *listOfRelationsInorOut=newRelation;
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
        newType->previous=NULL;
        newType->relationIn=NULL;
        newType->relationOut=NULL;
        newType->leaderboardPosition=NULL;
        newType->typeInLeaderBoard=NULL;
        *relationTypes=newType;
        returnType=newType;
    } else{
        previous = NULL;
        current = *relationTypes;

        while(current!=NULL&&strcmp(nameRelation,current->nameTypeRelation)>0){
            previous=current; //TODO and if current==NULL?
            current=current->next;
        }

        if(current!=NULL && strcmp(nameRelation,current->nameTypeRelation)==0) {
            free(newType);
            return current;
        }

        newType= malloc(sizeof(typeRelationNormal));
        newType->nameTypeRelation=nameRelation;
        newType->counterIn=0;
        newType->next=current;
        newType->previous=previous;
        newType->relationIn=NULL;
        newType->relationOut=NULL;
        newType->typeInLeaderBoard=NULL; //TODO v0 to avoid unconditional jump
        newType->leaderboardPosition=NULL; //TODO v0 to avoid unconditional jump

        if(previous!=NULL)previous->next = newType;
        else *relationTypes=newType;
        if(current!=NULL)current->previous=newType;

        returnType=newType;
    }

    return returnType;
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
        previous=NULL;
        if(!inOrOut){
            while (current!=NULL&&strcmp(relSrc, current->nameOtherEntity)>0){
                previous=current;
                current=current->next;
            }
            if(current!=NULL&&strcmp(relSrc, current->nameOtherEntity)==0){
                return  current;
            }


            newRelation= (relation*)malloc(sizeof(relation));
            newRelation->nameOtherEntity=relSrc;
            newRelation->next=current;
            newRelation->previous=previous;
            if(previous!=NULL)previous->next=newRelation;
            else *listOfRelationsInorOut=newRelation;

        }


        else{
            while(current!=NULL&&strcmp(relDst, current->nameOtherEntity)>0){
                previous=current;
                current=current->next;
            }

            if(current!=NULL&&strcmp(relDst, current->nameOtherEntity)==0)return current;

            newRelation= (relation*)malloc(sizeof(relation));
            newRelation->nameOtherEntity=relDst;//TODO CHANGE THIS IN POINTER
            newRelation->next=current;
            newRelation->previous=previous;
            if(previous!=NULL)previous->next=newRelation;
            else *listOfRelationsInorOut=newRelation;


        }
        returnRelation = newRelation;
    }
    return returnRelation;
}

//______________________________________________________________________________________________________________
//DELREL FUNCTIONS

void deleteEntity(char *nameEntity){
    entity* entityToDelete = entityVector[findHashedValues(nameEntity, entityVectorLenght)];
    if(entityToDelete==NULL)return;

    typeRelationNormal* currentTypeRelation=entityToDelete->relationsType;
    if(currentTypeRelation==NULL){
        if(entityToDelete->name!=NULL){
            free(entityToDelete->name);
            entityToDelete->name=NULL; //TODO V0 when I find an entity with type relation null I must delete also the name
        }
        return;
    }


    relation* currentRelationNodeIn; //to scan the list
    relation* currentRelationNodeOut;
    relation* nodeMemory=NULL;
    typeRelationNormal* memoryTypeEntityWoW;

    while (currentTypeRelation!=NULL){
        currentRelationNodeIn = currentTypeRelation->relationIn;//to scan the list
        currentRelationNodeOut = currentTypeRelation->relationOut;


        if(currentRelationNodeOut!=NULL&&currentRelationNodeOut->nameOtherEntity!=NULL&&strcmp(currentRelationNodeOut->nameOtherEntity, entityToDelete->name)==0)
            currentRelationNodeOut=currentRelationNodeOut->next;

        //Clean all the relation in nodes
        while(currentRelationNodeIn!=NULL){
            nodeMemory=currentRelationNodeIn->next;
            deleteRelation(currentRelationNodeIn->nameOtherEntity, entityToDelete->name, currentTypeRelation->nameTypeRelation);
            currentRelationNodeIn=nodeMemory;  //The free is done by the delrel function

        }

        //Clean all the relation out nodes
        while(currentRelationNodeOut!=NULL){
            nodeMemory=currentRelationNodeOut->next;
            deleteRelation(entityToDelete->name, currentRelationNodeOut->nameOtherEntity,currentTypeRelation->nameTypeRelation );
            currentRelationNodeOut=nodeMemory; //The free is done by the delrel function
        }

        //FREE ENTITY NODE IN LEADERBOARD
        currentTypeRelation->counterIn=0;

        if(currentTypeRelation->leaderboardPosition!=NULL){
            if(currentTypeRelation->leaderboardPosition->previous==NULL){
                currentTypeRelation->typeInLeaderBoard->entities=currentTypeRelation->leaderboardPosition->next;
                if (currentTypeRelation->leaderboardPosition->next!=NULL)currentTypeRelation->leaderboardPosition->next->previous=currentTypeRelation->leaderboardPosition->previous;

            } else{
                currentTypeRelation->leaderboardPosition->previous->next=currentTypeRelation->leaderboardPosition->next;
                if (currentTypeRelation->leaderboardPosition->next!=NULL)currentTypeRelation->leaderboardPosition->next->previous=currentTypeRelation->leaderboardPosition->previous;
            }

            //FREE TYPE RELATION LEADERBOARD  //TODO V0
            if(currentTypeRelation->typeInLeaderBoard->entities==NULL){

                if(currentTypeRelation->typeInLeaderBoard->previous==NULL){
                    myLeaderBoard=currentTypeRelation->typeInLeaderBoard->next;
                } else{
                    currentTypeRelation->typeInLeaderBoard->previous->next=myLeaderBoard = currentTypeRelation->typeInLeaderBoard->next;
                    if(currentTypeRelation->typeInLeaderBoard->next!=NULL)currentTypeRelation->typeInLeaderBoard->next->previous=currentTypeRelation->typeInLeaderBoard->previous;
                }
                free(currentTypeRelation->typeInLeaderBoard);
            }


        }

        //FREE TYPE REL IN ENTITY
        memoryTypeEntityWoW=currentTypeRelation->next;
        if(currentTypeRelation->previous==NULL){
            entityToDelete->relationsType=currentTypeRelation->next;
            if(currentTypeRelation->next!=NULL)currentTypeRelation->next->previous=currentTypeRelation->previous;
        } else{
            currentTypeRelation->previous->next=currentTypeRelation->next;
            if(currentTypeRelation->next!=NULL)currentTypeRelation->next->previous=currentTypeRelation->previous;
        }
        free(currentTypeRelation);

        currentTypeRelation=memoryTypeEntityWoW;



    }
    char** nameMemory = &entityToDelete->name;
    free(entityToDelete->name);

    entityToDelete->name=NULL;
    entityToDelete->relationsType=NULL;
    free(nameEntity); //TODO V1 free the variable used for the names
    //free(entityToDelete);  //TODO v0, if I FREE this I can have strange values
    //Entity to delete is not null, otherwise I must rehash

}

//OLD DEPRECATED
void deleteEntityOld(char* nameEntity) {
    entity* entityToDelete = entityVector[findHashedValues(nameEntity, entityVectorLenght)];
    typeRelationNormal* currentTypeRelation=entityToDelete->relationsType;

    relation** RelationNodeIn=&currentTypeRelation->relationIn;//TO track the head of list
    relation* currentRelationNodeIn=currentTypeRelation->relationIn;//to scan the list

    relation** RelationNodeOut= &currentTypeRelation->relationOut;
    relation* currentRelationNodeOut= currentTypeRelation->relationOut;

    while (currentTypeRelation!=NULL){

        //Clean all the relation occurrencies in other entities and in leaderboard
        while(currentRelationNodeIn!=NULL){
            cleanSystemFromEntity(RelationNodeIn, currentRelationNodeIn ,currentTypeRelation);
            currentRelationNodeIn=currentRelationNodeIn->next;  //TODO Change this with a free or something

        }

        //Clean all the out relation
        while(currentRelationNodeOut!=NULL){
            cleanSystemFromEntity(RelationNodeOut, currentRelationNodeOut, currentTypeRelation);
            currentRelationNodeOut=currentRelationNodeOut->next; //TODO change this with a free or something
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

    if(previous==NULL) {
        *headOfList = current->next;
        free(current);
        current->previous=NULL;
        current->nameOtherEntity=NULL;
        current->next=NULL;
        current->otherRelation=NULL;

    }else{
        next->previous=previous;
        previous->next=next;
        free(current);
        current->previous=NULL;
        current->nameOtherEntity=NULL;
        current->next=NULL;
        current->otherRelation=NULL;
    }

    //deleteLeadeboardEntity

    //if the entity node is the one with more relations, I must work on the wincount
    if(*currentTypeRelation->leaderboardPosition->countIn==typeRelLeaderboard->winCount){
        int winCountMax=0;
        previosLeaderBoard->next=nextLeaderBoard;
        if(nextLeaderBoard!=NULL)nextLeaderBoard->previous=previosLeaderBoard;

        currentLeaderBoard->next=NULL;
        currentLeaderBoard->previous=NULL;
        currentLeaderBoard->name=NULL;
        currentLeaderBoard->countIn=NULL;
        currentTypeRelation->leaderboardPosition=NULL;

        free(currentLeaderBoard);

        currentTypeRelation->leaderboardPosition=NULL;


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
    if(typeRelLeaderboard->entities==NULL)
        typeRelLeaderboard->winCount=1;
}

/**
 * function used to scan all the nodes in leaderboard after the winner (or one of them) has been deleted
 * in order to find the right wincount
 * @param currentLeaderBoardHead
 * @return
 */
int findNewWinCount(entityLeaderBoard* currentLeaderBoardHead){
    int winCountMax=0;
    if(currentLeaderBoardHead==NULL) winCountMax=0;

    while(currentLeaderBoardHead!=NULL){
        if(*currentLeaderBoardHead->countIn>winCountMax)winCountMax=*currentLeaderBoardHead->countIn;
        currentLeaderBoardHead=currentLeaderBoardHead->next;
    }

    return winCountMax;
}


//___________________________________________________________________________________________________________________________________
//DEL REL FUNCTIONS

/**
 * The function deletes the source and destination node for the relation, when the destination
 * relation node is deleted, the leaderboard is updated
 * @param relSrc the name of source entity
 * @param relDst the name of destinatio entity
 * @param relType the name of the relation
 */
void deleteRelation(char* relSrc, char* relDst, char* relType){
    entity* srcEntity = entityVector[findHashedValues(relSrc, entityVectorLenght)];
    entity* dstEntity = entityVector[findHashedValues(relDst,entityVectorLenght)]; //TODO this could drain a lot of time

    if(srcEntity==NULL||dstEntity==NULL)return;
    relation* dstNode; //Destination node, return value of function

    //TODO CHECK IF THE RELATION IS DELETED IN DST

    //I delete the relation node from the relation source entity
    dstNode=deleteRelationInSrc(srcEntity, relType, relDst);

    if(dstNode==NULL)
        return; //No relation found

    //I delete the relation node from the relation  dest entity and manage the leaderboard
    if(dstEntity->relationsType==NULL)
        return;


    deleteRelationInDst(dstNode,relType,dstEntity->relationsType);

}

relation* deleteRelationInSrc(entity* srcEntity, char*relType, char* relDst){

    typeRelationNormal* currentRelTypeEnt=srcEntity->relationsType;
    relation* currentRelation=NULL;//Source relation node, deleted at the end of function
    relation* dstNode; //Destination node, return value of function

    while(currentRelTypeEnt!=NULL&&strcmp(relType, currentRelTypeEnt->nameTypeRelation)!=0){
        currentRelTypeEnt=currentRelTypeEnt->next;
    }
    //TODO I CAN USE OPTIMIZATION, alfabetical order must be used to do it faster
    if(currentRelTypeEnt==NULL) return NULL;//If the relation does not exists

    currentRelation=currentRelTypeEnt->relationOut;
    while(currentRelation!=NULL&&strcmp(currentRelation->nameOtherEntity, relDst)!=0){
        currentRelation=currentRelation->next;
    }
    if(currentRelation==NULL)
        return NULL;//if the relation does not exists


    //I link the previous and next nodes before the current node is deleted
    if(currentRelation->previous==NULL){
        currentRelTypeEnt->relationOut=currentRelation->next;
        if(currentRelation->next!=NULL)currentRelation->next->previous=currentRelation->previous; //TODO V0
    } else{
        if(currentRelation->next!=NULL)currentRelation->next->previous=currentRelation->previous;
        currentRelation->previous->next=currentRelation->next;
    }

    if(currentRelation->next==NULL&&currentRelation->previous==NULL)
        currentRelTypeEnt->relationOut=NULL; //TO SOLVE PROBLEMS IN DELETING LAST ENTITY

    dstNode=currentRelation->otherRelation;
    dstNode->otherRelation=NULL;
    currentRelation->otherRelation=NULL;
    currentRelation->previous=NULL;
    currentRelation->next=NULL;
    currentRelation->nameOtherEntity=NULL;
    free(currentRelation);


    return dstNode;
}

void deleteRelationInDst(relation* dstNode,char* relType,  typeRelationNormal* relationTypesEntityDst){
    //I prepare the entity structure for the deletion of the relation

    while(relationTypesEntityDst!=NULL&&strcmp(relType, relationTypesEntityDst->nameTypeRelation)!=0){
        relationTypesEntityDst=relationTypesEntityDst->next;
    }

    if(relationTypesEntityDst==NULL) return ;//If the relation does not exists

    if(dstNode->previous==NULL){
        if(dstNode->next!=NULL)dstNode->next->previous=dstNode->previous;
        relationTypesEntityDst->relationIn=dstNode->next;
    } else{
        if(dstNode->next!=NULL)dstNode->next->previous=dstNode->previous;
        dstNode->previous->next=dstNode->next;
    }

    //Deleting the node in dst node in sources
    if(dstNode->next==NULL&&dstNode->previous==NULL)
        relationTypesEntityDst->relationIn=NULL;

    dstNode->next=NULL;
    dstNode->previous=NULL;
    dstNode->nameOtherEntity=NULL;
    dstNode->otherRelation=NULL;
    free(dstNode);

    //OPTIMIZATION: if the player was not in the first position I remove him from leaderboard

    //TODO V0 remove an element only if the element has wincount == 0
//    if(relationTypesEntityDst->leaderboardPosition!=NULL&&relationTypesEntityDst->typeInLeaderBoard->winCount>relationTypesEntityDst->counterIn){
//
//        entityLeaderBoard* entityLbToDelete=relationTypesEntityDst->leaderboardPosition; //leaderboard entity to delete
//        relationTypesEntityDst->leaderboardPosition=NULL;
//        entityLeaderBoard* previousEntityLb=entityLbToDelete->previous;
//        entityLeaderBoard* nextEntityLb=entityLbToDelete->next;
//
//        if(previousEntityLb==NULL){
//            relationTypesEntityDst->typeInLeaderBoard->entities=nextEntityLb;
//            if(nextEntityLb->previous!=NULL)nextEntityLb->previous=NULL;
//            free(entityLbToDelete);
//        }
//        else{
//            previousEntityLb->next=entityLbToDelete->next;
//            if(nextEntityLb!=NULL)nextEntityLb->previous=previousEntityLb;
//            free(entityLbToDelete);
//        }
//    }

    //TODO V0, added relation type !=NULL to avoid SEGFAULT
    //OPTIMIZATION: IF THE PLAYER HAS WINCOUNT ==0 I REMOVE HIM FROM LEADERBOARD
    if(relationTypesEntityDst!=NULL&&relationTypesEntityDst->leaderboardPosition!=NULL&&relationTypesEntityDst->leaderboardPosition->countIn!=NULL&&*relationTypesEntityDst->leaderboardPosition->countIn==1){

        entityLeaderBoard* entityLbToDelete=relationTypesEntityDst->leaderboardPosition; //leaderboard entity to delete
        relationTypesEntityDst->leaderboardPosition=NULL;
        entityLeaderBoard* previousEntityLb=entityLbToDelete->previous;
        entityLeaderBoard* nextEntityLb=entityLbToDelete->next;

        if(previousEntityLb==NULL){
            relationTypesEntityDst->typeInLeaderBoard->entities=nextEntityLb;
            if(nextEntityLb!=NULL&&nextEntityLb->previous!=NULL)nextEntityLb->previous=NULL;
            free(entityLbToDelete);
        }
        else{
            previousEntityLb->next=entityLbToDelete->next;
            if(nextEntityLb!=NULL)nextEntityLb->previous=previousEntityLb;
            free(entityLbToDelete);
        }
    }

    relationTypesEntityDst->counterIn--;

    //TODO V0 added relation type entity dst.typeIn ln leaderboard != NULL to avoid segfault
    //IF THE LEADERBOARD TYPE IS EMPTY AFTER A DELETION, I REMOVE THE TYPE FROM LEADERBOARD
    if(relationTypesEntityDst->typeInLeaderBoard!=NULL&&relationTypesEntityDst->typeInLeaderBoard->entities==NULL){
        typeRelationLeaderboard* currentType= relationTypesEntityDst->typeInLeaderBoard;
        typeRelationLeaderboard* previousType= currentType->previous;
        typeRelationLeaderboard* nextType=currentType->next;
        if(previousType==NULL){
            myLeaderBoard=nextType;
            if(nextType!=NULL&&nextType->previous!=NULL)nextType->previous=NULL;
            free(currentType);
            relationTypesEntityDst->typeInLeaderBoard=NULL; //TODO V0 added to avoid segfault
            relationTypesEntityDst->leaderboardPosition=NULL; //TODO V0 added to avoid segfault
        }
        else{
            previousType->next=nextType;
            if(nextType!=NULL)nextType->previous=previousType;
            free(currentType);
            relationTypesEntityDst->typeInLeaderBoard=NULL; //TODO V0 added to avoid segfault
            relationTypesEntityDst->leaderboardPosition=NULL; //TODO V0 added to avoid segfault
        }
    }


    //TODO V0 added relation type entity dst.typeIn ln leaderboard != NULL to avoid segfault
    //Assign new win count only if the relation was the one that gave victory to that entity
    if(relationTypesEntityDst->typeInLeaderBoard!=NULL&&relationTypesEntityDst->typeInLeaderBoard->winCount==relationTypesEntityDst->counterIn+1){
        relationTypesEntityDst->typeInLeaderBoard->winCount=findNewWinCount(relationTypesEntityDst->typeInLeaderBoard->entities);
    }



}

//___________________________________________________________________________________________________________________________________
//REPORT FUNCTIONS

/**
 * function used to explore the leaderboard and extract the winners
 */
void report(){
    typeRelationLeaderboard* currentTypeLb=myLeaderBoard;
    entityLeaderBoard* currentEntityLb=NULL;

    if(currentTypeLb==NULL){
        printf("none");
        printf("\n");
        return;
    }

    while(currentTypeLb!=NULL){
//        if(currentTypeLb->entities==NULL)continue; //if type relation is empty

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
        printf("%c ", ';');
        currentTypeLb=currentTypeLb->next;
    }
    printf("\n");



}



