#include "linklist.h"

/*malloc space for a new List and return address*/
List* newList(void)
{
	List* list = (List*)malloc(sizeof(List));
	list->head = NULL;
    list->tail = NULL;
    list->count = 0;
	return list;
}

/*initialize a list with a singe starting value 
(not used but figured it was worth writing...)*/
List* newList_v(Event* value)
{
	List* list = newList();

    if(value != NULL)
    {
	   addEvent(list, value);
    }
	
	return list;
}


/*malloc space for a new Node and return address*/
Node* newNode(void)
{
	Node* node = (Node*)malloc(sizeof(Node));
	node->next = NULL;
	node->value = NULL;
	
	return node;
}

/*malloc space for a new node and assign it a value*/
Node* newNode_v(Event* value)
{
	Node* node = newNode();
    if(value != NULL)
    {
	   node->value = value;
    }
	return node;
}


/*returns pointer to a node by name - will return pointer
to NULL if node does not exist within the list*/
Node* peekByName(List* theList, char* name)
{
	Node* retVal = NULL;
	
    /*no use trying if there is nothing in the list*/
	if(!isEmpty(theList))
	{  
        /*something to help us end the loop early
        if the node is not at the end of the list*/
		int found = 0;
		Node* currNode = theList->head;
		
        /*until we find what we want or run out of list*/
		while(currNode != NULL && !found)
		{
            /*check whether currNode is the one we want*/
			if(!strcmp(currNode->value->name, name))
			{
				found = 1;
			}
			else
			{
				currNode = currNode->next;
			}
		}
        /*return node if we found it*/
		if(found)
        {
            retVal = currNode;
        }
	}
	
	return retVal;

}

/*add a new event to list  will return 0 on success*/
int addEvent(List* theList, Event* event)
{
    /*value to return*/
    int error = 0;

    /*ensure value is not already in list
    (event names are treated as unique keys)*/
    if (peekByName(theList, event->name) == NULL)
    {
        Node* node = newNode_v(event);
        if(isEmpty(theList))
        {
            /*add event to an empty list*/
            theList->head = theList->tail = node;
        }
        else
        {
            /*add event to the end of a populated list*/
            theList->tail->next = node;
            theList->tail = node;
        }
        
        /*increment list count*/
        theList->count++;
    }
    /*event with same name already exists in list*/
    else
    {
        error = 1;
    }

    return error;
}

/*free an entire list and all nodes/values contained within*/
void freeList(List* theList)
{
    while(!isEmpty(theList))
    {
        freeNode(theList, theList->head);
    }
    
    free(theList);
}

/*remove a node from list and free it from memory*/
void freeNode(List* theList, Node* node)
{
    if(!isEmpty(theList))
    {
        /*only one item in list*/
        if(theList->head->next == NULL)
        {
            theList->head = theList->tail = NULL;
        }
        /*if first item in a list with >1 items*/
        else if(node == theList->head)
        {
            theList->head = theList->head->next;
        }
        /*two or more items in list*/
        else
        {
            /*keep track of previous node to chnage next pointer*/
            Node* prevNode = theList->head;
            
            /*loop until we find the node we want*/
            while(prevNode->next != node)
            {
                prevNode = prevNode->next;
            }
        
            /*assign next node to be previous node's new next node*/
            prevNode->next = node->next;
            
            /*re-assign the tail if last item in list*/
            if(node == theList->tail)
            {
                theList->tail = prevNode;
            }
        }

        /*decrement list count*/
        theList->count--;
    
        freeEvent(node->value);
        free(node);
        
    }
}

/*check if list is empty*/
int isEmpty(List* theList)
{
	return (theList->count == 0);
}


/*call readEvent() from event.c until EOF
to populate list via a specified file 
will return 0 on success*/
int loadListFile(char* fName, List* list)
{
    int fail = 0;

    /*open specified file for reading*/
    FILE* file;
    file = fopen(fName, "r");

    /*empty existing list first if not empty*/
    if(list->count > 0)
    {
        freeList(list);
        list = newList();
    }

    if(file == NULL)
    {
        fail = 1; /*let program know file didn't open*/
    }
    else
    {
        /*keep reading events until end of file*/
        while(fgetc(file)!=EOF)
        {
            Event* event;
            /*while loop condition moved location
            forward one char,move it back again*/
            fseek(file, -1, SEEK_CUR); 
            event = readEvent(file);

            /*add event if it is deemed valid*/
            if(event != NULL)
            {
                if(addEvent(list, event))
                {
                    /*error handling that i'm sure i should have...?*/
                }
            }
        }
        /*done with file, close it*/
        fclose(file);
    }   

    /*let caller know whether successful*/
    return fail;
}


/*save list to file in correct format 
will return 0 on success*/
int saveListFile(char* fName, List* list)
{
    int fail = 0;

    /*open file for writing*/
    FILE* file;
    file = fopen(fName, "w");
    if(file == NULL)
    {
        fail = 1; /*let program know file didn't open*/
    }
    else
    {
        /*cycle through events in list and write them to file*/
        Node* curr = list->head;

        while(curr != NULL)
        {
            writeEvent(file, curr->value);
            curr = curr->next;
        }

        /*done with file, close it*/
        fclose(file);
    }   
     
     /*let caller know whether successful*/    
    return fail;
}

/*nice pretty string formatting of list for ui*/
char* printList(List* list)
{
    char* retStr;

    /*make sure there is something to print*/
    if(!isEmpty(list))
    {
        Node* curr = list->head;
        int strLength = 20;
        retStr = (char*)calloc(sizeof(char), strLength);

        while(curr != NULL)
        {
            char* currStr = printEvent(curr->value);

            if((strlen(currStr) + strlen(retStr)) >= strLength)
            {
                strLength += strlen(currStr);
                retStr = (char*)realloc(retStr, sizeof(char)*strLength);
            }
            retStr = strcat(retStr, currStr);
            free(currStr);
            curr = curr->next;
        }
        retStr = (char*)realloc(retStr, sizeof(char)*(strlen(retStr)+1));
    }
    /*let user know list is empty (incase loading an incorrectly 
    formatted file or deleting all events in a list)*/
    else
    {
        /*need to sprintf it into malloc'd memory
        rather than using const char* to stop 
        program choking when i free the string*/
        retStr = (char*)calloc(sizeof(char), 20);
        sprintf(retStr, "Empty calendar file");
    }
    return retStr;
}
