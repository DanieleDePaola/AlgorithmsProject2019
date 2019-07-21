#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


#define HASHDEFAULTDIM 2048
#define BUFFERMAXDIM 1000
#define DIMENTITYHASHTABLE 1024

//GLOBAL VARIABLEs AND PROTOTYPES
void readingInput();
void performingAlgorithm();
char command[];
char instructionOne[];
char instructionTwo[];

typedef struct n{
    char * name;
    struct n*  nxt;
}node;

/**
 * Atomic relation with source and destination, following the arrow
 */
typedef struct r{
    node* src;
    node* dst;
    struct r*nxt;
}relation;

/**
 * Node of the typeOfRelationList
 */
typedef struct{
    relation* firstRelationIn;
    relation* firstRelationOut;
}typeOfRelation;
/**
 * Entity with name and pointer to the next entity in case of conflict
 */
typedef struct e{
    node* entityInfo;
    typeOfRelation* typeOfRelationVector;
}entity;



typedef struct {

}entityBoxHashed;

char * buffer;

void addEntity(char* nameEntity);
void deleteEntity(char* nameEntity);
void findName(char* stringLocation, char* buffer);

//HashTable structures

entityBoxHashed*  entityHashTable[DIMENTITYHASHTABLE];








int main() {


    //Data acquisition

    char* buffer = malloc(BUFFERMAXDIM);
    char* command = malloc(10);
    char* entityName = malloc(100);

    while(fgets(buffer,BUFFERMAXDIM, stdin)){

        sscanf(buffer, "%s",command);
        if(!strcmp(command, "addent")) {
            findName(entityName, buffer);
            addEntity(entityName);
        }
        else if (!strcmp(command, "delent")) {
            deleteEntity(entityName);
        }
        else if (!strcmp(command, "report")) {
            deleteEntity(entityName);
        }
        else if (!strcmp(command, "addrel")) {
            deleteEntity(entityName);
        }
        else if (!strcmp(command, "delrel")) {
            deleteEntity(entityName);
        }

    }
    return 0;







}

//Other functions

//TODO functiion that takes a global variable an retaes an hash table;
//    void hashCreate(){
//
//    }

void findName(char* stringLocation, char* buffer){
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
}

//TODO function used to hash a string
//void hashValue(char* valueToInsert, hashTableEntity);

//TODO function used to delete an entity
void deleteEntity(char* nameEntity){
    printf("Deleting entity");
}

//TODO function used to delete a relation
void deleteRelation(relation* nameRelation);

//TODO function used to add a relation
void addRelation(char * nameRelation);

//TODO  function used to add an entity
void addEntity(char* nameEntity){

}

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