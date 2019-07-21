#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


#define HASHDEFAULTDIM 2048
#define BUFFERMAXDIM 1000


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







int main() {


    //Data acquisition
//    while (fscanf(stdin, "%s %s %s",  &command, &instructionOne, &instructionTwo)==1||fscanf(stdin, "%s" "%s" "%s",  &command, &instructionOne, &instructionTwo)==2||fscanf(stdin, "%s" "%s" "%s",  &command, &instructionOne, &instructionTwo)==3){
//        printf(command);
//        printf(instructionOne);
//        printf(instructionTwo);
//        printf("\n");
//        fflush(stdout);
//
//    }

    char* string = malloc(BUFFERMAXDIM);
    printf("ciao bello");
    while(fgets(string,BUFFERMAXDIM, stdin)){
        printf("%s", string);

    }
    return 0;


    //Other functions

    //TODO function used to hash a string
    //void hashValue(char* valueToInsert, hashTableEntity);

    //TODO function used to delete an entity
    void deleteEntity(entity* nameEntity);

    //TODO function used to delete a relation
    void deleteRelation(relation* nameRelation);

    //TODO function used to add a relation
    void addRelation(char * nameRelation);

    //TODO  function used to add an entity
    void addEntity(char* nameEntity);

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




}