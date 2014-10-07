/*
linklist:
define the structs required for a linklist of events
houses functions for creating, and interfacing with a list
*/

#ifndef LINKLIST_H
#define LINKLIST_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "event.h"

/*point to value we actually want
as well as next node in list*/
typedef struct Node{
        struct Node* next;
        /*this should be a void pointer but... lazy*/
        Event* value;
}Node;

/*point to first and last nodes in list
and track size of list*/
typedef struct{
	Node* head;
	Node* tail;
	int count;
}List;

/*malloc space for a new List and return address*/
List* newList(void);
/*initialize a list with a singe starting value 
(not used but figured it was worth writing...)*/
List* newList_v(Event* value);

/*malloc space for a new Node and return address*/
Node* newNode(void);
Node* newNode_v(Event* value);

/*return a node by supplied name*/
Node* peekByName(List* theList, char* name);

/*call readEvent() from event.c until EOF
to populate list via a specified file 
will return 0 on success*/
int loadListFile(char* fName, List* list);

/*save list to file in correct format 
will return 0 on success*/
int saveListFile(char* fName, List* list);

/*add a new event ot list 
will return 0 on success*/
int addEvent(List* theList, Event* event);

/*free struct and values pointed to*/
void freeNode(List* theList, Node* node);

/*free struct and values pointed to*/
void freeList(List* theList);

/*nice pretty string formatting of list for ui*/
char* printList(List* list);

/*check if list is empty...*/
int isEmpty(List* theList);
#endif

