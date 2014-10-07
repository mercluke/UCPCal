/*
Calendar Assignment

Objective: Read in events from file to a 
"calendar" to be stored in a linked list
utilize provided 'gui.c' and 'gui.h' to
display, save, edit, add and remove
calendar events.

Luke Mercuri
Unix and C Programming 
Semester 2, 2013
*/



#ifndef CALENDAR_H
#define CALENDAR_H
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "gui.h"
#include "gui_luke.h"
#include "event.h"
#include "linklist.h"

/*data we want to pass to buttons
pointer to event list and
pointer to the actual window*/
typedef struct{
	List* list;
	Window* window;
}Data;

/*function pointer for laziness sake
pass these to addButton to specify 
what function to run on click*/
typedef void (*pFunc)(void*);

/*functions we'll actually run*/
void uiLoadButton(void* input);
void uiSaveButton(void* input);
void uiEditButton(void* input);
void uiNewButton(void* input);
void uiDelButton(void* input);

/*i didn't end up using this function,
i only wrote it because i was really tired
one time and i forgot sprintf existed...*/
/*char* subStr(char* string, char del);*/

#endif
